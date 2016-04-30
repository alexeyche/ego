# -*- coding: utf-8 -*-

import GPy
from matplotlib import pyplot as plt
import numpy as np
import os
from numpy import genfromtxt

def readMat(f, sep=" "):
    f = os.path.expanduser(f)
    return genfromtxt(f, delimiter=sep)


data = readMat('~/ego/stdp2.ssv', " ")
data = data[:,:]

X = data[:,:-1]
Y = data[:,-1]
Y = np.asarray([Y]).T
D = X.shape[1]
kernel = GPy.kern.Matern52(input_dim=1, variance=1., lengthscale=1.) #+ GPy.kern.Bias(1)

m = GPy.models.GPRegression(X,Y,kernel)
fig = m.plot()
m.optimize_restarts(num_restarts = 10)
fig = m.plot()