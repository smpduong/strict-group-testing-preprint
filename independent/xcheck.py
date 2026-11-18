import os
import random, subprocess, itertools
from verify import r_le
GT2 = os.path.join(os.path.dirname(os.path.abspath(__file__)), "gt2")   # build: gcc -O2 -o independent/gt2 independent/gt2.c
rnd=random.Random(7); bad=0; N=400
for t in range(N):
    m=rnd.randint(2,7)
    allsets=[(i,) for i in range(m)]+list(itertools.combinations(range(m),2))+[()]
    cnt=rnd.randint(1,min(len(allsets),10))
    sets=rnd.sample(allsets,cnt)
    # min q by verifier
    q=0
    while not r_le(list(range(m)),sets,q): q+=1
    inp=f"{m} {len(sets)} "+" ".join(str(sum(1<<e for e in C)) for C in sets)+"\n"
    got=int(subprocess.run([GT2,"chi"],input=inp,capture_output=True,text=True).stdout)
    if got!=q:
        bad+=1; print("MISMATCH",m,sets,q,got)
print("random families:",N,"mismatches:",bad)
