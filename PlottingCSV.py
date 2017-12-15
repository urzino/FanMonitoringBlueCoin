import matplotlib.pyplot as plt
from scipy.signal import lfilter
import csv
import sys
import os
import numpy
'''

'''

def running_mean(x, N):
    cumsum = numpy.cumsum(numpy.insert(x, 0, 0))
    return (cumsum[N:] - cumsum[:-N]) / float(N)

def plotCSV():
    b = [0.000921992213781247, 0.00335702792571534, 0.00666846411080491, 0.00755286336642893, 0.00103278035537582, -0.0154786656220236, -0.0369577564468621, -0.0489939394790694, -0.0332613413754055, 0.0208964377504099, 0.105771877241975, 0.194080920241419, 0.250550621094879, 0.250550621094879, 0.194080920241419, 0.105771877241975, 0.0208964377504099, -0.0332613413754055, -0.0489939394790694, -0.0369577564468621, -0.0154786656220236, 0.00103278035537582, 0.00755286336642893, 0.00666846411080491, 0.00335702792571534, 0.000921992213781247]

    logs = [name for name in os.listdir('Logs') if os.path.splitext(name)[1] == '.csv']

    for file_path in logs:
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

        with open('Logs/'+file_path,'r') as csvfile:
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
            csvfile.close()

        filter_size=6
        preprocessing_cut = 800
        postfilter_cut = 50

        accX = accX[preprocessing_cut:-preprocessing_cut]
        accX = lfilter(b, [1.0], accX)
        accX = accX[postfilter_cut:]

        accY = accY[preprocessing_cut:-preprocessing_cut]
        accY = lfilter(b, [1.0], accY)
        accY = accY[postfilter_cut:]

        accZ = accZ[preprocessing_cut:-preprocessing_cut]
        accZ = lfilter(b, [1.0], accZ)
        accZ = accZ[postfilter_cut:]

        GyroX = GyroX[preprocessing_cut:-preprocessing_cut]
        GyroX = lfilter(b, [1.0], GyroX)
        GyroX = GyroX[postfilter_cut:]

        GyroY = GyroY[preprocessing_cut:-preprocessing_cut]
        GyroY = lfilter(b, [1.0], GyroY)
        GyroY = GyroY[postfilter_cut:]

        GyroZ = GyroZ[preprocessing_cut:-preprocessing_cut]
        GyroZ = lfilter(b, [1.0], GyroZ)
        GyroZ = GyroZ[postfilter_cut:]

        MagX = MagX[preprocessing_cut:-preprocessing_cut]
        MagX = lfilter(b, [1.0], MagX)
        MagX = MagX[postfilter_cut:]

        MagY = MagY[preprocessing_cut:-preprocessing_cut]
        MagY = lfilter(b, [1.0], MagY)
        MagY = MagY[postfilter_cut:]

        MagZ = MagZ[preprocessing_cut:-preprocessing_cut]
        MagZ = lfilter(b, [1.0], MagZ)
        MagZ = MagZ[postfilter_cut:]

        time = time[preprocessing_cut + postfilter_cut:-preprocessing_cut]
        time = [x - preprocessing_cut - postfilter_cut for x in time]
        #time = time[filter_size-1:]

        plt.figure(num=file_path)

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
    plotCSV()
