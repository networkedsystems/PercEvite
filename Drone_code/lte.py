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

parse protocol:
returns a series of commands
(cmd_id, data)
'''
import serial
from time import sleep
from collections import deque
from threading import Thread

class LTE:

    def __init__(self,port,pin_code):
        
        self.port = serial.Serial(port,115200,timeout=2)
        self.port.write(b'at+cpin=?')
        pin_response = self.port.read(100)
        if b'READY' not in pin_response:
            self.port.write(b'AT+CPIN='+pin_code.encode('utf-8')+b'\n')
        try:
            self.port.send(b'AT+CMGF=1\n')
        except:
            pass
        sleep(2)
        self.queue = deque()
        self.reader = Thread(target=self._read_thrd)
        self.reader.start()

    def send_sms(self,number,txt):
        try:
            self.port.write(b'AT+CMGS="'+number.encode('utf-8')+b'"\n')
            sleep(1)
            self.port.write(txt.encode()+chr(26))
            print(self.port.read(100))
        except:
            print(self.port.read(1000))

    def clear_queue(self):
        self.queue.clear()

    def _read_thrd(self):
        
        while True:
            
            try:
                rcv = self.port.read_all()
                print(rcv)
                if len(rcv) > 0:
                    rcvs = rcv.split(b',')
                    print(rcvs)
                    for s in rcvs:
                        if b'$' in s:
                            self.queue.append(s.split(b'$')[1])
                            print(self.queue[-1])

            except KeyboardInterrupt:
                return
            sleep(0.5)
            
    def parse(self,vehicle):
        for m in self.queue:
            if 'RTL' in m.upper():
                vehicle.mode = VehicleMode("RTL")

            elif 'GO' in m.upper():
                ms = m.split('@')
                coords = ms[1].split(';')
                #1) get local coordinates
                #2) convert local coordinates to (X,Y,Z) Earth centered 
                #3) convert x,y,z to Earth centered (X,Y,Z) referred to current drone location
                #4) Convert back ECEF->Latitude,longitude,H
                #5) Call simple_goto

                x = float(coords[0])
                y = float(coords[1])
                z = float(coords[2])
                geo = getGPS(vehicle)
                #convert GPS to earth centered
                k = geo2ECEF(geo[0],geo[1],geo[2]) #(Xref,Yref,Zref) -> ECEF
                enu_point = ENU2ECEF(geo[0],geo[1],k[0],k[1],k[2],x,y,z)
                geo_point = ECEF2geo(enu_point[0],enu_point[1],enu_point[2])
                pointN = LocationGlobalRelative(geo_point[0], geo_point[1], geo_point[2])
                mission.append(pointN)
        lte.clear_queue()

if __name__=='__main__':
    lte = LTE('COM49','9112')
    #lte = LTE('COM49','1358')
    running = True
    lte.send_sms('0487889057','Python test happy weekend')
    while running:
        try:
            #print("reading")
            lte.port.write(b'at+cmgrd=0\n')
            lte.send_sms('0487889057','Python test happy weekend')
        except:
            print("nothing")
            lte.reader.join()
            running = False
        sleep(10)
        #print(lte.queue)
    lte.reader.join()
##    
    # lte.port.write('at+cmgl="all"\r\n')
    # sleep(3)
    # lte.send_sms('0487889057','Python test happy weekend')
    lte.port.close()

