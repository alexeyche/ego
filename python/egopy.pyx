

from libcpp.vector cimport vector
import numpy as np
cimport numpy as np

from egopy_configs cimport modelConfigFromDict
from egopy_configs cimport optParamsFromDict

cdef TMatWrap fromNumpyToMatWrap(np.ndarray[np.double_t, ndim=2] mat):
    nrow = mat.shape[0]
    ncol = mat.shape[1]

    cdef np.ndarray[np.double_t, ndim=2, mode="c"] x = np.ascontiguousarray(mat, dtype=np.double)
    return TMatWrap(&x[0, 0], nrow, ncol)

cdef np.ndarray[np.double_t] fromMatWrapToNumpy(TMatWrap matWr):
    cdef np.ndarray res = np.zeros([matWr.GetNRows(), matWr.GetNCols()], dtype=np.double)
    for i in range(matWr.GetNRows()):
        for j in range(matWr.GetNCols()):
            res[i, j] = matWr.GetValue(i, j)
    return res

cdef class Cov:
    cdef TCovWrap* obj

    def __init__(self, str covName, int dim_size, vector[double] params = vector[double]()):
        self.obj = new TCovWrap(covName, dim_size, params)

    def __del__(self):
        del self.obj

    def calculateKernel(self, m):
        if len(m.shape) == 1:
            m = np.asmatrix(m).T
        return fromMatWrapToNumpy(self.obj.CalculateKernel(fromNumpyToMatWrap(m)))


cdef class Mean:
    cdef TMeanWrap* obj

    def __init__(self, str meanName, int dim_size, vector[double] params = vector[double]()):
        self.obj = new TMeanWrap(meanName, dim_size, params)

    def __del__(self):
        del self.obj

cdef class Lik:
    cdef TLikWrap* obj

    def __init__(self, str likName, int dim_size, vector[double] params = vector[double]()):
        self.obj = new TLikWrap(likName, dim_size, params)

    def __del__(self):
        del self.obj

cdef class Acq:
    cdef TAcqWrap* obj

    def __init__(self, str acqName, int dim_size, vector[double] params = vector[double]()):
        self.obj = new TAcqWrap(acqName, dim_size, params)

    def evaluateCriteria(self, x):
        if len(x.shape) == 1:
            x = np.asmatrix(x).T
        r = self.obj.EvaluateCriteria(fromNumpyToMatWrap(x))
        return fromMatWrapToNumpy(r.first), fromMatWrapToNumpy(r.second)

    def setParameters(self, x):
        self.obj.SetParameters(x)

    def __del__(self):
        del self.obj

cdef class Inf:
    cdef TInfWrap* obj

    def __init__(self, str infName): #, Mean mean, Cov cov, Lik lik
        self.obj = new TInfWrap(infName)  # , mean.obj, cov.obj, lik.obj

    def __del__(self):
        del self.obj


cdef class Distr:
    cdef TDistrWrap* obj

    def __init__(self):
        self.obj = new TDistrWrap()

    cdef setDistr(self, TDistrWrap d):
        del self.obj
        self.obj = new TDistrWrap(d) 

    def getMean(self):
        return self.obj.GetMean()

    def getSd(self):
        return self.obj.GetSd()

    def __del__(self):
        del self.obj


cdef double OptimCallback(const vector[double]& v, void* f):
    return (<object>f)(v)

cdef class Model:
    cdef TModelWrap* obj

    def __init__(self, Mean mean, Cov cov, Lik lik, Inf inf, Acq acq):
        self.obj = new TModelWrap(mean.obj, cov.obj, lik.obj, inf.obj, acq.obj)

    def setData(self, X, Y):
        if len(X.shape) == 1:
            X = np.asmatrix(X).T
        if len(Y.shape) == 1:
            Y = np.asmatrix(Y).T
        
        self.obj.SetData(fromNumpyToMatWrap(X), fromNumpyToMatWrap(Y))

    def setConfig(self, d):
        self.obj.SetConfig(modelConfigFromDict(d))

    def getPrediction(self, X):
        if len(X.shape) == 1:
            X = np.asmatrix(X).T
        cdef vector[TDistrWrap] res = self.obj.GetPrediction(fromNumpyToMatWrap(X))
        l = list()
        for o in res:
            d = Distr()
            d.setDistr(o)
            l.append(d)
        return l

    def getParameters(self):
        return self.obj.GetParameters()

    def optimize(self, f):
        self.obj.Optimize(OptimCallback, <void*>f)

    def __del__(self):
        del self.obj

    def getData(self):
        d = self.obj.GetData()
        return fromMatWrapToNumpy(d.first), fromMatWrapToNumpy(d.second)

    def optimizeHyp(self):
        self.obj.OptimizeHyp()
        
def setDebugLogLevel():
    SetDebugLogLevel()
