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
cov = Cov("cSqExpISO", D, [0.0, np.log(np.sqrt(0.2))])
mean = Mean("mConst", D)
lik = Lik("lGauss", D)
inf = Inf("iExact")
acq = Acq("aEI", D)
model = Model(mean, cov, lik, inf, acq)

model.setConfig({
    "Seed": 1, 
    "HypOptMethod": "CG",
    "HypOptMaxEval": 10,
    "HypOptFreq": 1,
    "AcqOptMethod" : "GN_DIRECT",
    "MaxEval": 50
})


gridSize = 100
Xgrid = np.linspace(0.0, 1.0, gridSize)

points = [
    (0.5, 0.25),
    (0.3, 0.33),
    (0.2, 0.21),
    (0.6, 1.0),
]

X = np.asarray([]).reshape(0, 1)
Y = np.asarray([])

for pi, (x, y) in enumerate(points):
    X = np.vstack((X, np.asarray([x])))
    Y = np.append(Y, y)

    model.setData(X, Y)

    preds = model.getPrediction(Xgrid)
    Ymean = np.asarray([ y.getMean() for y in preds ])
    Ysd = np.asarray([ y.getSd() for y in preds ])

    plt.figure(pi)       
    plt.plot(Xgrid, Ymean, '-', color='green', linewidth=2.0)
    plt.fill_between(Xgrid, Ymean-Ysd, Ymean+Ysd, facecolor='green', interpolate=True, alpha=0.2)
    plt.plot(X, Y, 'bp')

plt.show()