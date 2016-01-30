

from libcpp.string cimport string
from libcpp cimport bool

cdef extern from "ego_wrapper.h":
    cdef struct TOptConfig:
        string Method
        int MinimizersNum
        int MaxEval
        double Tolerance

cdef extern from "ego_wrapper.h":
    cdef struct TModelConfig:
        int Seed
        TOptConfig HyperOpt
        TOptConfig AcqOpt


cdef TModelConfig modelConfigFromDict(dict d)
cdef TOptConfig optParamsFromDict(dict d)