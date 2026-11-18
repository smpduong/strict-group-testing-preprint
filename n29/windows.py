# Re-verify the screening arithmetic for n,T: allowed (k, pool size a) from Lemma A (single pool) and Lemma B.
from math import comb
f={0:0,1:1,2:2,3:3,4:4,5:5,6:6,7:9,8:13,9:18}
def windows(n,T):
    out={}
    for k in range(1,T+1):
        q=T-k
        amin=n-f[T-1]
        ok=[a for a in range(max(amin,0),n+1) if comb(n,2)-comb(n-a,2)<=2**(k-1)*comb(q,q//2)]
        out[k]=(q,ok)
    return out
for n,T in [(19,9),(29,10),(18,9)]:
    print(n,T)
    for k,(q,ok) in windows(n,T).items(): print("  k=%d q=%d pool sizes %s"%(k,q,ok))
# union bounds for n=29
f10=f
for s in range(1,10): print("N size",s,"union >=",29-f[10-s])
