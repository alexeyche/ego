#!/usr/bin/env python

import numpy as np
import random

import egopy as ego
from egopy import Cov, Mean, Lik, Inf, Model, Acq
from math import pi, cos, sin, pow
from matplotlib import pyplot as plt

ego.setDebugLogLevel()

def scale_to(x, minv=None, maxv=None, a=0.0, b=1.0):
    if minv is None:
        minv = min(x)
    if maxv is None:
        maxv = max(x)
    return ((b-a)*(x - minv)/(maxv-minv)) + a

def plot_mins(X, Y, plot_spec):
    minY = min(Y)
    minYIds = np.where(Y == minY)[0]
    plt.plot(X[minYIds], [minY]*minYIds.shape[0], plot_spec)
    

seed = 9
if not seed is None:
    rng = np.random.RandomState(seed)
else:
    rng = np.random.RandomState()

def lhs_sample(n, rng):
    return (rng.permutation(range(1,n+1)) - rng.random_sample(n))/float(n)

def rosen(x):
    a = 1
    b = 100
    return ((a - x[0])**2 + b * (x[1]- x[0]*x[0])**2)/100.0

def branin(x):
    """
% Min = 0.1239 0.8183
% Min = 0.5428 0.1517  => 0.3979
% Min = 0.9617 0.1650
    """
    a = x[0] * 15 - 5;
    b = x[1] * 15; 

    return ((b-(5.1/(4*pi**2))*a**2+5*a/pi-6)**2+10*(1-1/(8*pi))*cos(a)+10)


def periodic1d(x):
    """
    Opt: 0.23719
    """
    return (x-0.3)*(x-0.3) + sin(20*x)*0.2

def sq1d(x):
    return (0.5-x)**2

D, fopt = 1, sq1d

cov = Cov("cSqExpISO", D, [np.log(1.0), np.log(1.0)])
mean = Mean("mConst", D, [1.0])
lik = Lik("lGauss", D, [np.log(0.01)])
inf = Inf("iExact")
acq = Acq("aEI", D, [0.0])
model = Model(mean, cov, lik, inf, acq)

init_size = 5

X = np.zeros((init_size, D))

for di in range(D):
    X[:, di] = lhs_sample(init_size, rng)


Y = np.asarray([ fopt(x) for x in X ])

#np.savetxt("/var/tmp/testfuncs.csv", np.hstack((X, np.asarray([Y]).T)), delimiter=',')
np.savetxt("/var/tmp/testfuncs.csv", np.hstack((X, Y)), delimiter=',')

model.setConfig({
    "Seed": seed, 
    "HypOptMethod": "CG",
    "HypOptMaxEval": 10,
    "HypOptFreq": 1,
    "AcqOptMethod" : "GN_DIRECT",
    "MaxEval": 10
})

model.setData(X, Y)
model.optimizeHyp()

#gridSize = pow(1000, 1.0/D)
gridSize = 1000
#gridSize = 100

grid = list()
for di in range(D):
    grid.append(np.linspace(0.0, 1.0, gridSize))

if D>1:
    points = np.vstack(np.meshgrid(*grid)).reshape(len(grid), -1).T
else:
    points = grid[0]

Ygrid = np.asarray([ fopt(x) for x in points ])

preds = model.getPrediction(points)
Ymean = np.asarray([ y.getMean() for y in preds ])
Ysd = np.asarray([ y.getSd() for y in preds ])
ev, dev = acq.evaluateCriteria(points)
ev = scale_to(ev)
ev = ev.reshape(len(ev))

    
if D == 1:
    plt.figure(1)       
    plt.plot(points, Ymean, '-', color='green', linewidth=2.0)
    plt.plot(points, Ygrid, '-', color='blue')
    plt.fill_between(points, Ymean-Ysd, Ymean+Ysd, facecolor='green', interpolate=True, alpha=0.2)
    plt.plot(X, Y, 'bp')
    plt.plot(points, ev, '-', color='red')
    plot_mins(points, ev, 'rd')
    plot_mins(points, Ymean, 'bd')
    
else:
    ev = ev.reshape((gridSize, gridSize))
    Ymean = Ymean.reshape((gridSize, gridSize))
    Ysd = Ysd.reshape((gridSize, gridSize))
    Ygrid = Ygrid.reshape((gridSize, gridSize))
    
    plt.figure(1)
    plt.contourf(grid[0], grid[1], Ygrid, alpha=0.5)
    plt.contour(grid[0], grid[1], Ymean, linestyles='dashed')
    plt.figure(2)
    plt.contourf(grid[0], grid[1], ev)
    

#ego.optimizeModel(model, "CG", {"MaxEval": 100})

#model.optimize(fopt)

#X, Y = model.getData()

