

cdef TOptConfig optParamsFromDict(dict d):
    cdef TOptConfig p
    p.Method = d.get("Method", p.Method)
    p.MinimizersNum = d.get("MinimizersNum", p.MinimizersNum)
    p.MaxEval = d.get("MaxEval", p.MaxEval)
    p.Tolerance = d.get("Tolerance", p.Tolerance)
    return p

cdef TModelConfig modelConfigFromDict(dict d):
    cdef TModelConfig c
    c.Seed = d.get("Seed") if d.get("Seed") else c.Seed
    c.HyperOpt = optParamsFromDict(d.get("HyperOpt", {}))
    c.AcqOpt = optParamsFromDict(d.get("AcqOpt", {}))
    return c