// Exhaustive search, item-by-item (signatures nondecreasing). usage: ./exh2 n k q minp maxp [certprefix]
// env FIRST=1 : stop at first design found.  Prune: candidate-set count per W <= 2^q, then exact CSP r(W)<=q
// for every W containing the new item's signature (monotone: adding items never decreases r(W)).
// Symmetry used: counts of singleton-signature items nonincreasing in pool index (pool permutation WLOG).
#define main gt_main
#include "gt.c"
#undef main
static int n_,k_,q_,minp_,maxp_,S_;
static int items[MAXN], ccnt[512];  // ccnt[W]=number of candidate sets (partial design)
static int poolc[16], cntsig[512]; static int mW[512]; static int FT[12]={0,1,2,3,4,5,6,9,13,100000,100000,100000};
static long nodes_=0, found_=0, csp_=0;
static const char*certp;
static void rec(int idx,int minsig){
  nodes_++;
  if(idx==n_){
    for(int j=0;j<k_;j++) if(poolc[j]<minp_||poolc[j]>maxp_) return;
    found_++;
    printf("FOUND sigs:"); for(int i=0;i<n_;i++)printf(" %d",items[i]); printf("\n"); fflush(stdout);
    if(certp && found_==1){ char fn[256]; snprintf(fn,256,"%s_%ld.json",certp,found_); dump_cert(fn,items,n_,k_,q_);}
    if(getenv("FIRST")){printf("DONE(first) nodes=%ld\n",nodes_);exit(gt_report_status());}
    return; }
  int remaining=n_-idx-1;
  for(int t=minsig;t<S_;t++){
    if((t&(t-1))==0 && t>1){ if(cntsig[t]+1>cntsig[t>>1]) continue; }
    int ok=1;
    for(int j=0;j<k_;j++){ int c=poolc[j]+(t>>j&1); if(c>maxp_||c+remaining<minp_){ok=0;break;} }
    if(!ok) continue;
    int x=idx; items[x]=t;
    int touched[MAXN+1], nt=0; int cap=1<<q_;
    int Ws[MAXN+1]; Ws[0]=t; for(int y=0;y<idx;y++) Ws[y+1]=t|items[y];
    for(int y=0;y<=idx;y++){ ccnt[Ws[y]]++; touched[nt++]=Ws[y]; if(ccnt[Ws[y]]>cap) ok=0; }
    for(int W=t;W<S_;W++) if((W&t)==t){ mW[W]++; if(mW[W]>FT[q_+__builtin_popcount(W)>11?11:q_+__builtin_popcount(W)]) ok=0; }
    if(ok && !getenv("NOCSP")){
      for(int W=t;W<S_&&ok;W++) if((W&t)==t){
        if(build(items,idx+1,W)){ csp_++; if(!feasible(q_)) ok=0; }
      }
    }
    if(ok){
      for(int j=0;j<k_;j++) poolc[j]+=(t>>j&1);
      cntsig[t]++;
      rec(idx+1,t);
      for(int j=0;j<k_;j++) poolc[j]-=(t>>j&1);
      cntsig[t]--;
    }
    for(int W=t;W<S_;W++) if((W&t)==t) mW[W]--;
    for(int y=0;y<nt;y++) ccnt[touched[y]]--;
  }
}
int main(int argc,char**argv){
  n_=atoi(argv[1]);gt_check_n(n_);k_=atoi(argv[2]);gt_check_k(k_);q_=atoi(argv[3]);minp_=atoi(argv[4]);maxp_=atoi(argv[5]);
  certp=argc>6?argv[6]:NULL; S_=1<<k_;
  ccnt[0]=1; // the empty candidate set belongs to W=0
  rec(0,0);
  printf("DONE n=%d k=%d q=%d pools[%d,%d] nodes=%ld cspcalls=%ld designs_found=%ld\n",n_,k_,q_,minp_,maxp_,nodes_,csp_,found_);
  return gt_report_status();
}
