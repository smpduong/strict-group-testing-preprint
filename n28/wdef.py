from math import comb
f={0:0,1:1,2:2,3:3,4:4,5:5,6:6,7:9,8:13,9:18}
def run(n,T):
    out=[]
    amin=n-f[T-1]
    H=lambda a: comb(n,2)-comb(n-a,2)
    for k in range(1,T):
        q=T-k; B=comb(q,q//2); cap=2**(k-1)*B
        if H(amin)>cap: out.append((k,'B-excl')); continue
        amax=max(a for a in range(n+1) if H(a)<=cap)
        ub=k*(cap-H(amin))              # max sum of slacks
        need=(2**k-1)*B-comb(n,2)       # sum_{W!=0} d >= need
        lb=0; rem=need
        for w in range(1,k+1):
            c=comb(k,w); take=min(rem,c*B); lb+=take*w; rem-=take
            if rem<=0: break
        out.append((k,'W-excl' if lb>ub else 'OPEN',f'a<={amax} lb={lb} ub={ub}'))
    return out
for n,T in [(18,9),(19,9),(24,10),(25,10),(26,10),(27,10),(28,10),(29,10)]:
    print(n,T,[o for o in run(n,T) if o[1]!='B-excl'])
