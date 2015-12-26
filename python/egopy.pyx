

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
        TMatWrap CalculateKernel(TMatWrap) except +

cdef extern from "ego_wrapper.h" namespace "TCovWrap":
        void ListEntities()


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

    @staticmethod
    def listEnitites():
        ListEntities()

    def calculateKernel(self, m):
        if len(m.shape) == 1:
            m = np.asmatrix(m).T
        return fromMatWrapToNumpy(self.obj.CalculateKernel(fromNumpyToMatWrap(m)))