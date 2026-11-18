# Empirical check of Lemma 2 (pairs hitting any stage-1 pool <= 2^(k-1)*C(q,q//2)) on ALL designs produced by exhaustive search
import subprocess
from math import comb
def run(n,k,q):
    out=subprocess.run(['./exh5',str(n),str(k),str(q),'0',str(n)],capture_output=True,text=True,timeout=250).stdout
    tot=viol=0
    for l in out.split('\n'):
        if l.startswith('FOUND'):
            sg=list(map(int,l.split(':')[1].split())); tot+=1
            for j in range(k):
                a=sum(1 for s in sg if s>>j&1)
                if comb(n,2)-comb(n-a,2) > 2**(k-1)*comb(q,q//2): viol+=1
    return tot,viol
for cfg in [(9,4,3),(10,4,3),(8,3,3),(9,5,2),(10,5,2),(11,5,3),(12,5,3),(8,6,1),(9,6,1),(10,6,2),(7,3,2),(6,3,1)]:
    print(cfg, run(*cfg), flush=True)
