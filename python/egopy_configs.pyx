

cdef TOptimizeConfig optParamsFromDict(dict d):
    cdef TOptimizeConfig p
    p.MaxEval = d.get("MaxEval", p.MaxEval)
    p.Tol = d.get("Tol", p.Tol)
    return p

cdef TModelConfig modelConfigFromDict(dict d):
    cdef TModelConfig c

    c.Seed = d.get("Seed") if d.get("Seed") else c.Seed
    c.AcqOptMethod = d.get("AcqOptMethod", c.AcqOptMethod)
    c.HypOptMethod = d.get("HypOptMethod", c.HypOptMethod)
    c.HypOptMaxEval = d.get("HypOptMaxEval", c.HypOptMaxEval)
    c.HypOptFreq = d.get("HypOptFreq", c.HypOptFreq)
    c.MaxEval = d.get("MaxEval", c.MaxEval)
    return c