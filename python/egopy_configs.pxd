

from libcpp.string cimport string
from libcpp cimport bool
from libcpp.map cimport map

cdef extern from "ego_wrapper.h":
    cdef struct TOptConfig:
        string Method
        int MinimizersNum
        int MaxEval
        double Tolerance

cdef extern from "ego_wrapper.h":
    cdef struct TSolverConfig:
        TOptConfig HyperOpt
        TOptConfig AcqOpt
        int InitSampleSize
        int BatchSize
        int IterationsNum
        int HyperOptFreq
        string BatchPolicy


cdef extern from "ego_wrapper.h":
    cdef struct TModelConfig:
        int Seed



cdef extern from "ego_wrapper.h":
    cdef struct TVariable:
        string Name
        double Min
        double Max
        int Id

cdef extern from "ego_wrapper.h":
    TVariable FillVariableWithType(TVariable, string)


cdef extern from "ego_wrapper.h":
    cdef struct TProblemConfig:
        string Name
        int DimSize
        map[string, TVariable] Variables

cdef extern from "ego_wrapper.h":
    cdef struct TSolverSpec:
        TSolverConfig SolverConfig
        TProblemConfig ProblemConfig

cdef TSolverConfig solverConfigFromDict(dict d)
cdef TModelConfig modelConfigFromDict(dict d)
cdef TOptConfig optParamsFromDict(dict d)
cdef TVariable variableFromDict(dict d)
cdef TProblemConfig problemConfigFromDict(dict d)
cdef TSolverSpec makeSolverSpec(dict solverConfig, dict problemConfig)