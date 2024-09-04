##########################################################
##    Author: Pedro Rojo - pedroeroca@outlook.com       ##
##              File: PIDController.py                  ##
##    This program prints the the CPU load measure      ##
##            gaved by the freeRTOS kernel              ##
##########################################################
import serial, usb
import serial.tools.list_ports
import atexit, signal
import time
import os, platform
from PID import PID
from threading import Thread

#DEFAULT CONFIGURATIONS (Need recompile to modify it)
BAUDRATE = 115200
PARITY = serial.PARITY_NONE
STOP_BITS = serial.STOPBITS_ONE
BYTE_SIZE = serial.EIGHTBITS
PRODUCT_STRING = "Pneumatic Levitator ITM"

DEFAULT_KP = 0.30
DEFAULT_KI = 0.000015
DEFAULT_KD = 0.002
DEFAULT_SET_POINT = 260

ser = serial.Serial()
portOpened = False
pneumaticReady = False

def sigint_handler(signum, frame):
    global ser
    """Callback executed when the CTRL+C is pressed"""
    print("\r", end="")
    ser.write(str.encode("ACT"))
    time.sleep(0.001)
    ser.write(str.encode(str(int(0))))
    os._exit(0)

def exit_handler():
    """Callback executed when the program finishes"""
    global ser
    global portOpened
    
    if portOpened:
        print("Cerrando " + ser.name)
        ser.close()
        
def find_serial_devices():
    """Find all connected serial devices."""
    ports = serial.tools.list_ports.comports()
    devices = []
    for port in ports:
        devices.append({
            'port': port.device,
            'vendor_id': port.vid,
            'product_id': port.pid
        })
    return devices

def get_product_string(vendor_id, product_id):
    """Retrieve product and manufacturer strings from a USB device."""
    try:
        #Find the USB device
        device = usb.core.find(idVendor=vendor_id, idProduct=product_id)
        
        if device is None:
            #print(f"No device found with Vendor ID: {vendor_id}, Product ID: {product_id}")
            return None

        #Detach kernel driver if necessary
        if device.is_kernel_driver_active(0):
            try:
                device.detach_kernel_driver(0)
                #print(f"Detached kernel driver from device {vendor_id}:{product_id}")
            except usb.core.USBError as e:
                print(f"Could not detach kernel driver: {e}")
                return None

        #Set the active configuration
        device.set_configuration()

        #Get manufacturer and product strings
        #manufacturer = usb.util.get_string(device, device.iManufacturer)
        product_str = usb.util.get_string(device, device.iProduct)

        #Reattach the kernel driver
        try:
            usb.util.dispose_resources(device)
            device.attach_kernel_driver(0)
            #print(f"Reattached kernel driver to device {vendor_id}:{product_id}")
        except usb.core.USBError as e:
            print(f"Could not reattach kernel driver: {e}")
        return product_str
    except usb.core.USBError as e:
        print(f"USB Error: {e}")
        return None

def is_float(element : any) -> bool:
    """Function that checks if a variable it's float"""
    #If you expect None to be passed:
    if element is None: 
        return False
    try:
        float(element)
        return True
    except ValueError:
        return False
    
def is_int(element : any) -> bool:
    """Function that checks if an element can be parsed to int"""
    if element is None:
        return False
    try:
        int(element)
        return True
    except ValueError:
        return False

def inputConst(inputMsg : str) -> str:
    """Function that parses the reception of a control gain"""
    parse = True
    const = input(inputMsg)
    while parse:
        if is_float(const):
            return const
        else:
            print("No se ingreso bien el valor, intente de nuevo")
            const = input()

def inputSetPoint(inputMsg : str) -> str:
    """Function that parses the reception of the set point"""
    parse = True
    setPoint = input(inputMsg)
    while parse:
        if is_int(setPoint):
            return setPoint
        else:
            print("No se ingreso bien el valor, intente de nuevo")
            setPoint = input()

def readSerial():
    """Read the serial port waiting for ready char"""
    global ser
    global pneumaticReady
    stringRx = ""

    #Wait Pneumatic levitator to be ready
    print("Esperando a que el sistema este listo para controlar")
    while not pneumaticReady:
        stringRx = ser.readline().decode()
        if stringRx == "READY":
            print("El sistema esta listo para controlar")
            pneumaticReady = True
        elif stringRx[:5] == "DIST:":
            print("El sistema se adelanto")
            print("Desea usar los valores predeterminados de ganancias(S/n)?")
            pneumaticReady = True

def pidLoop(kp : float, kd : float, ki : float, setPoint : float) -> None:
    """Implements the PID controller"""
    pid = PID(kp, ki, kd, float(setPoint))
    distance  = float(0)
    while True:
        stringRx = ser.readline().decode()
        print(stringRx)
        if stringRx[:5] == "DIST:":
            if is_float(stringRx[6:9]):
                distance = float(stringRx[6:9])
                control = pid.process(distance)
                print("Control: " + str(int(control)))
                ser.write(str.encode("ACT"))
                time.sleep(0.001)
                ser.write(str.encode(str(int(control))))
    
if __name__ == "__main__":
    mcuFound = False
    codePort = ""
    signal.signal(signal.SIGINT, sigint_handler)
    #Resgistering the exit hook
    atexit.register(exit_handler)
    #Searching for the pneumatic levitator
    devices = find_serial_devices()
    for device in devices:
        product_string = get_product_string(device['vendor_id'], device['product_id'])
        if product_string:
            if product_string == PRODUCT_STRING:
                codePort = device['port']
                mcuFound = True
                break
    if mcuFound:
        print("Pneumatic Levitator en puerto: " + codePort, end="\n\n")
        ser = serial.Serial(
            port=codePort,
            baudrate=BAUDRATE,
            parity=PARITY,
            stopbits=STOP_BITS,
            bytesize=BYTE_SIZE
        )
        portOpened = True
        readThread = Thread(target=readSerial)
        readThread.start()
        #Parsing const and setpoint
        kp = DEFAULT_KP
        ki = DEFAULT_KI
        kd = DEFAULT_KD
        setPoint = DEFAULT_SET_POINT
        print(f"Desea usar los valores predeterminados de ganancias(S/n)?\nKp: {DEFAULT_KP}\nKi: {DEFAULT_KI}\nKd: {DEFAULT_KD}")
        answer = input("")
        if answer.lower().strip() == "s":
            setPoint = inputSetPoint("Ingrese al punto de consigna del controlador: ")
        else:
            notGettedConst = True
            while notGettedConst:
                kp = inputConst("Ingrese el valor de Kp: ")
                ki = inputConst("Ingrese el valor de Ki: ")
                kd = inputConst("Ingrese el valor de Kd: ")
                setPoint = inputSetPoint("Ingrese el punto de consiga del controlador: ")
                print("")
                print("Confirme que los valores son correctos (S/n)")
                sel = input(f"Kp: {kp}\nKi: {ki}\nKd: {kd}\nPunto de Consigna: {setPoint}\n\n")
                if sel.lower().strip() == "s" or sel.lower().strip() == "\n":
                    break
                else:
                    print("")
        #Wait for system ready
        while not pneumaticReady:
            pass
        pidThread = Thread(target=pidLoop, args=(float(kp), float(ki), float(kd), float(setPoint)))
        pidThread.start()
        pidThread.join()
    else:
        print("Pneumatic levitator no conectado")