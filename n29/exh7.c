// Exhaustive search, item-by-item (signatures nondecreasing). usage: ./exh2 n k q minp maxp [certprefix]
// env FIRST=1 : stop at first design found.  Prune: candidate-set count per W <= 2^q, then exact CSP r(W)<=q
// for every W containing the new item's signature (monotone: adding items never decreases r(W)).
// Symmetry used: counts of singleton-signature items nonincreasing in pool index (pool permutation WLOG).
#ifndef FT9
#define FT9 1000000 /* f(9); set -DFT9=18 only for T=10 runs */
#endif
#define main gt_main
#include "gt.c"
#undef main
static int n_,k_,q_,minp_,maxp_,S_;
static int items[MAXN], ccnt[512];  // ccnt[W]=number of candidate sets (partial design)
static int poolc[16], cntsig[512]; static int mW[512], pcnt[512]; static int FT[12]={0,1,2,3,4,5,6,9,13,FT9,1000000,1000000}; static int SPER; static int SPLITD=2; static int inter[16][16]; static int INTERCAP=1000;
static long nodes_=0, found_=0, csp_=0;
static const char*certp; static int P_=1,p_=0; static long ctr2=0;
static void rec(int idx,int minsig){
  nodes_++;
  if(idx==SPLITD){ long c=ctr2++; if((c%P_)!=p_) return; if(getenv("PROGRESS")) fprintf(stderr,"d2 #%ld items %d %d nodes=%ld\n",c,items[0],items[1],nodes_); }
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
    if(INTERCAP<1000){ int bad=0; for(int i=0;i<k_&&!bad;i++) if(t>>i&1) for(int j=i+1;j<k_;j++) if((t>>j&1) && inter[i][j]+1>INTERCAP){bad=1;break;} if(bad) continue; }
    if((t>>(k_-1))&1) { if(poolc[k_-1]==0 && idx < n_-maxp_) continue; }   // top pool = items placed last; final size = n-|A| <= maxp
    int x=idx; items[x]=t;
    int touched[MAXN+1], nt=0; int cap=1<<q_;
    int Ws[MAXN+1]; Ws[0]=t; for(int y=0;y<idx;y++) Ws[y+1]=t|items[y];
    for(int y=0;y<=idx;y++){ ccnt[Ws[y]]++; touched[nt++]=Ws[y]; if(ccnt[Ws[y]]>cap) ok=0; if(y>=1){ pcnt[Ws[y]]++; if(pcnt[Ws[y]]>SPER) ok=0; } }
    for(int W=t;W<S_;W++) if((W&t)==t){ mW[W]++; int T=q_+__builtin_popcount(W); if(T>11)T=11; if(mW[W]>FT[T]) ok=0; }
    if(ok){
      for(int W=t;W<S_&&ok;W++) if((W&t)==t){
        if(build(items,idx+1,W)){ csp_++; if(!feasible(q_)) ok=0; }
      }
    }
    if(ok){
      for(int j=0;j<k_;j++) poolc[j]+=(t>>j&1);
      for(int i=0;i<k_;i++) if(t>>i&1) for(int j=i+1;j<k_;j++) if(t>>j&1) inter[i][j]++;
      cntsig[t]++;
      rec(idx+1,t);
      for(int j=0;j<k_;j++) poolc[j]-=(t>>j&1);
      for(int i=0;i<k_;i++) if(t>>i&1) for(int j=i+1;j<k_;j++) if(t>>j&1) inter[i][j]--;
      cntsig[t]--;
    }
    for(int W=t;W<S_;W++) if((W&t)==t) mW[W]--;
    for(int y=0;y<nt;y++){ ccnt[touched[y]]--; }
    for(int y=1;y<nt;y++) pcnt[touched[y]]--;
  }
}
int main(int argc,char**argv){
  n_=atoi(argv[1]);gt_check_n(n_);k_=atoi(argv[2]);gt_check_k(k_);q_=atoi(argv[3]);minp_=atoi(argv[4]);maxp_=atoi(argv[5]);
  certp=argc>6?argv[6]:NULL; {int qq=q_,c=1; /* Sperner: C(q,floor(q/2)) */ long a=1; for(int i=1;i<=qq/2;i++) a=a*(qq-i+1)/i; c=(int)a; SPER=c;} if(argc>8){P_=atoi(argv[7]);p_=atoi(argv[8]);} S_=1<<k_; if(getenv("SPLITD")) SPLITD=atoi(getenv("SPLITD")); if(getenv("INTERCAP")) INTERCAP=atoi(getenv("INTERCAP"));
  if(getenv("FTMAX")){ for(int i=atoi(getenv("FTMAX"));i<12;i++) FT[i]=1000000; } /* FTMAX=T disables the counting prune at the run's own total T (it would be circular there) */
  ccnt[0]=1; // the empty candidate set belongs to W=0
  rec(0,0);
  printf("DONE n=%d k=%d q=%d pools[%d,%d] nodes=%ld cspcalls=%ld designs_found=%ld\n",n_,k_,q_,minp_,maxp_,nodes_,csp_,found_);
  return gt_report_status();
}
