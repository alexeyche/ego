

cdef TOptConfig optParamsFromDict(dict d):
    cdef TOptConfig p
    p.Method = d.get("Method", p.Method)
    p.MinimizersNum = d.get("MinimizersNum", p.MinimizersNum)
    p.MaxEval = d.get("MaxEval", p.MaxEval)
    p.Tolerance = d.get("Tolerance", p.Tolerance)
    return p

cdef TSolverConfig solverConfigFromDict(dict d):
    cdef TSolverConfig c
    c.InitSampleSize = d.get("InitSampleSize", c.InitSampleSize)
    c.BatchSize = d.get("BatchSize", c.BatchSize)
    c.IterationsNum = d.get("IterationsNum", c.IterationsNum)
    c.BatchPolicy = d.get("BatchPolicy", c.BatchPolicy)
    c.HyperOpt = optParamsFromDict(d.get("HyperOpt", {}))
    c.AcqOpt = optParamsFromDict(d.get("AcqOpt", {}))
    return c

cdef TModelConfig modelConfigFromDict(dict d):
    cdef TModelConfig c
    c.Seed = d.get("Seed") if d.get("Seed") else c.Seed
    return c

cdef TVariable variableFromDict(dict d):
    cdef TVariable v
    v.Name = d["Name"]
    v.Min = d["Min"]
    v.Max = d["Max"]
    v = FillVariableWithType(v, d["Type"])
    return v

cdef TProblemConfig problemConfigFromDict(dict d):
    cdef TProblemConfig c
    if not "Variables" in d:
        raise Exception("Need `Variables' in problem config dictionary")
    c.Name = d["Name"]
    for i, vd in enumerate(d["Variables"]):
        v = variableFromDict(vd)
        v.Id = i
        c.Variables[v.Name] = v
    c.DimSize = len(c.Variables)
    return c


cdef TSolverSpec makeSolverSpec(dict solverConfig, dict problemConfig):
    cdef TSolverSpec s
    s.SolverConfig = solverConfigFromDict(solverConfig)
    s.ProblemConfig = problemConfigFromDict(problemConfig)
    return s
