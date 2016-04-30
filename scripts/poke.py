#!/usr/bin/env python

import egopy as ego

from egopy import Cov, Mean, Lik, Inf, Model, Acq
from egopy import Solver

ego.setDebugLogLevel()

import numpy as np
import os
from matplotlib import pyplot as plt
from numpy import genfromtxt
import random

def readMat(f, sep=" "):
    f = os.path.expanduser(f)
    return genfromtxt(f, delimiter=sep)


data = readMat('~/ego/stdp2.ssv', " ")
data = data[:,:]

X = data[:,:-1]
Y = data[:,-1]
D = X.shape[1]


model = Model(
    Mean("mConst", D), 
    Cov("cSqExpARD", D),
    Lik("lGauss", D),
    Inf("iExact"),
    Acq("aEI", D),
)

model.setData(X, Y)
model.update()
model.optimizeHypers({"Method": "CG"})

Xtest = np.linspace(0.0, 1.0, num=300)
preds = model.getPrediction(Xtest)
    
ym = np.asarray([ p.getMean() for p in preds ])
ysd = np.asarray([ p.getSd() for p in preds ])

plt.plot(X, Y, 'bp')    
plt.plot(Xtest, ym, '-', color='green', linewidth=2.0)
plt.fill_between(Xtest, ym-ysd, ym+ysd, facecolor='green', interpolate=True, alpha=0.2)
    