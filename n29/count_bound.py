# Necessary condition ("slot deficiency" counting) for a two-stage strict design with k stage-1 pools, q=T-k.
# p(W)=#2-sets of items whose signature-union is W; Lemma B: p(W)<=B=C(q,q//2).
# Pool j with a_j items: sum_{W∋j} p(W) = H(a_j)=C(n,2)-C(n-a_j,2).  Deficiency D_j = 2^(k-1)*B - H(a_j) >= 0.
# Sum_j D_j = sum_W |W|(B-p(W)) >= sum_{W!=0}(B-p(W)) = (2^k-1)B - (C(n,2)-p(0)) >= (2^k-1)B - C(n,2).
from math import comb
f={0:0,1:1,2:2,3:3,4:4,5:5,6:6,7:9,8:13,9:18}
def check(n,T,fT1=18):
    res=[]
    for k in range(1,T):          # k=T (one stage) handled separately (cover-free argument)
        q=T-k; B=comb(q,q//2)
        amin=max(0,n-fT1)          # Lemma A, single pool: n-a <= f(T-1)
        # best case for the inequality: every pool as small as allowed -> largest deficiency
        # but a_j must satisfy Lemma B: H(a_j)<=2^(k-1)B
        a=amin
        H=comb(n,2)-comb(n-a,2); D=2**(k-1)*B-H
        lhs=k*D if D>=0 else None
        rhs=(2**k-1)*B-comb(n,2)
        res.append((k,q,B,a,H,2**(k-1)*B,D,lhs,rhs, (D>=0 and lhs>=rhs)))
    return res
if __name__=="__main__":
    import sys
    for n in range(19,31):
        print("n=%d T=10 (f(9)=18)"%n)
        for r in check(n,10): print("  k=%d q=%d B=%d amin=%d H=%d cap=%d D=%d  sumD<=%s  need>=%d  passes=%s"%r)
