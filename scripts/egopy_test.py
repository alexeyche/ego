#!/usr/bin/env python

from egopy import Cov


import numpy as np

L = 10

l = np.linspace(0, 100, num=L)

X = np.vstack((l,l)).T 
#X = l
D = X.shape[1]

Y = 2*np.sum(X,1) + np.random.normal(0, 1.0, L)
#Y = 2*X + np.random.normal(0, 1.0, L)

cov = Cov("cMaternARD1", D, [1.0]*(D+1))
r = cov.calculateKernel(X)
