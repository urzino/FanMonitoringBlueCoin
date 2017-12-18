import matplotlib.pyplot as plt
from scipy.signal import lfilter
import csv
import sys
import os
import numpy as np
from Froe import _Froe as FroeAlgorithm
from joblib import Parallel, delayed
import textwrap as tw

def running_mean(x, N):
    cumsum = np.cumsum(np.insert(x, 0, 0))
    return (cumsum[N:] - cumsum[:-N]) / float(N)

def filterData(data, postfilter_cut):
    b = [0.000921992213781247, 0.00335702792571534, 0.00666846411080491, 0.00755286336642893, 0.00103278035537582, -0.0154786656220236, -0.0369577564468621, -0.0489939394790694, -0.0332613413754055, 0.0208964377504099, 0.105771877241975, 0.194080920241419, 0.250550621094879, 0.250550621094879, 0.194080920241419, 0.105771877241975, 0.0208964377504099, -0.0332613413754055, -0.0489939394790694, -0.0369577564468621, -0.0154786656220236, 0.00103278035537582, 0.00755286336642893, 0.00666846411080491, 0.00335702792571534, 0.000921992213781247]
    temp = lfilter(b, [1.0], data)
    temp = temp[postfilter_cut:]
    return temp

def extractDataFromFile(file_path):
    time = []
    accX = []
    accY = []
    accZ = []
    gyroX = []
    gyroY = []
    gyroZ = []
    magX = []
    magY = []
    magZ = []

    with open('Logs/VentolaGrande/'+file_path,'r') as csvfile:
        next(csvfile, None);
        plots = csv.reader(csvfile, delimiter=',')
        i = 0
        for row in plots:
            time.append(i)
            i += 1
            accX.append(int(row[1]))
            accY.append(int(row[2]))
            accZ.append(int(row[3]))
            gyroX.append(int(row[4]))
            gyroY.append(int(row[5]))
            gyroZ.append(int(row[6]))
            magX.append(int(row[7]))
            magY.append(int(row[8]))
            magZ.append(int(row[9]))
        csvfile.close()

    preprocessing_cut = 0
    postfilter_cut = 50

    if preprocessing_cut > 0:
        accX = accX[preprocessing_cut:-preprocessing_cut]
        accY = accY[preprocessing_cut:-preprocessing_cut]
        accZ = accZ[preprocessing_cut:-preprocessing_cut]
        gyroX = gyroX[preprocessing_cut:-preprocessing_cut]
        gyroY = gyroY[preprocessing_cut:-preprocessing_cut]
        gyroZ = gyroZ[preprocessing_cut:-preprocessing_cut]
        magX = magX[preprocessing_cut:-preprocessing_cut]
        magY = magY[preprocessing_cut:-preprocessing_cut]
        magZ = magZ[preprocessing_cut:-preprocessing_cut]
        time = time[preprocessing_cut + postfilter_cut:-preprocessing_cut]
    else:
        time = time[postfilter_cut:]

    accX = filterData(accX, postfilter_cut)

    accY = filterData(accY, postfilter_cut)

    accZ = filterData(accZ, postfilter_cut)

    gyroX = filterData(gyroX, postfilter_cut)

    gyroY = filterData(gyroY, postfilter_cut)

    gyroZ = filterData(gyroZ, postfilter_cut)

    magX = filterData(magX, postfilter_cut)

    magY = filterData(magY, postfilter_cut)

    magZ = filterData(magZ, postfilter_cut)


    time = [x - preprocessing_cut - postfilter_cut for x in time]

    return [accX, accY, accZ, gyroX, gyroY, gyroZ, magX, magY, magZ, time]

def calculateTheta(i, data):
    varianceToExplain = [0.9997, 0.9997, 0.9997, 0.9997, 0.9997, 0.9997, 0.9997, 0.9997, 0.9997]
    froe = FroeAlgorithm()
    theta = froe.calculateModel(data, varianceToExplain[i])
    np.save('Models/model_' + str(i), theta)

def calculateModels():
    #accXBuono, accYBuono, accZBuono, gyroXBuono, gyroYBuono, gyroZBuono, magXBuono, magYBuono, magZBuono, timeBuono = extractDataFromFile('buono1.csv')
    dataBuono = extractDataFromFile('buono1.csv')
    Parallel(n_jobs = 5)(delayed(calculateTheta)(i, dataBuono[i]) for i in range(len(dataBuono) - 1))


def plotCSV():
    froe = FroeAlgorithm()

    thetas = []
    for i in range(9):
        thetas.append(np.load('Models/model_' + str(i) + '.npy'))

    dataBuono = extractDataFromFile('buono1.csv')

    logs = [name for name in os.listdir('Logs/VentolaGrande/') if os.path.splitext(name)[1] == '.csv']

    dataNames = ["AccX","AccY","AccZ","GyroX","GyroY","GyroZ","MagX","MagY","MagZ"]

    for file_name in logs:
        data = extractDataFromFile(file_name)
        time = data[9]
        th = 10;
        print(file_name)
        finalAnnotation = ''
        for i in range(len(data) - 1):
            ratio = froe.run(dataBuono[i], data[i], thetas[i])
            if(ratio > th):
                finalAnnotation += dataNames[i] + ' - ratio ' + str(ratio) + " X \n"
                print('sensorData', dataNames[i], ' - ratio',ratio, " X ")
            else:
                finalAnnotation += dataNames[i] + ' - ratio ' + str(ratio) + " ✓ \n"
                print('sensorData', dataNames[i], ' - ratio',ratio, " ✓ ")



        print('\n\n')
        plt.figure(num=file_name)

        plt.subplot(221)
        plt.plot(time,data[0], label='AccX')
        plt.plot(time,data[1], label='AccY')
        plt.plot(time,data[2], label='AccZ')
        plt.xlabel('Time')
        plt.ylabel('Accelerometer')
        plt.title('Accelerometer data')
        plt.suptitle(file_name, fontsize=16)
        plt.legend()

        plt.subplots_adjust(hspace = 0.5, wspace = 0.5)
        plt.subplot(222)
        plt.plot(time,data[3], label='GyroX')
        plt.plot(time,data[4], label='GyroY')
        plt.plot(time,data[5], label='GyroZ')
        plt.xlabel('Time')
        plt.ylabel('gyroscope')
        plt.title('gyroscope data')
        plt.legend()
        plt.annotate(finalAnnotation,
            xy=(0.5, 0), xytext=(0, 0),
            xycoords=('axes fraction', 'figure fraction'),
            textcoords='offset points',
            size=12, ha='center', va='bottom')

        plt.subplots_adjust(hspace = 0.5)
        plt.subplot(223)
        plt.plot(time,data[6], label='MagX')
        plt.plot(time,data[7], label='MagY')
        plt.plot(time,data[8], label='MagZ')
        plt.xlabel('Time')
        plt.ylabel('magnetometer')
        plt.title('magnetometer data')
        plt.legend()


        plt.show()

if __name__ == "__main__":
    models = [name for name in os.listdir('Models') if os.path.splitext(name)[1] == '.npy']
    if len(models) < 9:
        calculateModels()
    else:
        plotCSV()
