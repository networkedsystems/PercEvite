import numpy as np

def X(x,y,z,Tx,Ty,Tz):
    return x*Tx/(x*Tx+y*Ty+z*Tz)

x = np.float32(.1)
y = np.float32(.85)
z = np.float32(0.05)

Tx = np.float32(60.0)
Ty = np.float32(15.0)
Tz = np.float32(100.0)

S = X(x,y,z,Tx,Ty,Tz)
B = X(y,x,z,Ty,Tx,Tz)
N = X(z,y,x,Tz,Ty,Tx)

a = np.array([[S,B,N], [S,B,N], [S,B,N]])
print('%.6f;%.6f;%.6f'%(S,B,N))
print(a*a*a*a*a*a*a*a*a*a*a*a*a*a)