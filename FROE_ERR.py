import scipy.io as sio
from scipy.signal import lfilter
import numpy as np
from matplotlib import pyplot as plt
import itertools as it
from operator import add
from functools import reduce
from sklearn.preprocessing import PolynomialFeatures as pf
from statsmodels.tsa.stattools import acf , ccf
import argparse
import sys
import csv

class _Froe(object):
    def __init__(self, args): #init all parameters from terminal or default values
        self.nE = args.ne if args.ne != None else 3
        self.nU = args.nu if args.nu != None else 0
        self.nY= args.ny if args.ny != None else 10
        self.polDegree = args.deg if args.deg != None else 3
        self.inclBias = args.incbias if args.incbias != None else True
        self.varianceToExplain1 = args.var1 if args.var1 != None else 0.99999
        self.varianceToExplain2 = args.var2 if args.var2 != None else 0.9999
        self.convergenceThresholdNARMAX = args.convth if args.convth != None else 0.2
        self.maxIterationsNARMAX = args.maxit if args.maxit != None else 10

    def running_mean(self, x, N):
        cumsum = np.cumsum(np.insert(x, 0, 0))
        return (cumsum[N:] - cumsum[:-N]) / float(N)

    def writeModel(self, Theta, regDim, nU, nY, nE, polDegree, inclBias): #write model as a string easier to print
        tempModelBasic = list()
        for i in range(nU + nY + nE):
            if (i < nU):
                tempModelBasic.append("u(t - {})".format(i + 1))
            elif (i < nU + nY):
                tempModelBasic.append("y(t - {})".format(i - nU + 1))
            else:
                tempModelBasic.append("e(t - {})".format(i - (nU + nY) + 1))

        tempModel = list()
        currentPos = nU + nY + nE
        startPos = 0
        if(inclBias):
            tempModel.append("1")
            currentPos +=1
            startPos = 1

        tempModel[startPos:currentPos] = tempModelBasic

        for i in range(2, polDegree + 1): #creates non linear terms ex. u(t-1)^2y(t-2)
            temp = list(it.combinations_with_replacement(tempModelBasic, i))
            temp = np.array([reduce(add, x) for x in temp])
            nextPos = currentPos + len(temp)
            tempModel[currentPos:nextPos] = temp
            currentPos = nextPos

        for i in range(regDim): #adds theta
            tempModel[i] = "{}*{}".format(Theta[i],tempModel[i])
        remove_idx = list()
        for i in range(regDim):
            if(Theta[i] == 0.0):
                remove_idx.append(i)
        model = [i for j, i in enumerate(tempModel) if j not in remove_idx]

        return model

    def createPhi(self, inpt, outpt, resids, polDeg, incB, inMean, outMean, yDim , nU, nY, nE): #creates phi matrix

        PhiTempBasic= np.zeros(shape = (yDim, (nU + nY + nE)), dtype = np.float64) #basic phi matrix degree 1
        if (nE != 0):
            resMean = np.mean(resids)

        for i in range(yDim):
            for u in range(nU):
                if (i - u - 1) < 0:
                    PhiTempBasic[i,u] = inMean
                else:
                    PhiTempBasic[i,u] = inpt[i - u - 1]
            for y in range(nY):
                if (i - y - 1) < 0:
                    PhiTempBasic[i,y + nU] = outMean
                else:
                    PhiTempBasic[i,y + nU] = outpt[i - y - 1]
            for e in range(nE):
                if (i - e - 1) < 0:
                    PhiTempBasic[i,e + nU + nY] = resMean
                else:
                    PhiTempBasic[i,e + nU + nY] = resids[i - e - 1]

        if(not np.isfinite(PhiTempBasic).all()):
            return None

        PhiTemp = pf(polDeg , include_bias = incB).fit_transform(PhiTempBasic) #expand to the right polynomial degree
        return PhiTemp.copy()

    def convergence(self, actual, previous, yDim, K, th, maxIt): #check convergence for narmax stopping criterion

        diff = sum(np.abs(self.calcResiduals(actual, previous, yDim))) / yDim

        return diff <= th or K >= maxIt

    def froe(self, nU, nY, nE, U, Y, E, polDegree, yDim, inclBias, varianceToExplain1, varianceToExplain2, K, th, maxIt): #froe algorithm
        if(K == 0): #narx or first iteration of narmax -> no residuals
            Phi = self.createPhi(U, Y, E, polDegree, inclBias, np.mean(U), np.mean(Y), yDim, nU, nY, 0)
        else:
            Phi = self.createPhi(U, Y, E, polDegree, inclBias, np.mean(U), np.mean(Y), yDim, nU, nY, nE)
        regDim = Phi.shape[1]
        Theta = np.zeros(shape = (regDim,), dtype = np.float64)
        A = np.zeros(shape = (regDim,regDim), dtype = np.float64)
        np.fill_diagonal(A, 1)
        sum_y_pow_2 = Y.T.dot(Y)
        regressorToKeep = np.array([], dtype = int)
        varianceExplained = 0
        W = np.zeros(shape = (yDim, regDim), dtype = np.float64)
        G = np.array([], dtype=np.float64)
        for j in range(regDim): #calculates err, g, updates a
            w = np.zeros(shape=(yDim,regDim), dtype = np.float64)
            err = np.zeros(shape=(regDim), dtype = np.float64)
            g = np.zeros(shape=(regDim), dtype = np.float64)

            for i in range(regDim):
                if i not in regressorToKeep:
                    wi = Phi[:,i].copy()
                    for k in range(j):
                        den = (W[:,k].T.dot(W[:,k]))
                        akj = 0
                        if (den != 0.0):
                            akj = W[:,k].T.dot(Phi[:,i]) / den
                        wi -= akj * W[:,k]
                        A[k,j] = akj
                    w[:,i] = wi
                    den_1 = (wi.T.dot(wi))
                    g[i] =0
                    if(den_1 != 0):
                        g[i] = wi.T.dot(Y) / den_1
                    err[i] = np.power(g[i], 2) * den_1 / sum_y_pow_2
            bestRegressor=np.argmax(err)
            regressorToKeep = np.append(regressorToKeep, bestRegressor)
            varianceExplained += err[bestRegressor]
            for k in range(j): #updates A based on selected regressor
                den = (W[:,k].T.dot(W[:,k]))
                akj = 0
                if (den != 0.0):
                    akj = W[:,k].T.dot(Phi[:,bestRegressor]) / den
                A[k,j] = akj
            W[:,j] = w[:,bestRegressor]
            G = np.append(G,g[bestRegressor])
            if (((1 - varianceExplained) < (1 - varianceToExplain1)) or (j == regDim - 1)): #stopping criterion narx and narmax
                if (nE != 0):
                    print("Iteration",K)
                print("taken" , len(regressorToKeep) , "/" , regDim , "parameters explaining" , varianceExplained, "variance\n")
                break

        ThetaTemp = np.zeros(len(G))
        for i in reversed(range(len(G))): #calculates theta
            if i == len(G):
                ThetaTemp[i] = G[i]
            else:
                temp = 0
                for k in range(i+1, len(G)):
                    temp += A[i,k] * ThetaTemp[k]
                ThetaTemp[i] = G[i] - temp
        Theta[regressorToKeep] = ThetaTemp

        model = self.writeModel(Theta,regDim, nU, nY, nE, polDegree, inclBias)

        if (nE != 0): #next iterations when narmax
            if(K == 0):
                Yhat = W[:,0:len(G)].dot(G)

            else:
                Yhat = self.generateYhat(0, U, Y, np.zeros(shape = (yDim), dtype = np.float64), Theta, polDegree, inclBias, yDim, nU, nY, nE)
            if (Yhat is None):
                print("Impossible to identify the residuals for the NARMAX\n")
                sys.exit()
            actualResiduals = self.calcResiduals(Y, Yhat, yDim)
            if (K == 0):
                return self.froe(nU, nY, nE, U, Y, actualResiduals, polDegree, yDim, inclBias, varianceToExplain2, varianceToExplain2, K + 1, th, maxIt)
            else:
                if(self.convergence(actualResiduals, E, yDim, K, th, maxIt)):
                    return Theta , model
                else:
                    return self.froe(nU, nY, nE, U, Y, actualResiduals, polDegree, yDim, inclBias, varianceToExplain2, varianceToExplain2, K + 1, th, maxIt)

        return Theta , model

    def generateYhat(self, type, u, y, e, theta, polDegree, inclBias, yDim, nU, nY, nE): #generates Yhat
        if (type == 0 and nE == 0): #identification or narx
            Phi = self.createPhi(u, y, e, polDegree, inclBias, np.mean(u), 0.0, yDim, nU, nY, nE)
            Yhat= Phi.dot(theta)
        else:
            if(nE != 0 and type != 0): #simulation narmax -> residuals are random from normal distribution
                resMean = np.mean(e)
                resStdDev = np.std(e)
            Yhat = np.zeros(shape = (yDim), dtype = np.float64)

            Phi = self.createPhi(u, np.zeros(shape = (yDim), dtype = np.float64), np.zeros(shape = (yDim), dtype = np.float64), 1, False, np.mean(u), 0.0, yDim, nU, nY, nE) #basic Phi with only U
            for i in range(yDim):
                try:
                    Yhat[i] = pf(polDegree , include_bias = inclBias).fit_transform(Phi[i,:].reshape(1,-1)).dot(theta) #expand row[i] of Phi and calculate Yhat
                    if(nE != 0 and type != 0):
                        res = np.random.normal(resMean, resStdDev)
                    for j in range(nY): #fill next rows of Phi with Y and E
                        if((i + j + 1) > (yDim - 1)):
                            break
                        if (type != 0):
                            Phi[i + j + 1,nU + j] = Yhat[i]
                        else:
                            Phi[i + j + 1,nU + j] = y[i]
                    for k in range(nE):
                        if((i + k + 1) > (yDim - 1)):
                            break
                        if (type == 0):
                            Phi[i + k + 1, nU + nY + k] = y[i] - Yhat[i]
                        else:
                            Phi[i + k + 1, nU + nY + k] = res
                except ValueError:
                    if (type == 0):
                        print("Model Divergent in prediction\n")
                        return None
                    else:
                        print("Model Divergent in simulation\n")
                        return None
        return Yhat

    def calcResiduals(self, out, outhat, yDim): #calcs residuals
        residualsTemp = np.zeros(shape=(yDim,), dtype = np.float64)
        for i in range(yDim):
            residualsTemp[i] = out[i]-outhat[i]
        return residualsTemp


    def calcMSE(self, res, yDim): #calcs MSE
        MSE=0
        if (not np.isfinite(res).all()):
            return np.inf
        for i in range(yDim):
            MSE += np.power(res[i],2)
        MSE /= yDim
        return MSE

    def plotCorrelation(self, value, xlabel, ylabel, tit, confidence, yDim, nr): #Plot correlation graphs
        plt.subplots_adjust(hspace = 0.4)
        plt.subplot(nr)
        plt.plot(value, color = "blue")
        plt.plot(confidence, color = "red")
        plt.plot(-confidence, color = "green")
        plt.title(tit)
        plt.xlabel(xlabel)
        plt.ylabel(ylabel)
        plt.gca().set_ylim([-1,1])
        plt.gca().set_xlim([0,100]) #first 100 lags

    def plotValidationTest(self, confidence, residuals, Uval, yDim, testType, num): #plot all validation tests

        uMeanSquared = np.power(np.mean(Uval), 2)

        Phi_e_e = acf(residuals , nlags = yDim - 1, unbiased = False)
        Phi_u_e = ccf(Uval, residuals, unbiased = False)
        Phi_e_eu = ccf(residuals ,np.multiply(residuals, Uval), unbiased = False)
        Phi_u2_e = ccf(np.subtract(np.power(Uval, 2), uMeanSquared), residuals, unbiased = False)
        Phi_u2_e2 = ccf(np.subtract(np.power(Uval, 2), uMeanSquared),np.power(residuals, 2), unbiased = False)

        plt.suptitle(testType + " validation tests " + num)

        self.plotCorrelation(Phi_e_e, r'$\tau$', r'$\phi_{ee}$', r'$\phi_{ee}(\tau)$', confidence, yDim, 321)
        self.plotCorrelation(Phi_u_e, r'$\tau$', r'$\phi_{ue}$', r'$\phi_{ue}(\tau)$', confidence, yDim, 322)
        self.plotCorrelation(Phi_e_eu, r'$\tau$', r'$\phi_{e(eu)}$', r'$\phi_{e(eu)}(\tau)$', confidence, yDim, 323)
        self.plotCorrelation(Phi_u2_e, r'$\tau$', r'$\phi_{{u}^2e}$', r'$\phi_{{u}^2e}(\tau)$', confidence, yDim, 324)
        self.plotCorrelation(Phi_u2_e2, r'$\tau$', r'$\phi_{{u}^2{e}^2}$',  r'$\phi_{{u}^2{e}^2}(\tau)$',confidence,yDim, 325)

        plt.show()

    def run(self): #main algorithm
        z1 =[]
        z2= []
        z3= []
        u1 = []
        u2=[]
        u3=[]
        with open('Logs/buono1.csv','r') as csvfile:
            next(csvfile, None);
            plots = csv.reader(csvfile, delimiter=',')
            i = 0
            for row in plots:
                i += 1
                z1.append(int(row[1]))
            csvfile.close()

        with open('Logs/buono2.csv','r') as csvfile:
            next(csvfile, None);
            plots = csv.reader(csvfile, delimiter=',')
            i = 0
            for row in plots:
                i += 1
                z2.append(int(row[1]))
            csvfile.close()

        with open('Logs/cattivo2.csv','r') as csvfile:
            next(csvfile, None);
            plots = csv.reader(csvfile, delimiter=',')
            i = 0
            for row in plots:
                i += 1
                z3.append(int(row[1]))
            csvfile.close()


        b = [0.000921992213781247, 0.00335702792571534, 0.00666846411080491, 0.00755286336642893, 0.00103278035537582, -0.0154786656220236, -0.0369577564468621, -0.0489939394790694, -0.0332613413754055, 0.0208964377504099, 0.105771877241975, 0.194080920241419, 0.250550621094879, 0.250550621094879, 0.194080920241419, 0.105771877241975, 0.0208964377504099, -0.0332613413754055, -0.0489939394790694, -0.0369577564468621, -0.0154786656220236, 0.00103278035537582, 0.00755286336642893, 0.00666846411080491, 0.00335702792571534, 0.000921992213781247]

        filter_size=6

        z1 = z1[75:-75]
        z1 = lfilter(b, [1.0], z1)
        #z1 = self.running_mean(z1,filter_size)
        z2 = z2[75:-75]
        z2 = lfilter(b, [1.0], z2)
        #z2 = self.running_mean(z2,filter_size)
        z3 = z3[75:-75]
        z3 = lfilter(b, [1.0], z3)
        #z3 = self.running_mean(z3,filter_size)
        #Training Set
        U, Y = np.array(u1) , np.array(z1)

        #Validation Set
        Uval1, Yval1 = np.array(u2) , np.array(z2)
        Uval2, Yval2 = np.array(u3) , np.array(z3)

        yDimIde = len(Y)
        uDimIde = len(U)

        yDimVal1 = len(Yval1)
        uDimVal1 = len(Uval1)

        yDimVal2 = len(Yval2)
        uDimVal2 = len(Uval2)

        #parameters
        polDegree = self.polDegree
        varianceToExplain1 = self.varianceToExplain1
        varianceToExplain2 = self.varianceToExplain2
        nU = self.nU
        nY = self.nY
        nE = self.nE
        inclBias = self.inclBias
        convergenceThresholdNARMAX = self.convergenceThresholdNARMAX
        maxIterationsNARMAX = self.maxIterationsNARMAX

        YhatPredictionIde = None #default initialization
        YhatSimulationIde = None
        YhatPredictionVal1 = None
        YhatSimulationVal1 = None
        YhatPredictionVal2 = None
        YhatSimulationVal2 = None

        print("\nStarting model with the following time delays: U", nU, "Y", nY, "E", nE, "\n")

        ThetaSelected , Model = self.froe(nU, nY, nE, U, Y, np.zeros(shape = (yDimIde), dtype = np.float64), polDegree, yDimIde, inclBias, varianceToExplain1, varianceToExplain2, 0, convergenceThresholdNARMAX, maxIterationsNARMAX)

        print("Model Obtained:\n")
        print(Model,"\n")

        print("Results on the identification set\n")

        #Prediction results on identification set
        YhatPredictionIde = self.generateYhat(0, U, Y, np.zeros(shape = (yDimIde), dtype = np.float64), ThetaSelected, polDegree, inclBias, yDimIde, nU, nY, nE)
        if (not YhatPredictionIde is None):
            residualsPredictionIde = self.calcResiduals(Y, YhatPredictionIde, yDimIde)
            msePredictionIde = self.calcMSE(residualsPredictionIde, yDimIde)
            '''
            #Simulation results on identification set
            YhatSimulationIde = self.generateYhat(1, U, Y, residualsPredictionIde, ThetaSelected, polDegree, inclBias, yDimIde, nU, nY, nE)
            if (not YhatSimulationIde is None):
                residualsSimulationIde = self.calcResiduals(Y, YhatSimulationIde, yDimIde)
                mseSimulationIde = self.calcMSE(residualsSimulationIde, yDimIde)
            '''
        if (not YhatPredictionIde is None):
            print("MSPE" , msePredictionIde, "\n")
        if (not YhatSimulationIde is None):
            print("MSSE" , mseSimulationIde, "\n")

        print("Results on the validation set 1\n")

        #Prediction results on validation set 1
        YhatPredictionVal1 = self.generateYhat(0, Uval1, Yval1, np.zeros(shape = (yDimVal1), dtype = np.float64), ThetaSelected, polDegree, inclBias, yDimVal1, nU, nY, nE)
        if (not YhatPredictionVal1 is None):
            residualsPredictionVal1 = self.calcResiduals(Yval1, YhatPredictionVal1, yDimVal1)
            msePredictionVal1 = self.calcMSE(residualsPredictionVal1, yDimVal1)
            '''
            #Simulation results on validation set 1
            YhatSimulationVal1 = self.generateYhat(1, Uval1, Yval1, residualsPredictionVal1, ThetaSelected, polDegree, inclBias, yDimVal1, nU, nY, nE)
            if (not YhatSimulationVal1 is None):
                residualsSimulationVal1 = self.calcResiduals(Yval1, YhatSimulationVal1, yDimVal1)
                mseSimulationVal1 = self.calcMSE(residualsSimulationVal1, yDimVal1)
            '''
        if (not YhatPredictionVal1 is None):
            print("MSPE" , msePredictionVal1, "\n")
        if (not YhatSimulationVal1 is None):
            print("MSSE" , mseSimulationVal1, "\n")

        print("Results on the validation set 2\n")

        #Prediction results on validation set 2
        YhatPredictionVal2 = self.generateYhat(0, Uval2, Yval2, np.zeros(shape = (yDimVal2), dtype = np.float64), ThetaSelected, polDegree, inclBias, yDimVal2, nU, nY, nE)
        if (not YhatPredictionVal2 is None):
            residualsPredictionVal2 = self.calcResiduals(Yval2, YhatPredictionVal2, yDimVal2)
            msePredictionVal2 = self.calcMSE(residualsPredictionVal2, yDimVal2)
            '''
            #Simulation results on validation set 2
            YhatSimulationVal2 = self.generateYhat(1, Uval2, Yval2, residualsPredictionVal2, ThetaSelected, polDegree, inclBias, yDimVal2, nU, nY, nE)
            if (not YhatSimulationVal2 is None):
                residualsSimulationVal2 = self.calcResiduals(Yval2, YhatSimulationVal2, yDimVal2)
                mseSimulationVal2 = self.calcMSE(residualsSimulationVal2, yDimVal2)
            '''
        if (not YhatPredictionVal2 is None):
            print("MSPE" , msePredictionVal2, "\n")
        if (not YhatSimulationVal2 is None):
            print("MSSE" , mseSimulationVal2, "\n")


        plt.subplot("311")
        plt.plot(Y[30:], color = "red")
        if (not YhatPredictionIde is None):
            plt.plot(YhatPredictionIde[30:], color = "blue")
        if (not YhatSimulationIde is None):
            plt.plot(YhatSimulationIde[30:], color = "green")
        plt.title('Y (red) / Y predicted (blue)') # / Y simulated (green) - Identification Set
        plt.xlabel('time')
        plt.ylabel('output')

        plt.subplots_adjust(hspace = 0.5)
        plt.subplot('312')
        plt.plot(Yval1[30:], color = "red")
        if (not YhatPredictionVal1 is None):
            plt.plot(YhatPredictionVal1[30:], color = "blue")
        if (not YhatSimulationVal1 is None):
            plt.plot(YhatSimulationVal1[30:], color = "green")
        plt.title('Y (red) / Y predicted (blue)') # / Y simulated (green) - Validation Set 1
        plt.xlabel('time')
        plt.ylabel('output')

        plt.subplots_adjust(hspace = 0.5)
        plt.subplot('313')
        plt.plot(Yval2[30:], color = "red")
        if (not YhatPredictionVal2 is None):
            plt.plot(YhatPredictionVal2[30:], color = "blue")
        if (not YhatSimulationVal2 is None):
            plt.plot(YhatSimulationVal2[30:], color = "green")
        plt.title('Y (red) / Y predicted (blue)') # / Y simulated (green) - Validation Set 2
        plt.xlabel('time')
        plt.ylabel('output')
        plt.show()

        #plt.show()
        '''
        #95% confidence
        confidence1 = np.full(shape = (yDimVal1), fill_value = (1.96 / np.sqrt(yDimVal1)) , dtype = np.float64)
        confidence2 = np.full(shape = (yDimVal2), fill_value = (1.96 / np.sqrt(yDimVal2)) , dtype = np.float64)
        if (not YhatPredictionVal1 is None):
            self.plotValidationTest(confidence1, residualsPredictionVal1 ,Uval1, yDimVal1, "Prediction", "1")
        if (not YhatSimulationVal1 is None):
            self.plotValidationTest(confidence1, residualsSimulationVal1 ,Uval1, yDimVal1, "Simulation", "1")
        if (not YhatPredictionVal2 is None):
            self.plotValidationTest(confidence2, residualsPredictionVal2 ,Uval2, yDimVal2, "Prediction", "2")
        if (not YhatSimulationVal2 is None):
            self.plotValidationTest(confidence2, residualsSimulationVal2 ,Uval2, yDimVal2, "Simulation", "2")
        '''

if __name__ == "__main__": #all optional parameters from terminal
    parser = argparse.ArgumentParser(description='Performs identification and validation on narx/narmax models.')
    parser.add_argument('--ny', type=int, dest='ny', help='The maximum lag for y (integer)')
    parser.add_argument('--nu', type=int, dest='nu',help='The maximum lag for u (integer)')
    parser.add_argument('--ne', type=int, dest='ne',help='The maximum lag for e (integer)')
    parser.add_argument('--deg', type=int, dest='deg',help='The polynomial degree of the model (integer)')
    parser.add_argument('--incbias', type=bool, dest='incbias', help='Add or not Bias to the polynomial (boolean)')
    parser.add_argument('--var1', type=float, dest='var1',help='The variance to be explained  for NARX or fist NARMAX iteration (float)')
    parser.add_argument('--var2', type=float, dest='var2',help='The variance to be explained for following NARMAX iterations (float)')
    parser.add_argument('--convth', type=float, dest='convth',help='Convergence threshold in the NARMAX case (float)')
    parser.add_argument('--maxit', type=int, dest='maxit',help='Iterations limit in the NARMAX case (integer)')
    args = parser.parse_args()
    froe = _Froe(args) #init class
    froe.run() #start algorithm
