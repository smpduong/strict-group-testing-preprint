// Dedicated q=0 (nonadaptive) solver: n items, k tests; condition (Cor.5 with r(W)=0 for all W) is equivalent to
// "2-cover-free family": for all items v and all C subset of others with |C|<=2: sig_v not subset of union sig_C
// (this includes C=empty: sig_v nonempty).  usage: ./cf n k  (env FIRST=1)
// Symmetry: items nondecreasing in signature value; singleton-signature counts nonincreasing in pool index.
#include <stdio.h>
#include <stdlib.h>
static int n_,k_,S_; static int it[32]; static int cntsig[1024]; static long nodes=0, found=0;
static int okadd(int idx,int t){
  if(t==0) return 0;
  for(int y=0;y<idx;y++){
    if((t&~it[y])==0) return 0;            // t subset of y
    if((it[y]&~t)==0) return 0;            // y subset of t (C={t}, v=y)
    for(int z=y+1;z<idx;z++){
      if((t&~(it[y]|it[z]))==0) return 0;  // t covered by y,z
      if((it[y]&~(t|it[z]))==0) return 0;  // y covered by t,z
      if((it[z]&~(t|it[y]))==0) return 0;  // z covered by t,y
    }
  }
  return 1;
}
static void rec(int idx,int minsig){
  nodes++;
  if(idx==n_){ found++; if(getenv("FIRST")){printf("FOUND:"); for(int i=0;i<n_;i++)printf(" %d",it[i]); printf("\nDONE(first) nodes=%ld\nSTATUS: COMPLETE (design found; no node limit in this solver)\n",nodes); exit(0);} return; }
  for(int t=minsig;t<S_;t++){
    if((t&(t-1))==0 && t>1){ if(cntsig[t]+1>cntsig[t>>1]) continue; }
    if(!okadd(idx,t)) continue;
    it[idx]=t; cntsig[t]++;
    rec(idx+1,t+1);                        // antichain => distinct signatures, strictly increasing
    cntsig[t]--;
  }
}
int main(int argc,char**argv){ n_=atoi(argv[1]); k_=atoi(argv[2]); S_=1<<k_; rec(0,1);
  printf("DONE n=%d k=%d q=0 nodes=%ld designs_found=%ld\nARTEFACT: designs_found=0 is a NEGATIVE-SEARCH LOG (not an independently checkable certificate)\nSTATUS: COMPLETE (exhaustive; no node limit in this solver)\n",n_,k_,nodes,found); return 0; }
