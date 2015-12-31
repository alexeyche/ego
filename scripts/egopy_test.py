#!/usr/bin/env python

import egopy as ego

from egopy import Cov, Mean, Lik, Inf

ego.setDebugLogLevel()

import numpy as np
from matplotlib import pyplot as plt

L = 10

l = np.linspace(0, 100, num=L)

X = np.vstack((l,l)).T 
D = X.shape[1]

Y = 2*np.sum(X,1) + np.random.normal(0, 1.0, L)


cov = Cov("cMaternARD5", D, [1.0]*(D+1))
mean = Mean("mSum(mLinear, mConst)", D, [1.0]*(D+1))
lik = Lik("lGauss", D, [1.0])
inf = Inf("iExact")
