import os


def filter_results(r):
    
    files = filter(lambda name: 'result' in name and 'r%d-'%r in name, os.listdir('.'))

    transmissions = {}

    for fn in files:
        
        f = open(fn)
        raw = f.read()
        f.close()
        dn = fn.replace('r%d-d'%r,'').replace('-result.txt','')
       
        for i in range(len(raw)):
            if raw[i] =='B':
                try:
                    transmissions[i].append(dn)
                except:
                    transmissions[i] = [dn]

    fclean = open("r%d-clean-.txt"%r,"w")
    for i in transmissions:
        if len(transmissions[i])==1:
            fclean.write("%d;%s\n"%(i,transmissions[i][0]))
    fclean.close()

    fres = open('r%d-collisions.txt'%r,'w')
    for t in transmissions:
        fres.write('\t'.join(transmissions[t])+'\n')
    fres.close()
    print('done with: %d'%r)




        
def str_contains(s,items):
    cnt = []
    for i in items:
        cnt.append((i,s.find(i)))

    return cnt

def getN(drones,i):
    
    col = []
    clean = []
    f = open('r%d-collisions.txt'%i,'r')
    raw = f.readlines()
    f.close()
    drs = [str(n) for n in drones]
    for line in raw:
        cnt = str_contains(line,drs)
        if len(cnt) == 1:
            clean.append(line)

        if len(cnt) > 1:
            col.append(line)

    return clean,col
        # f = open('r%d-collision-%d.txt'%(i,len(drones)),'w')
        # for c in col:
        #     print(c,file=f)
        # f.close()
        # f = open('r%d-clean-%d.txt'%(i,len(drones)),'w')
        # for c in clean:
        #     print(c,file=f)
        # f.close()

