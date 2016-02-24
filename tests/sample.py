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

def sample(K):
    return np.random.multivariate_normal(np.zeros(K.shape[0]), K)


ego.setDebugLogLevel()
D=1
cov = Cov("cSqExpISO", D, [np.log(0.01), np.log(0.5)])
mean = Mean("mConst", D, [0.0])
lik = Lik("lGauss", D, [np.log(0.01)])
inf = Inf("iExact")
acq = Acq("aEI", D)
model = Model(mean, cov, lik, inf, acq)



model.setConfig({
    "Seed": 1,
    "HyperOpt": {"Method": "CG", "MaxEval": 100},
    "AcqOpt": {"Method": "CG", "MaxEval": 100},
})


gridSize = 100
Xgrid = np.linspace(0.0, 1.0, gridSize)

K = cov.calculateKernel(Xgrid)
Ysamp = sample(K)

point_ids = [25,75, 0, 30, 60, 50]

X = np.asarray([]).reshape(0, 1)
Y = np.asarray([])

for pnum, pid in enumerate(point_ids):
    x = Xgrid[pid]
    y = Ysamp[pid]
    X = np.vstack((X, np.asarray([x])))
    Y = np.append(Y, y)

    model.setData(X, Y)

    preds = model.getPrediction(Xgrid)
    Ymean = np.asarray([ yp.getMean() for yp in preds ])
    Ysd = np.asarray([ yp.getSd() for yp in preds ])

    plt.figure(pnum)
    plt.plot(Xgrid, Ymean, '-', color='green', linewidth=2.0)
    plt.fill_between(Xgrid, Ymean-Ysd, Ymean+Ysd, facecolor='green', interpolate=True, alpha=0.2)
    plt.plot(X, Y, 'bp')

plt.show()