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
    def __init__(self, args = None): #init all parameters from terminal or default values
        self.nE = args.ne if args != None and args.ne != None else 0
        self.nU = args.nu if args != None and args.nu != None else 0
        self.nY= args.ny if args != None and args.ny != None else 10
        self.polDegree = args.deg if args != None and args.deg != None else 3
        self.inclBias = args.incbias if args != None and args.incbias != None else True
        self.varianceToExplain1 = args.var1 if args != None and args.var1 != None else 0.99973
        self.varianceToExplain2 = args.var2 if args != None and args.var2 != None else 0.9999
        self.convergenceThresholdNARMAX = args.convth if args != None and args.convth != None else 0.2
        self.maxIterationsNARMAX = args.maxit if args != None and args.maxit != None else 10

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

    def calculateModel(self, z1, varianceToExplain):
        u1 = []
        #parameters
        polDegree = self.polDegree
        varianceToExplain1 = varianceToExplain
        varianceToExplain2 = self.varianceToExplain2
        nU = self.nU
        nY = self.nY
        nE = self.nE
        inclBias = self.inclBias
        convergenceThresholdNARMAX = self.convergenceThresholdNARMAX
        maxIterationsNARMAX = self.maxIterationsNARMAX
        #Training Set
        U, Y = np.array(u1) , np.array(z1)

        yDimIde = len(Y)
        uDimIde = len(U)

        print("\nStarting model with the following time delays: U", nU, "Y", nY, "E", nE, "\n")

        ThetaSelected , Model = self.froe(nU, nY, nE, U, Y, np.zeros(shape = (yDimIde), dtype = np.float64), polDegree, yDimIde, inclBias, varianceToExplain1, varianceToExplain2, 0, convergenceThresholdNARMAX, maxIterationsNARMAX)

        print("Model Obtained:\n")
        print(Model,"\n")
        return ThetaSelected

    def run(self, z1, z2, ThetaSelected): #main algorithm
        u1 = []
        u2 = []

        #Training Set
        U, Y = np.array(u1) , np.array(z1)

        #Validation Set
        Uval1, Yval1 = np.array(u2) , np.array(z2)
        #Uval2, Yval2 = np.array(u3) , np.array(z3)

        yDimIde = len(Y)
        uDimIde = len(U)

        yDimVal1 = len(Yval1)
        uDimVal1 = len(Uval1)

        #parameters
        polDegree = self.polDegree
        nU = self.nU
        nY = self.nY
        nE = self.nE
        inclBias = self.inclBias

        #default initialization
        YhatPredictionIde = None
        YhatPredictionVal1 = None

        #Prediction results on identification set
        YhatPredictionIde = self.generateYhat(0, U, Y, np.zeros(shape = (yDimIde), dtype = np.float64), ThetaSelected, polDegree, inclBias, yDimIde, nU, nY, nE)
        if (not YhatPredictionIde is None):
            residualsPredictionIde = self.calcResiduals(Y, YhatPredictionIde, yDimIde)
            msePredictionIde = self.calcMSE(residualsPredictionIde, yDimIde)
        '''
        if (not YhatPredictionIde is None):
            print("MSPE" , msePredictionIde, "\n")
        '''

        #print("Results on the validation set 1\n")

        #Prediction results on validation set 1
        YhatPredictionVal1 = self.generateYhat(0, Uval1, Yval1, np.zeros(shape = (yDimVal1), dtype = np.float64), ThetaSelected, polDegree, inclBias, yDimVal1, nU, nY, nE)
        if (not YhatPredictionVal1 is None):
            residualsPredictionVal1 = self.calcResiduals(Yval1, YhatPredictionVal1, yDimVal1)
            msePredictionVal1 = self.calcMSE(residualsPredictionVal1, yDimVal1)
        '''
        if (not YhatPredictionVal1 is None):
            print("MSPE" , msePredictionVal1, "\n")
        '''

        return msePredictionVal1 / msePredictionIde

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
