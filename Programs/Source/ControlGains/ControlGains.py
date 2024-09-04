##########################################################
##    Author: Pedro Rojo - pedroeroca@outlook.com       ##
##              File: ControlGains.py                   ##
##                    Version 1.0                       ##
##    This program ask you for the constants that       ##
##    you want to program on the internal PID           ##
##                 controlller                          ##
##########################################################
import serial, usb
import serial.tools.list_ports
import atexit, signal
import time
import os

#DEFAULT CONFIGURATIONS (Need recompile to modify it)
BAUDRATE = 115200
PARITY = serial.PARITY_NONE
STOP_BITS = serial.STOPBITS_ONE
BYTE_SIZE = serial.EIGHTBITS
PRODUCT_STRING = "Pneumatic Levitator ITM"

ser = serial.Serial()
portOpened = False

def sigint_handler(signum, frame):
    """Callback executed when the CTRL+C is pressed"""
    print("\r", end="")
    global ser
    global portOpened
    
    if portOpened:
        print("Cerrando " + ser.name)
        ser.close()
    os._exit(0)

def exit_handler():
    """Callback executed when the program finishes"""
    global ser
    global portOpened
    
    if portOpened:
        print("Cerrando " + ser.name)
        ser.close()
        
def find_serial_devices() -> None:
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

def get_product_string(vendor_id, product_id) -> None:
    """Retrieve product and manufacturer strings from a USB device."""
    try:
        # Find the USB device
        device = usb.core.find(idVendor=vendor_id, idProduct=product_id)
        
        if device is None:
            #print(f"No device found with Vendor ID: {vendor_id}, Product ID: {product_id}")
            return None

        # Detach kernel driver if necessary
        if device.is_kernel_driver_active(0):
            try:
                device.detach_kernel_driver(0)
                #print(f"Detached kernel driver from device {vendor_id}:{product_id}")
            except usb.core.USBError as e:
                print(f"Could not detach kernel driver: {e}")
                return None

        # Set the active configuration
        device.set_configuration()

        # Get manufacturer and product strings
        #manufacturer = usb.util.get_string(device, device.iManufacturer)
        product_str = usb.util.get_string(device, device.iProduct)

        # Reattach the kernel driver
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
        #Print the current constants
        ser.write(str.encode("CONST_GET"))
        stringRx = ser.readline().decode()
        stringRx = stringRx.replace("!", "\n")
        print("Constantes actuales:\n" + stringRx)
        #Start the parsing
        notGettedConst = True
        dataToSend = ""
        finishProgram = False
        while not finishProgram:
            while notGettedConst:
                kp = inputConst("Ingrese el valor de Kp: ")
                ki = inputConst("Ingrese el valor de Ki: ")
                kd = inputConst("Ingrese el valor de Kd: ")
                print("")
                print("Confirme que los valores son correctos (S/n)")
                sel = input(f"Kp: {kp}\nKi: {ki}\nKd: {kd}\n\n")
                if sel.lower().strip() == "s" or sel.lower().strip() == "\n":
                    dataToSend = f"{kp},{ki},{kd},"
                    break
                else:
                    print("")
            #Sending the data
            print("Enviando a Pneumatic Levitator")
            constSetted = False
            while not constSetted:    
                ser.write(str.encode("CONST"))
                time.sleep(1e-3) #Wait 1 ms
                ser.write(str.encode(dataToSend))
                stringRx = ser.readline().decode()
                if stringRx[:2] == "ER":
                    print("")
                    print("No se pudieron escribir las constantes")
                    print("Reintentando en 1 segundo")
                    time.sleep(1)
                elif stringRx[:2] == "OK":
                    print("Constantes escritas con exito")
                    constSetted = True
            print("")
            sele = input("Desea reescribir las constantes? (S/n)\n")
            if sele.lower().strip() == "s" or sele.lower().strip() == "\n":
                pass
            else:
                break
        ser.close()
        portOpened = False
    else:
        print("Pneumatic levitator no conectado")