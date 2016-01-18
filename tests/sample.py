# -*- coding: utf-8 -*-
"""
Created on Sun Jan 17 23:17:55 2016

@author: alexeyche
"""

#!/usr/bin/env python

import numpy as np
import random

import egopy as ego
from egopy import Cov, Mean, Lik, Inf, Model, Acq
from math import pi, cos, sin, pow
from matplotlib import pyplot as plt

ego.setDebugLogLevel()
D=1
cov = Cov("cSqExpISO", D, [0.0,5])
mean = Mean("mConst", D)
lik = Lik("lGauss", D)
inf = Inf("iExact")
acq = Acq("aEI", D)
model = Model(mean, cov, lik, inf, acq)

gridSize = 100

X = np.asarray([[0.5], [0.25]])
Y = np.asarray([0.5, 0.33])

model.setConfig({
    "Seed": 1, 
    "HypOptMethod": "CG",
    "HypOptMaxEval": 1,
    "HypOptFreq": 1,
    "AcqOptMethod" : "GN_DIRECT",
    "MaxEval": 50
})

model.setData(X, Y)

Xgrid = np.linspace(0.0, 1.0, gridSize)
preds = model.getPrediction(Xgrid)
Ymean = np.asarray([ y.getMean() for y in preds ])
Ysd = np.asarray([ y.getSd() for y in preds ])

plt.figure(1)       
plt.plot(Xgrid, Ymean, '-', color='green', linewidth=2.0)
plt.fill_between(Xgrid, Ymean-Ysd, Ymean+Ysd, facecolor='green', interpolate=True, alpha=0.2)
plt.plot(X, Y, 'bp')
