# -*- coding: utf-8 -*-
'''
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
main_drone.py:
Version: 1.0
Python implementation of the drone control loop.
It runs on Raspberry Pi.
dependencies:
-dronekit
-pyserial
-python 3
Authors: Hazem Sallouha, Franco Minucci
emails: {name.surname}@kuleuven.be
'''
from dronekit import connect, VehicleMode, LocationGlobalRelative, LocationGlobal, mavutil
from geodesy import *
import time
from serial import Serial
from drone_ctrl import *
from wifi import *
from math import degrees

################################################################################

#this depends from the pi, it needs to be a parameter
connection_string = '/dev/ttyACM0'
print("\nConnecting to vehicle on: %s" % connection_string)
vehicle = connect(connection_string, wait_ready=True)
vehicle.wait_ready('autopilot_version')
print(" Autopilot Firmware version: %s" % vehicle.version)
vehicle.mode = VehicleMode("STABILIZE")

#Confirm current value on vehicle by re-downloading commands
cmds = vehicle.commands
cmds.download()
cmds.wait_ready()

running = True
################################################################################

#Serial connection to WiFi module
wifi = Serial("/dev/ttyUSB0",115200)


md = deque(maxlen=1)

def escape(t1,t2):
    #current coordinates
    lat,lon = t1[0],t1[1]
    dp1 = geo2ECEF(t1[0],t1[1],t1[2])
    #other drone coordinates
    dp2 = geo2ECEF(t2[0],t2[1],t2[2])
    #Convert second drone coordinates into East, North, Up relative frame
    x,y,z = ECEF2ENU(lat,lon,dp1[0],dp1[1],dp1[2],dp2[0],dp2[1],dp2[2])
    #How far is the second drone,
    dist = sqrt(x**2+y**2)
    xn,yn = 0
    #Less than 6m is too close, we move towards the opposite coordinates
    if dist < 6:
        xn = -x
        yn = -y
        #convert back from local ENU frame to geodetic
        la,lo,h = ECEF2geo(ENU2ECEF(lat,lon,dp1[0],dp1[1],dp1[2],xn,yn,z))
        #return escape point
        return (degrees(la),degrees(lo),h)
    #If the other is far, nothing to return
    return None



def listen(lat,lon,alt):
    #Update the coordinates in the Wi-Fi module and retreive incoming messages
    d = wifiRW(wifi,(lat,lon,alt),1,buffer=md)
    
    if d is not None:
        #another drone is broadcasting its position, check if escape is needed
        e = escape((lat,lon,alt),(d[1],d[3],d[5]))

        if e is not None:
            #If escape is needed, return the escape point
            return LocationGlobalRelative(e[0],e[1],5)
    #else: nothing to return
    return None

#Hardcoded for testing purposes
mission = [
     #LocationGlobalRelative(50.862077, 4.684963, 5),#In front of the castle
     #LocationGlobalRelative(50.862003, 4.684558, 5),#In front of the castle
     #LocationGlobalRelative(50.862428, 4.684319, 5),#In front of the castle
     LocationGlobalRelative(50.862262, 4.684424, 5)#,
     #LocationGlobalRelative(50.861970, 4.682687, 5) #birds place
 ]


esc_point = None
arm_and_takeoff(vehicle, 4)
vehicle.airspeed = 1.5
time.sleep(10)

lat,lon,alt = getGPS(vehicle)

ind = 0
eind = []

while running:
    if ind == len(mission):
        ind = 0
    try:
        #update current position
        lat,lon,alt = getGPS(vehicle)
        ref = geo2ECEF(lat,lon,alt)
        #check wifi: if another drone is close, compute escape point
        esc_point = listen(lat,lon,alt)

        if esc_point is not None:
            #another drone is close!
            #replace current target point with the escape point
            mission.insert(ind,esc_point)
            #save the index of the escape point into a list
            eind.append(ind)
        
        elif esc_point is None and len(eind) > 0:
            #all clear, we can remove the escape point
            mission.pop(eind[0])
            #follow in order
            eind.pop(0)
        
        #get the target coordinates
        tar_lat, tar_lon, tar_alt = mission[ind]
        tar = geo2ECEF(lat,lon,alt)
        #compute the relative position in East North Up
        x,y,z = ECEF2ENU(lat,lon,ref[0],ref[1],ref[2],tar[0],tar[1], tar[2])
        #How far is the point?
        distance = sqrt(x**2+y**2)
        #if closer than 5m, it is good enough
        if distance < 5:
            ind+=1
        #move to the next target point
        goto_position_target_global_int(vehicle,mission[ind])
        
        time.sleep(0.5)
        
    except:
        break

print("Returning to Launch")
vehicle.mode = VehicleMode("RTL")
time.sleep(0.5)

# Close vehicle object before exiting script
print("Close vehicle object")
vehicle.close()

print("Completed")
