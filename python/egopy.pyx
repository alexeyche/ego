

from libcpp.vector cimport vector
import numpy as np
cimport numpy as np

cdef extern from "ego_wrapper.h":
    cdef cppclass TMatWrap:
        TMatWrap(double *v, size_t n_rows, size_t n_cols)
        TMatWrap()
        size_t GetNRows()
        size_t GetNCols()
        double GetValue(size_t, size_t)

cdef extern from "ego_wrapper.h":
    cdef cppclass TCovWrap:
        TCovWrap(const char *covName, size_t dim_size, vector[double]) except +
        TMatWrap CalculateKernel(TMatWrap)

cdef extern from "ego_wrapper.h":
    cdef cppclass TMeanWrap:
        TMeanWrap(const char *meanName, size_t dim_size, vector[double]) except +

cdef extern from "ego_wrapper.h":
    cdef cppclass TLikWrap:
        TLikWrap(const char *likName, size_t dim_size, vector[double]) except +

cdef extern from "ego_wrapper.h":
    cdef cppclass TInfWrap:
        TInfWrap(const char *infName) except +

cdef extern from "ego_wrapper.h":
    cdef cppclass TModelWrap:
        TModelWrap(TMeanWrap* mean, TCovWrap* cov, TLikWrap* lik, TInfWrap* inf)
        SetData(TMatWrap, TMatWrap)

cdef extern from "ego_wrapper.h":
    void SetDebugLogLevel()

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

    def __init__(self, str covName, int dim_size, vector[double] params):
        self.obj = new TCovWrap(covName, dim_size, params)

    def __del__(self):
        del self.obj

    def calculateKernel(self, m):
        if len(m.shape) == 1:
            m = np.asmatrix(m).T
        return fromMatWrapToNumpy(self.obj.CalculateKernel(fromNumpyToMatWrap(m)))


cdef class Mean:
    cdef TMeanWrap* obj

    def __init__(self, str meanName, int dim_size, vector[double] params):
        self.obj = new TMeanWrap(meanName, dim_size, params)

    def __del__(self):
        del self.obj

cdef class Lik:
    cdef TLikWrap* obj

    def __init__(self, str likName, int dim_size, vector[double] params):
        self.obj = new TLikWrap(likName, dim_size, params)

    def __del__(self):
        del self.obj

cdef class Inf:
    cdef TInfWrap* obj

    def __init__(self, str infName): #, Mean mean, Cov cov, Lik lik
        self.obj = new TInfWrap(infName)  # , mean.obj, cov.obj, lik.obj

    def __del__(self):
        del self.obj


cdef class Model:
    cdef TModelWrap* obj

    def __init__(self, Mean mean, Cov cov, Lik lik, Inf inf):
        self.obj = new TModelWrap(mean.obj, cov.obj, lik.obj, inf.obj)

    def setData()

    def __del__(self):
        del self.obj


def setDebugLogLevel():
    SetDebugLogLevel()