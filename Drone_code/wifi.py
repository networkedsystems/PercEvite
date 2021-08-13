##© Copyright 2018, Networked Systems group, ESAT-TELEMIC,KU Leuven.
##All rights reserved.
##
##Redistribution and use in source and binary forms, with or without
##modification, are permitted provided that the following conditions are met:
##1. Redistributions of source code must retain the above copyright
##   notice, this list of conditions and the following disclaimer.
##2. Redistributions in binary form must reproduce the above copyright
##   notice, this list of conditions and the following disclaimer in the
##   documentation and/or other materials provided with the distribution.
##3. All advertising materials mentioning features or use of this software
##   must display the following acknowledgement:
##   This product includes software developed by Networked Systems group,
##   ESAT-TELEMIC,KU Leuven.
##4. Neither the name of the Networked Systems group, ESAT-TELEMIC,KU Leuven nor the
##   names of its contributors may be used to endorse or promote products
##   derived from this software without specific prior written permission.
##
##THIS SOFTWARE IS PROVIDED BY ESAT-TELEMIC-KU Leuven, ''AS IS'' AND ANY
##EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
##WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
##DISCLAIMED. IN NO EVENT SHALL Networked Systems group, ESAT-TELEMIC,KU Leuven
##BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
##OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
##OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
##OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
##WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
##ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
##THE POSSIBILITY OF SUCH DAMAGE.
###############################################################################
from random import random
from time import sleep
from collections import deque




def broadcastGPS(port, coords, idn):
    lat,lon,alt = coords

    snd = 'D'+str(idn)+'-'+str(lat)+'N_'+str(lon)+'E_'+str(alt)+'\n'
    
    m = port.write(snd.encode('utf-8'))

def scan(port, message):
    message = port.readline()
    if b'>' in message:
        return message
    try:
        ms = str(message[:-2]).strip("**'").split('-')
            
        Did = ms[0][2:]
        if 'DD' in Did:
            Did = Did[1:]
        elif 'D' not in Did:
            Did = 'D'+Did
        coords = ms[1].split('_')
        lat = float(coords[0][0:-1])
        lon = float(coords[1][0:-1])
        alt = float(coords[2])
        D = (Did,lat,coords[0][-1],lon,coords[1][-1],alt)
        return D
    except:
        return None


def wifiRW(port,coords,idn,buffer = deque(maxlen=4)):
    message = port.readline()

    if b'>' in message:
        broadcastGPS(port,coords,idn)
        return
    else:
        D = scan(message)
        if D is not None:
            buffer.append(D)
        return D
        

#####Test program #####
if __name__ == '__main__':
    from serial import Serial
    md = deque(maxlen=8)
    lat = 50.862754
    lon = 4.683839
    alt = 5
    #wifi = Serial("/dev/serial0",115200)
    wifi1 = Serial("COM4",115200,xonxoff=True)
    wifi2 = Serial("COM9",115200,xonxoff=True)
    
    while True:
        #d = scan(wifi)
        #print(d)
        d1 = wifiRW(wifi1,(random()*50,random()*5,alt),1,md)
        d2 = wifiRW(wifi2,(random()*50,random()*5,alt),2,md)
        try:
            # sleep(0.1)
            print(md)
        except:
            break

