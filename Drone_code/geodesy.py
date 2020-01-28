# -*- coding: utf-8 -*-
"""
License:
© Copyright 2018, Networked Systems group, ESAT-TELEMIC,KU Leuven.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by Networked Systems group,
   ESAT-TELEMIC,KU Leuven.
4. Neither the name of the Networked Systems group, ESAT-TELEMIC,KU Leuven nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ESAT-TELEMIC-KU Leuven, ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Networked Systems group, ESAT-TELEMIC,KU Leuven
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
###############################################################################
geodesy.py:
Version: 1.0
Module for coordinates conversions and calculations
It runs on Raspberry Pi.
dependencies:
-pyserial
-python 3
Authors: Hazem Sallouha, Franco Minucci
emails: {name.surname}@kuleuven.be
"""
from math import sin,cos, atan2, atan, acos, asin, radians
import numpy as np
from numpy import sqrt

Re = np.float32(6378.1370E3)  #equatorial radius
Rp = np.float32(6356.7523E3)  #polar radius
R = np.float32(6371E3)  #Average Earth radius in metres

e = np.float32(sqrt(1-(Rp**2)/(Re**2)))  #eccentricity
e2 = np.float64(6.6943799901377997e-3)
a = Re
a1 = np.float64(4.2697672707157535e+4) #a1 = Re*e2
a2 = np.float64(1.8230912546075455e+9) #a2 = a1*a1
a3 = np.float64(1.4291722289812413e+2) #a3 = a1*e2/2
a4 = np.float64(4.5577281365188637e+9) #a4 = 2.5*a2
a5 = np.float64(4.2840589930055659e+4) #a5 = a1+a3
a6 = np.float64(9.9330562000986220e-1) #a6 = 1-e2

def N(phi):
    return  Re/sqrt(1-(e**2)*(sin(phi)**2))

def geo2ECEF(latitude,longitude,height):
    '''Geodetic to Earth Centered Earth Fixed: latitude and longitude are in pip adians, height in meters'''
    lat = radians(latitude)
    lon = radians(longitude)

    Nl = N(lat)
    Nlhc = (Nl+height)*cos(lat)
    X = Nlhc*cos(lon)
    Y = Nlhc*sin(lon)
    Z = (height + (1-e**2)*Nl)*sin(lat)
    return (X,Y,Z)

def ECEF2ENU(Latr,Lonr,Xr,Yr,Zr,Xu,Yu,Zu):
    '''Earth Centered Earth Fixed to East North Up frame conversion
    It works only in the North Emisphere'''
    Latr = radians(Latr)
    Lonr = radians(Lonr)

    x = -sin(Lonr)*(Xu-Xr)+cos(Lonr)*(Yu-Yr)#EAST

    y = -sin(Latr)*cos(Lonr)*(Xu-Xr)-sin(Latr)*sin(Lonr)*(Yu-Yr)+cos(Latr)*(Zu-Zr)

    z = cos(Latr)*cos(Lonr)*(Xu-Xr)+cos(Latr)*sin(Lonr)*(Yu-Yr)+sin(Latr)*(Zu-Zr)

    return (x,y,z)

def haversine(lat1,lat2,lon1,lon2):
    '''Gets latitude and longitude in degrees and returns the distance in meters
    See https://en.wikipedia.org/wiki/Haversine_formula for the details'''
    DPhi = radians(lat2 - lat1)
    DLam = radians(lon2 - lon1)
    a = sin(DPhi/2)**2 + cos(lat1)*cos(lat2)*sin(DLam/2)**2
    c = 2*atan(sqrt(a)/sqrt(1-a))
    return R*c #Orthodromic distance

def ECEF2geo(X,Y,Z):
    geo = [0,0,0]
    zp = abs( Z )
    w2 = X**2 + Y**2
    w = sqrt( w2 )
    r2 = w2 + Z**2
    r = sqrt( r2 )
    geo[1] = atan2( Y, X ) #Lon (final)
    s2 = Z**2 / r2
    c2 = w2/r2
    u = a2/r
    v = a3 - a4/r
    if c2 > 0.3 :
        s = ( zp/r )*( 1.0 + c2*( a1 + u + s2*v )/r )
        geo[0] = asin( s ) #Lat
        ss = s**2
        c = sqrt( 1.0 - ss )

    else:
        c = ( w/r )*( 1.0 - s2*( a5 - u - c2*v )/r )
        geo[0] = acos( c ) #Lat
        ss = 1.0 - c**2
        s = sqrt( ss )

    g = 1.0 - e2*ss
    rg = a/sqrt( g )
    rf = a6*rg
    u = w - rg*c
    v = zp - rf*s
    f = c*u + s*v
    m = c*v - s*u
    p = m/( rf/g + f )
    geo[0] = geo[0] + p #Lat
    geo[2] = f + m*p/2.0 #Altitude
    if  Z < 0.0 :
        geo[0] *= -1.0#Lat

    return tuple(geo) #Return Lat(in radians), Lon (in radians), Altitude in that order

def ENU2ECEF(Latr,Lonr,Xr,Yr,Zr,x,y,z):
    M = np.array([[-sin(Lonr),-sin(Latr)*cos(Lonr),cos(Lonr)*cos(Latr)],
                  [cos(Lonr),-sin(Latr)*sin(Lonr),cos(Latr)*sin(Lonr)],
                  [0,cos(Latr),sin(Latr)]])
    V = np.array([x,y,z])
    
    return M.dot(V) + np.array([Xr, Yr, Zr])

if __name__=="__main__":
    from math import radians
    latitude = 50.862447
    longitude = 4.683828
    height = 50

    Latr = 50.862136
    Lonr = 4.684596
    Hr = 50

    k = geo2ECEF(latitude,longitude,height)
    kr = geo2ECEF(Latr,Lonr ,Hr)
    enu = ECEF2ENU(Latr,Lonr,kr[0],kr[1],kr[2],k[0],k[1],k[2])
    X,Y,Z = ENU2ECEF(Latr,Lonr,kr[0],kr[1],kr[2],enu[0],enu[1],enu[2])
    
    print(enu)
    print((X,Y,Z))
    G = ECEF2geo(k[0],k[1],k[2])
    #print((latitude,longitude,height))

    print(G)
