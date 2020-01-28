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
"""
from dronekit import connect, VehicleMode, LocationGlobalRelative, LocationGlobal, mavutil
from geodesy import *
import time
from serial import Serial
from drone_ctrl import *

from time import sleep, perf_counter
from threading import Thread
from datetime import datetime

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

global running
running = True
################################################################################

#Serial connection to WiFi module
wifi1 = Serial("/dev/serial0",115200)

def getGPS(vehicle):
    lat = str(vehicle.location.global_frame.lat)
    lon = str(vehicle.location.global_frame.lon)
    alt = str(vehicle.location.global_frame.alt)
    return (lat,lon,alt)



lat,lon,alt = getGPS(vehicle)

ind = 0
with open('position.txt','a') as pf:
    print('--------------------',file=pf)
###########################################################
def read_ssid(wi,n):

    start = perf_counter()
    
    global running
    lg = []
    while running:
        s = wi.readline()        
        print(s)
        timestamp = str(datetime.now()).split()[1]
        try:
            lg.append(timestamp+'\t'+s.decode("utf-8")[:-1])
        except:
            pass
            
        if len(lg)>20:
            with open('lora-%d.txt'%n,'a') as log:
                for l in lg:
                    print(l,file=log)
            lg = []
            
    with open('lora-%d.txt'%n,'a') as log:
        for l in lg:
            print(l,file=log)

###########################################################

tr1 = Thread(target=read_ssid,args=(wifi1,1))
  
tr1.daemon = True
   
tr1.start()
    
while running:
    
    if ind == len(mission):
        ind = 0
    try:
        lat,lon,alt = getGPS(vehicle)
        with open('position.txt','a') as pf:
            print((lat,lon,alt),file=pf)
        print(lat,lon,alt)
        time.sleep(0.5)
        
    except KeyboardInterrupt:
        running = False
        time.sleep(1)
    except:
        pass


