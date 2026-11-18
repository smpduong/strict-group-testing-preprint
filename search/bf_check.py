# Independent brute-force check of r(W) (pure set-cover over all pool subsets) vs ./gt eval, on random small designs
import random, itertools, subprocess
def rW_bf(sigs, W, cap=5):
    n=len(sigs)
    E=[i for i in range(n) if sigs[i]&~W==0]
    cs=[]
    for r in range(3):
        for C in itertools.combinations(E,r):
            u=0
            for c in C:u|=sigs[c]
            if u==W: cs.append(set(C))
    if not cs: return 0
    pairs=[(frozenset(C),v) for C in cs for v in E if v not in C]
    # pools = all subsets of E; pool covers (C,v) iff v in pool and pool∩C empty
    pools=[frozenset(x) for r in range(len(E)+1) for x in itertools.combinations(E,r)]
    cover=[frozenset(i for i,(C,v) in enumerate(pairs) if v in p and not (p&C)) for p in pools]
    cover=list(set(cover))
    cover=[c for c in cover if not any(c<d for d in cover)]
    allp=frozenset(range(len(pairs)))
    if not pairs: return 0
    for q in range(1,cap+1):
        for combo in itertools.combinations(cover,q):
            u=frozenset().union(*combo)
            if u==allp: return q
    return cap+1
random.seed(1); bad=0; tot=0
for t in range(60):
    k=random.choice([2,3]); n=random.randint(3,6)
    sigs=[random.randrange(1<<k) for _ in range(n)]
    out=subprocess.run(["./gt","eval",str(k),"0"]+list(map(str,sigs)),capture_output=True,text=True).stdout.split("\n")
    for W in range(1<<k):
        line=[l for l in out if l.startswith("W=%d "%W)][0]
        r=int(line.split("r=")[1]); b=rW_bf(sigs,W)
        tot+=1
        if r!=b: bad+=1; print("MISMATCH",sigs,W,r,b)
print("compared",tot,"mismatches",bad)
