

from libcpp.string cimport string
from libcpp cimport bool

cdef extern from "ego_wrapper.h" namespace "NOpt":
    cdef struct TOptimizeConfig:
        int MaxEval
        double Tol

cdef extern from "ego_wrapper.h":
    cdef struct TModelConfig:
        int Seed
        string AcqOptMethod
        string HypOptMethod
        int MaxEval
        int HypOptMaxEval
        int HypOptFreq


cdef TModelConfig modelConfigFromDict(dict d)
cdef TOptimizeConfig optParamsFromDict(dict d)