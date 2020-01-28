from math import e, factorial,log, gamma, sqrt, floor
from matplotlib import pyplot as pt
f1 = open("r7-d1-result.txt")
raw1 = f1.read()
f1.close()

ToA = []
Ttx = 1
flag = False

for i in range(len(raw1)):
    if raw1[i] != 'B':
        continue
    if not flag:
        ToA.append(i)
        counter = 0
    if counter < Ttx:
        counter += 1
        flag = True
    else:
        counter = 0
        flag = False

print(len(ToA))

dif = []
for j in range(1,len(ToA)):
    dif.append(ToA[j]-ToA[j-1]-Ttx)

dist = {}
lm = Ttx*len(dif)/len(raw1)

for d in dif:
    try:
        dist[d] += 1
    except:
        dist[d] = 1

print(dist)
x = sorted(dist.keys())
y = []
for t in x:
    y.append(dist[t]/len(dif))

with open('dist.txt','w') as f:
    for p in x:
        f.write(str(p)+'\t')
    f.write('\n')
    
    for p in y:
        f.write(str(p)+'\t')

exp = []
for k in range(1,19):
    #exp.append((y[0]) * e**(-(y[0])*(k-1)))
    exp.append(y[0]*(1-y[0])**(k-1))

expected_value = 1/y[0]
print(1/y[0])
pt.bar(x,y, width=2, label="Time difference between consecutive transmissions")
pt.plot(x,exp[0:len(x)],'r',label="Analytical")
pt.xticks(x, [str(int(n)) for n in x])
pt.legend()
pt.show()
