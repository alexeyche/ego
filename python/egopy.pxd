
from libcpp.vector cimport vector
from libcpp.pair cimport pair

from egopy_configs cimport TOptConfig
from egopy_configs cimport TModelConfig

cdef extern from "ego_wrapper.h":
    cdef cppclass TMatWrap:
        TMatWrap(double *v, size_t n_rows, size_t n_cols) # except +
        TMatWrap()
        size_t GetNRows()
        size_t GetNCols()
        double GetValue(size_t, size_t) # except +

cdef extern from "ego_wrapper.h":
    cdef cppclass TCovWrap:
        TCovWrap(const char *covName, size_t dim_size, vector[double]) # except +
        TMatWrap CalculateKernel(TMatWrap)

cdef extern from "ego_wrapper.h":
    cdef cppclass TMeanWrap:
        TMeanWrap(const char *meanName, size_t dim_size, vector[double]) # except +

cdef extern from "ego_wrapper.h":
    cdef cppclass TLikWrap:
        TLikWrap(const char *likName, size_t dim_size, vector[double]) # except +

cdef extern from "ego_wrapper.h":
    cdef cppclass TAcqWrap:
        TAcqWrap(const char *meanName, size_t dim_size, vector[double]) # except +
        pair[TMatWrap, TMatWrap] EvaluateCriteria(TMatWrap) # except +
        void SetParameters(vector[double]) # except +

cdef extern from "ego_wrapper.h":
    cdef cppclass TInfWrap:
        TInfWrap(const char *infName) # except +

cdef extern from "ego_wrapper.h":
    cdef cppclass TDistrWrap:
        TDistrWrap()
        TDistrWrap(TDistrWrap)
        double GetMean() # except +
        double GetSd() # except +


cdef extern from "ego_wrapper.h":
    ctypedef double (*FOptimCallback)(const vector[double]&, void*)

cdef extern from "ego_wrapper.h":
    cdef cppclass TModelWrap:
        TModelWrap(TMeanWrap* mean, TCovWrap* cov, TLikWrap* lik, TInfWrap* inf, TAcqWrap* acq) # except +
        void SetData(TMatWrap, TMatWrap) # except +
        void SetConfig(TModelConfig)
        vector[TDistrWrap] GetPrediction(TMatWrap) # except +
        vector[double] GetParameters() # except +
        void Optimize(FOptimCallback, void*) # except +
        pair[TMatWrap, TMatWrap] GetData() # except +
        void OptimizeHyp() # except +

cdef extern from "ego_wrapper.h":
    void SetDebugLogLevel()

cdef extern from "ego_wrapper.h" namespace "NLa":
    double NormCdf(double)
    double NormPdf(double)

