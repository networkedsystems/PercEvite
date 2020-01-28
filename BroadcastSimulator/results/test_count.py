Trx = 4
Ttx = 2
def count_BS(V1,V2):
    Cb = {}
    evt = 0
    for i in range(len(V2)):
        if V2[i] != 'S':
            continue
        else:
            
            if V2[i-1] != 'S' or evt == 0:
                Cb[i] = 0
            evt += 1
            if evt == Trx:
                evt = 0
    for k in Cb:
        print("V1="+V1[k:k+Trx])
        print("V2="+V2[k:k+Trx])
        for i in range(k,Trx+k):
            if V1[i] == 'B':
                Cb[k] += 1
        Cb[k] /= Ttx
    

    return Cb
            
            
        
