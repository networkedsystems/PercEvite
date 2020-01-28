from math import e, factorial,log, gamma, sqrt, floor, exp
from matplotlib import pyplot as pt
from numpy.random import geometric, poisson, exponential
from scipy.stats import ks_2samp
from scipy.stats import norm#,poisson
from numpy import linspace
import re

def computeEvents(V, ttx, trx, tn):
    Eb = 0
    Es = 0
    En = 0

    for c in V:
        if c == 'B':#broadcast event is a B sorrounded by 14 X
            Eb += 1
        elif c == 'S':
            Es += 1
        elif c== 'N':
            En += 1
    
    return (Eb,Es/trx,En/tn)

def geom_dist(p,maxK, k0=0):
    return [p*(1-p)**(k-k0) for k in range(0,maxK)]

def poisson_dist(lam,maxK):
    return [e**(k*log(lam)-(lam)-log(gamma((k+1)))) for k in range(0,maxK)]

def exponential_dist(lam, maxK):
    return [lam*(e**(-lam*k)) for k in range(0,maxK)]

def gamma_dist(beta,maxK,alpha):
    k = (beta**alpha)/gamma(alpha)
    return [k*(x**(alpha-1))*exp(-beta*x) for x in range(maxK)]

def rmse(v1,v2):
    rmse = 0
    for x1,x2 in zip(v1,v2):
        rmse += (x1-x2)**2
    return sqrt(rmse/len(v1))

def checkSuccess(Vtx,Vrx):
    Cb = 0
    
    for b,s in zip(Vtx,Vrx):
        if b =='B' and s =='S':
            Cb+=1
    return Cb#number of successfull events

def findScanInterval(Vtx,Vrx,Ttx,Trx):
    scan_intervals = [{'s':m.start(),'e':m.end(),'n':0} for m in re.finditer(Trx*'S', Vrx)]
    for si in scan_intervals:
        si['n'] = Vtx[si['s']:si['e']].count(Ttx*'B')
    
    return scan_intervals

def generateHistogram(sc_int,Trx):
    hist = [0 for x in range(Trx+1)]
    for si in sc_int:
        hist[si['n']]+=1
    for i in range(Trx+1):
        hist[i] /= len(sc_int)
    return hist


def genWindowHistogram(r1,r2,wnd,Eb):
    windows = []
    for i in range(0,len(r1),wnd):
        count = 0
        for j in range(i,i+wnd):
            try:
                if r1[j]=='S' and r2[j]=='B':
                    count += 1
            except:
                pass
        windows.append(count)
     
    result = [0 for i in range(wnd)]
    for n in range(len(result)):
        result[n] = windows.count(n)/len(windows)
    print(sum(result))
    return result