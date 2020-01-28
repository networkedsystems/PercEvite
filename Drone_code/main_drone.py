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
from escape import *
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
wifi = Serial("/dev/serial0",115200)


def getGPS(vehicle):
    lat = str(vehicle.location.global_frame.lat)
    lon = str(vehicle.location.global_frame.lon)
    alt = str(vehicle.location.global_frame.alt)
    return (lat,lon,alt)

def listen():
    print("Going to",str(w))
    d = wifiRW(wifi,(lat,lon,alt))
    print(d)
    if d is not None:
        e = escape((lat,lon,alt),(d[1],d[3],d[5]))
        if e is not None:
            return LocationGlobalRelative(e[0],e[1],5)
    return None

#Hardcoded for testing purposes
mission = [
     LocationGlobalRelative(50.862105, 4.684649, 5),#In front of the castle
     #LocationGlobalRelative(50.861970, 4.682687, 5) #birds place
 ]


esc_point = None
arm_and_takeoff(vehicle, 4)
vehicle.airspeed = 2
time.sleep(10)

lat,lon,alt = getGPS(vehicle)

flag = False

ind = 0

while running:
    
    if ind == len(mission):
        ind = 0
    try:
        if esc_point is not None:
            w = esc_point
            goto_position_target_global_int(vehicle,w)
            lat,lon,alt = getGPS(vehicle)
            distance = haversine(float(lat),float(w.lat),float(lon),float(w.lon))

            if distance < 5:
                esc_point = None
        
            e = listen()
            if e is not None:
                esc_point = e
        else:
            w = mission[ind]
            goto_position_target_global_int(vehicle,w)
            lat,lon,alt = getGPS(vehicle)
            distance = haversine(float(lat),float(w.lat),float(lon),float(w.lon))

            if distance < 5:
                ind+=1
        
            e = listen()
            if e is not None:
                esc_point = e
        
        time.sleep(0.5)
        
    except KeyboardInterrupt:
        break

print("Returning to Launch")
vehicle.mode = VehicleMode("RTL")
time.sleep(0.5)

# Close vehicle object before exiting script
print("Close vehicle object")
vehicle.close()

print("Completed")
