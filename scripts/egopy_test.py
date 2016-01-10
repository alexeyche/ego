#!/usr/bin/env python

import egopy as ego

from egopy import Cov, Mean, Lik, Inf, Model

ego.setDebugLogLevel()

import numpy as np
import os
from matplotlib import pyplot as plt
from numpy import genfromtxt
import random


def readMat(f, sep=","):
    f = os.path.expanduser(f)
    return genfromtxt(f, delimiter=sep)

def generate_validation_ids(n, nfold):
    ids = range(n)
    val_list = list()
    for fi in range(nfold):
        val_ids = random.sample(range(len(ids)), n/nfold)
        val = set([ids[vi] for vi in val_ids])        
        val_list.append(list(val))
        ids = list(set(ids) - val)
    return val_list    

def plot_validation(Ytest, means, se, mse):
    plt.plot(Ytest, "g-", means, "b-", se, "r-")
    plt.title("MSE: {}".format(mse))


data = readMat('~/mc.csv', " ")
data = data[:,:]


nfold = 5
val_ids = generate_validation_ids(data.shape[0], nfold)

sel, msel, Ytestl, yml = [], [], [], []


for vi in range(len(val_ids)):
    test_idx = val_ids[vi]
    train_idx = [ id for id_set in val_ids[:vi] + val_ids[vi+1:] for id in id_set ]
    
    train_idx = range(800)
    test_idx = range(800, 1000)    
    
    X = data[train_idx,:-1]
    Y = data[train_idx, -1]

    Xtest = data[test_idx, :-1]
    Ytest = data[test_idx, -1]
    
    D = X.shape[1]
    
    cov = Cov("cMaternARD1", D)
    
    mean = Mean("mSum(mLinear, mConst)", D)
    #mean = Mean("mConst", D)
    #mean = Mean("mLinear", D)
    
    lik = Lik("lGauss", D, [np.log(1.0)])
    #lik = Lik("lStudentT", D)
    
    inf = Inf("iExact")
    #inf = Inf("iLaplace")
    
    model = Model(mean, cov, lik, inf)
    model.setData(X, Y)
    model.setConfig({"Seed": 1})
    
    ego.optimize(model, "CG", {"MaxEval": 100})
    
    hyps = model.getHyperParameters()
    preds = model.getPrediction(Xtest)
    ym = np.asarray([ p.getMean() for p in preds ])
    ysd = np.asarray([ p.getSd() for p in preds ])
    #plt.plot(X, Y, 'p', Xtest, ym, '-', Xtest, ym-0.5*ysd, '-', Xtest, ym+0.5*ysd, '-')
    
    se = (ym-Ytest)**2
    mse = sum(se)/len(se)

    plot_validation(Ytest, ym, se, mse)
    #print mse
    sel.append(se)
    msel.append(mse)
    Ytestl.append(Ytest)
    yml.append(ym)
    break
