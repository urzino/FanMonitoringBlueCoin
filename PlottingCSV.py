import matplotlib.pyplot as plt
import csv
import sys

'''

'''

def plotCSV(file_path):
    time = []
    accX = []
    accY = []
    accZ = []
    GyroX = []
    GyroY = []
    GyroZ = []
    MagX = []
    MagY = []
    MagZ = []

    with open(file_path,'r') as csvfile:
        next(csvfile, None);
        plots = csv.reader(csvfile, delimiter=',')
        i = 0
        for row in plots:
            time.append(i)
            i += 1
            accX.append(int(row[1]))
            accY.append(int(row[2]))
            accZ.append(int(row[3]))
            GyroX.append(int(row[4]))
            GyroY.append(int(row[5]))
            GyroZ.append(int(row[6]))
            MagX.append(int(row[7]))
            MagY.append(int(row[8]))
            MagZ.append(int(row[9]))

    plt.subplot(221)
    plt.plot(time,accX, label='AccX')
    plt.plot(time,accY, label='AccY')
    plt.plot(time,accZ, label='AccZ')
    plt.xlabel('Time')
    plt.ylabel('Accelerometer')
    plt.title('Accelerometer data')
    plt.legend()

    plt.subplot(222)
    plt.plot(time,GyroX, label='GyroX')
    plt.plot(time,GyroY, label='GyroY')
    plt.plot(time,GyroZ, label='GyroZ')
    plt.xlabel('Time')
    plt.ylabel('Gyroscope')
    plt.title('Gyroscope data')
    plt.legend()

    plt.subplot(223)
    plt.plot(time,MagX, label='MagX')
    plt.plot(time,MagY, label='MagY')
    plt.plot(time,MagZ, label='MagZ')
    plt.xlabel('Time')
    plt.ylabel('Magnetometer')
    plt.title('Magnetometer data')
    plt.legend()
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) > 1 && sys.argv[1] != '':
        file_path = sys.argv[1]
        plotCSV(file_path)
