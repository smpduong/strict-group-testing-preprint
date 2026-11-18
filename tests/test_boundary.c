/* Boundary test for gt.c at exactly n = MAXN (the largest supported size).
 * Build (from repo root):  gcc -g -O1 -fsanitize=address,undefined -DGT_SRC='"../search/gt.c"' -o test_boundary tests/test_boundary.c -lm
 * Before the fix, bt() wrote code[m] with m == MAXN into an array of size MAXN (one element past the end);
 * AddressSanitizer reports a global-buffer-overflow.  After the fix (code[MAXN+1]) it runs clean. */
#ifndef GT_SRC
#define GT_SRC "../search/gt.c"
#endif
#define main gt_main
#include GT_SRC
#undef main
int main(void){
  int sig[MAXN];
  /* MAXN-1 items with signature {0}, one item with signature {0,1}; outcome W=3: all MAXN items are candidate
     elements (m == MAXN), 24 candidate sets, so feasible(5) enters bt() with m == MAXN. */
  for(int i=0;i<MAXN-1;i++) sig[i]=1;
  sig[MAXN-1]=3;
  if(!build(sig,MAXN,3)){ printf("FAIL: build returned 0\n"); return 1; }
  printf("n=%d m=%d ncs=%d\n",MAXN,m,ncs);
  if(m!=MAXN){ printf("FAIL: expected m==MAXN\n"); return 1; }
  /* ncs = 24 candidate sets forming an antichain of the stage-2 vectors: needs C(q,q/2) >= 24, i.e. q >= 6 (Sperner),
     so q=5 is correctly INFEASIBLE (proved, not a node-limit artefact).  q=5 is the largest q that goes through bt()
     (q>5 uses btold), so this is the call that writes code[m] with m == MAXN. */
  int f=feasible(5);
  printf("feasible(5)=%d (expected 0 = proved infeasible, not -1 UNKNOWN)\n",f);
  if(f!=0) return 1;
  /* n = MAXN+1 must be rejected by gt_check_n (exit code 2) - tested in run_tests.sh */
  printf("BOUNDARY OK\n"); return 0;
}
