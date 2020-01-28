from math import e, factorial,log, gamma, sqrt
from matplotlib import pyplot as pt
from numpy.random import geometric, poisson, exponential
from scipy.stats import ks_2samp


f = open("cbdata.txt")
v = [int(x)+1 for x in f.readlines()]
f.close()
lam = 0.5275924670273324
poidata = poisson(lam ,len(v))
expdata = exponential(lam,len(v))
geodata = geometric(lam,len(v))
e= ks_2samp(v,expdata)
d = ks_2samp(v,poidata)
g = ks_2samp(v,geodata)
print('exponential',e)
print('poisson',d)
print('geometric',g)


