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
from datetime import datetime
from threading import Thread
from serial import Serial
from time import sleep, perf_counter
import subprocess

global running
running = True
def read_ssid(wi,n):

    c = 0
    global running
    lg = []
    while running:
        s = wi.readline()        
        print(s)
        timestamp = str(datetime.now()).split()[1]
        if b'>' in s:

            ssid = ("%d-"%n+timestamp+'-%c*'%c)
            wi.write(ssid.encode())
            lg.append('sent:'+ssid)
            c+=1
        if b'sync' in s:
            date = s.decode('utf-8').replace('*','').split('-')[1]
            o = subprocess.check_output('date +%%T -s "%s"'%date,stderr=subprocess.STDOUT,shell=True)
            print(o)
        try:
            lg.append(timestamp+'\t'+s.decode("utf-8")[:-1])
        except:
            pass
        
        if len(lg)>20:
            with open('res-%d.txt'%n,'a') as log:
                for l in lg:
                    print(l,file=log)
            lg = []
            
    with open('res-%d.txt'%n,'a') as log:
        for l in lg:
            print(l,file=log)
            
        
if __name__=='__main__':

    global running
    running = True
    wifi1 = Serial("/dev/serial0",230400)

       
    tr1 = Thread(target=read_ssid,args=(wifi1,2))
  
    tr1.daemon = True
   
    tr1.start()
   

    start = perf_counter()
    while running:
       try:
           print(perf_counter()-start)
           sleep(1)
       except:
           print('out')
           
           running = False
    
    tr1.join()

