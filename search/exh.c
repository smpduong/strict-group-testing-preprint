// Exhaustive search over signature-count vectors for n items, k pools, stage-2 budget q.
// Signatures processed in numeric order; W=s is checked as soon as all counts of subsets of s are fixed.
// Symmetry: singleton-signature counts are nonincreasing in pool index (pool permutation WLOG).
// usage: ./exh n k q minp maxp [certprefix]
#define main gt_main
#include "gt.c"
#undef main
static int n_,k_,q_,minp_,maxp_,S_;
static int cnt[512]; static int items[MAXN]; static int ni;
static long nodes_=0, leaves_=0, found_=0, wchecks=0;
static int poolc[16];
static const char*certp;
static void rec(int s,int rem){
  nodes_++;
  if(s==S_){ if(rem) return;
    for(int j=0;j<k_;j++) if(poolc[j]<minp_||poolc[j]>maxp_) return;
    leaves_++; found_++;
    printf("FOUND sigs:"); for(int i=0;i<ni;i++)printf(" %d",items[i]); printf("\n"); fflush(stdout);
    if(certp && found_==1){ char fn[256]; snprintf(fn,256,"%s_%ld.json",certp,found_); dump_cert(fn,items,ni,k_,q_);}
    if(getenv("FIRST")){printf("DONE(first) nodes=%ld\n",nodes_);exit(gt_report_status());}
    return; }
  int lo=0,hi=rem;
  if(s==S_-1) lo=rem;
  if((s&(s-1))==0 && s>1){ int prev=s>>1; if(cnt[prev]<hi) hi=cnt[prev]; if(lo>hi) return; }
  for(int c=lo;c<=hi;c++){
    cnt[s]=c;
    for(int t=0;t<c;t++) items[ni+t]=s;
    for(int j=0;j<k_;j++) if(s>>j&1) poolc[j]+=c;
    int ok=1;
    for(int j=0;j<k_&&ok;j++){ if(poolc[j]>maxp_) ok=0; /* later sigs may add */ }
    // max possible final pool size = poolc + (rem-c) ; need >= minp
    if(ok) for(int j=0;j<k_;j++) if(poolc[j]+(rem-c)<minp_){ok=0;break;}
    if(ok){
      int saved=ni; ni+=c; wchecks++;
      int good=1;
      if(c>0) for(int Wp=s;Wp<S_&&good;Wp++) if((Wp&s)==s){ if(build(items,ni,Wp) && !feasible(q_)) good=0; }
      if(good) rec(s+1,rem-c);
      ni=saved;
    }
    for(int j=0;j<k_;j++) if(s>>j&1) poolc[j]-=c;
  }
}
int main(int argc,char**argv){
  n_=atoi(argv[1]);gt_check_n(n_);k_=atoi(argv[2]);gt_check_k(k_);q_=atoi(argv[3]);minp_=atoi(argv[4]);maxp_=atoi(argv[5]);
  certp=argc>6?argv[6]:NULL; S_=1<<k_; ni=0;
  rec(0,n_);
  printf("DONE n=%d k=%d q=%d pools[%d,%d] nodes=%ld Wchecks=%ld designs_found=%ld\n",n_,k_,q_,minp_,maxp_,nodes_,wchecks,found_);
  return gt_report_status();
}
