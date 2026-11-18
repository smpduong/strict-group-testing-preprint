import subprocess, itertools
from math import comb
def ev(k,sigs):
    return subprocess.run(["./gt","eval",str(k),"0"]+list(map(str,sigs)),capture_output=True,text=True).stdout
def t(k):
    j=0
    while comb(j,j//2)<k: j+=1
    return j
for k in range(2,9):
    out=ev(2,[1,1]+[2]*k)
    line=[l for l in out.split("\n") if l.startswith("W=3")][0]
    print("K2,%d"%k,line,"expected",t(k)+2)
for m,exp in((5,8),(6,9),(7,10)):
    sigs=[(1<<a)|(1<<b) for a,b in itertools.combinations(range(m),2)]+[0,0,0]
    print(m,len(sigs),[l for l in ev(m,sigs).split("\n") if l.startswith("max_r")][0],"expected cost",exp)
print("K4+3",[l for l in ev(4,[(1<<a)|(1<<b) for a,b in itertools.combinations(range(4),2)]+[0,0,0]).split("\n") if l.startswith("max_r")][0])
print("C4",[l for l in ev(2,[1,2,1,2]).split("\n") if l.startswith("W=3")])
