from analysis import *
from matplotlib import pyplot as pt
from numpy.random import geometric, poisson, exponential
from scipy.stats import ks_2samp


f1 = open("r7-d0-result.txt")
raw1 = f1.read()
f1.close()

f2 = open("r7-d1-result.txt")
raw2 = f2.read()
f2.close()

v = countSuccessT1(raw1,raw2)
dk = []
for i in range(1,len(v)):
    dk.append(v[i]-v[i-1])

h = {}

for d in dk:
    try:
        h[d] += 1
    except:
        h[d] = 1
for n in h:
    h[n] /= len(v)

x = sorted(list(h.keys()))
y = [h[n] for n in x]

expdata = poisson(y[0],len(dk))

print(ks_2samp(dk,expdata))

pt.plot(x,y)
pt.show()