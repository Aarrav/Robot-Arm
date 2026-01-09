import serial
import matplotlib.pyplot as plt

ser = serial.Serial('/dev/cu.usbmodem178375201', 9600)
x, y = [], []

plt.ion()
print("Python is running")

while True:
    line = ser.readline().decode().strip()
    print(line)
    try:
        value = float(line)
        y.append(value)
        x.append(len(x))
        plt.clf()
        plt.plot(x, y)
        plt.pause(0.01)
    except:
        pass
