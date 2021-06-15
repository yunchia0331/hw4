import time
import serial

serdev = '/dev/ttyUSB0'                # use the device name you get from `ls /dev/ttyACM*`
s = serial.Serial(serdev, 9600)
print("RPC")
s.write("/Control/run 5 3 W \n".encode())
time.sleep(1)