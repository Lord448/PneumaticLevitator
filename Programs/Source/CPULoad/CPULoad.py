import serial, usb
import serial.tools.list_ports
import atexit, signal
import time
import os, platform
from threading import Thread

#DEFAULT CONFIGURATIONS (Need recompile to modify it)
BAUDRATE = 115200
PARITY = serial.PARITY_NONE
STOP_BITS = serial.STOPBITS_ONE
BYTE_SIZE = serial.EIGHTBITS
PRODUCT_STRING = "Pneumatic Levitator ITM"

ser = serial.Serial()
portOpened = False

def sigint_handler(signum, frame):
    print("\r", end="")
    os._exit(0)

def exit_handler():
    global ser
    global portOpened
    
    if portOpened:
        print("Closing " + ser.name)
        ser.close()

def writeSerial():
    global ser
    
    while True:
        ser.write(str.encode("CPU"))
        time.sleep(1)
    
def printHeader(end="\n"):
    print("Task                            Ticks             Percetage")
    print("-----------------------------------------------------------", end=end)
    
def readSerial():
    global ser
    stringRx = ""
    foundEndOfChart = False
        
    printHeader()
    stringRx = ser.readline().decode()
    print(stringRx, end="")
    while True:
        stringRx = ser.readline().decode()
        if stringRx.strip() == "end":
            foundEndOfChart = True
        else:
            if foundEndOfChart:
                if platform.system() == "Linux" or platform.system() == "Darwin": #Darwin for MacOs
                    os.system("clear")
                elif platform.system() == "Windows":
                    os.system("cls")
                foundEndOfChart = False
                printHeader(end="")
                print()
            print(stringRx, end = "")


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
        print("Pneumatic Levitator on port: " + codePort, end="\n\n")
        ser = serial.Serial(
            port=codePort,
            baudrate=BAUDRATE,
            parity=PARITY,
            stopbits=STOP_BITS,
            bytesize=BYTE_SIZE
        )
        portOpened = True
        writeThread = Thread(target=writeSerial)
        readThread = Thread(target=readSerial)
        writeThread.start()
        readThread.start()
        writeThread.join()
        readThread.join()
    else:
        print("Pneumatic levitator no connected")
