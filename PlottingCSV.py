'''
Created by Federico Milani, Davide Urzino, Dalla Longa Emanuele
'''

import matplotlib.pyplot as plt
from scipy.signal import lfilter
import csv
import sys
import os
import numpy as np
from Froe import _Froe as FroeAlgorithm
from joblib import Parallel, delayed
import textwrap as tw

#subfolder for models and logs
folder = '/VentolaPiccola'
#log of fan in good state
goodFile = 'good fan 1.csv'

#lowpass fir filtering data
def filterData(data, postfilter_cut):
    b = [0.000921992213781247, 0.00335702792571534, 0.00666846411080491, 0.00755286336642893, 0.00103278035537582, -0.0154786656220236, -0.0369577564468621, -0.0489939394790694, -0.0332613413754055, 0.0208964377504099, 0.105771877241975, 0.194080920241419, 0.250550621094879, 0.250550621094879, 0.194080920241419, 0.105771877241975, 0.0208964377504099, -0.0332613413754055, -0.0489939394790694, -0.0369577564468621, -0.0154786656220236, 0.00103278035537582, 0.00755286336642893, 0.00666846411080491, 0.00335702792571534, 0.000921992213781247]
    temp = lfilter(b, [1.0], data)
    temp = temp[postfilter_cut:]
    return temp

#extracts data from logs and returns an array.
#Preprocessing_cut removes n elements at the start and at the end of the log
def extractDataFromFile(file_path, preprocessing_cut):
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

    with open('Logs' + folder + '/'+file_path,'r') as csvfile:
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

#runs FROE and saves one model for each sensor
def calculateTheta(i, data):
    varianceToExplain = [0.99973, 0.99966, 0.999993, 0.9999, 0.9998, 0.999995, 0.9995, 0.9999785, 0.99978]
    froe = FroeAlgorithm()
    theta = froe.calculateModel(data, varianceToExplain[i])
    np.save('Models' + folder + '/model_' + str(i), theta)

#takes the log of the motor in a good state and calculate all the models
def calculateModels():
    goodData = extractDataFromFile(goodFile)
    Parallel(n_jobs = 5)(delayed(calculateTheta)(i, goodData[i]) for i in range(len(goodData) - 1))

#plots each sensor, compares each log with the model and shows the output
def plotCSV():
    froe = FroeAlgorithm()

    thetas = []
    for i in range(9):
        thetas.append(np.load('Models' + folder + '/model_' + str(i) + '.npy'))

    goodData = extractDataFromFile(goodFile, 0)

    logs = [name for name in os.listdir('Logs' + folder) if os.path.splitext(name)[1] == '.csv']

    dataNames = ["AccX","AccY","AccZ","GyroX","GyroY","GyroZ","MagX","MagY","MagZ"]

    for file_name in logs:
        name = os.path.splitext(file_name)[0]

        data = extractDataFromFile(file_name, 0)
        time = data[9]
        th = 10;
        print(name)
        finalAnnotation = ''
        backgroundcolor = 'green'
        AccDefCount = 0;
        GyroDefCount = 0;
        MagDefCount = 0;
        for i in range(len(data) - 1):
            ratio = froe.run(goodData[i], data[i], thetas[i])
            if(ratio > th):
                finalAnnotation += dataNames[i] + ' - MSPE ratio ' + '{:.2f}'.format(ratio) + " X \n"
                if(i>=0 & i<3):
                    AccDefCount+=1
                elif(i>=3 & i<6):
                    GyroDefCount+=1
                else:
                    MagDefCount+=1


                print('sensorData', dataNames[i], ' - MSPE ratio', ratio, " X ")
            else:
                finalAnnotation += dataNames[i] + ' - MSPE ratio ' + '{:.2f}'.format(ratio) + " ✓ \n"
                print('sensorData', dataNames[i], ' - MSPE ratio',ratio, " ✓ ")

        print('\n\n')
        if(AccDefCount + GyroDefCount + MagDefCount < 2):
            backgroundcolor = "green"

        elif(AccDefCount + GyroDefCount + MagDefCount <4):
            backgroundcolor = "orange"

        else:
            backgroundcolor = "red"




        plt.figure(num=name)

        plt.subplot(221)
        plt.plot(time,data[0], label='AccX')
        plt.plot(time,data[1], label='AccY')
        plt.plot(time,data[2], label='AccZ')
        plt.xlabel('Time')
        plt.ylabel('Accelerometer')
        plt.title('Accelerometer data')
        plt.suptitle(name, fontsize=16)
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
            xy=(0.5, 0.2), xytext=(0.5, 0.5),
            xycoords=('axes fraction', 'figure fraction'),
            textcoords='offset points',
            size=12, ha='center', va='bottom',backgroundcolor=backgroundcolor)

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
    models = [name for name in os.listdir('Models' + folder) if os.path.splitext(name)[1] == '.npy']
    #if there are not enough models trained, then train them
    if len(models) < 9:
        calculateModels()
        print('MODELS TRAINED! RUN AGAIN THE SCRIPT TO SEE THE RESULTS!')
    else:
        plotCSV()
