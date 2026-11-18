// Own exhaustive search (written from scratch) for 2-stage strict d=2 designs: n items, k stage-1 pools, q stage-2 pools.
// usage: mysearch n k q minp maxp [intercap] ; env: NOFT=1 (disable f-count prune), NOSP=1 (disable Sperner prune), NOTOP=1, FIRST=1
#include <bits/stdc++.h>
using namespace std;
static int n_,k_,q_,minp_,maxp_,S_,INTERCAP=1000,SPER;
static int sig[32];
static long nodes=0,calls=0,found=0;
static bool useFT=true,useSP=true,useTOP=true;
static int FT[13]={0,1,2,3,4,5,6,9,13,1<<20,1<<20,1<<20,1<<20};
// ---- own CSP over member codes
static int m_, E_[32], ncs_;  static int cs_[1<<10];     // cs_ over local indices
static int memberIdx[32], nmem; static int memLocal[32];
struct Con{int a,b,v;}; // local indices (a,b may be -1), v local (non-negative)
static vector<Con> cons_[32]; // constraints bucketed by last member position (in member order); bucket nmem = only-dummies
static int code_[32]; static int fullc;
static bool csp_rec(int pos,int used){
  if(pos==nmem) return true;
  int x=memLocal[pos];
  for(int val=0;val<=fullc;val++){
    int h=val>>used; if(h&(h+1)) continue;      // first-use ordering of pools
    code_[x]=val; bool ok=true;
    for(const Con&c:cons_[pos]){
      int U=(c.a>=0?code_[c.a]:0)|(c.b>=0?code_[c.b]:0);
      int cv=memberIdx[c.v]>=0?code_[c.v]:fullc;
      if((cv&~U)==0){ok=false;break;}
    }
    if(!ok) continue;
    int nu=used; while((1<<nu)<=val) nu++;
    if(csp_rec(pos+1,nu)) return true;
  }
  return false;
}

// ---- own intersection solver for q<=3: choose <=q sets A_i (unions of candidate sets) so that each candidate C equals the
// intersection of the A_i that contain C (E if none). Equivalent to Cor.5 (pool = E \ A_i).
static int icn, icm, icq; static int icC[1<<10]; static vector<int> icU;
static bool ic_rec(int* cur,int left){
  int c=-1; for(int i=0;i<icn;i++) if(cur[i]!=icC[i]){c=i;break;}
  if(c<0) return true; if(left==0) return false;
  for(int A:icU){ if((A&icC[c])!=icC[c]) continue; if((cur[c]&A)==cur[c]) continue;
    int nxt[1<<10]; for(int i=0;i<icn;i++) nxt[i]=((A&icC[i])==icC[i])?(cur[i]&A):cur[i];
    if(ic_rec(nxt,left-1)) return true; }
  return false;
}
static bool interSolve(int* cs,int nc,int m,int q){
  icn=nc; icm=m; for(int i=0;i<nc;i++) icC[i]=cs[i];
  static int tmp[1<<10]; icU.clear(); int cnt=1; tmp[0]=0;
  for(int i=0;i<nc;i++){ int old=cnt; for(int j=0;j<old;j++) tmp[cnt++]=tmp[j]|cs[i]; }
  sort(tmp,tmp+cnt); icU.assign(tmp,unique(tmp,tmp+cnt));
  int cur[1<<10]; int full=(1<<m)-1; for(int i=0;i<nc;i++) cur[i]=full;
  return ic_rec(cur,q);
}
static bool FORCECSP=false;
// returns 1 iff r(W)<=q ; also pruned by candidate-set count and Sperner
static int feasibleW(int W,int idx){
  m_=0; for(int i=0;i<=idx;i++) if((sig[i]&~W)==0) E_[m_++]=i;
  ncs_=0; int cap=1<<q_; int npair=0;
  static int ca[1<<10],cb[1<<10];
  if(W==0){ ca[ncs_]=-1;cb[ncs_]=-1;ncs_++; }
  for(int a=0;a<m_;a++) if(sig[E_[a]]==W){ if(ncs_>=cap) return 0; ca[ncs_]=a;cb[ncs_]=-1;ncs_++; }
  for(int a=0;a<m_;a++) for(int b=a+1;b<m_;b++) if((sig[E_[a]]|sig[E_[b]])==W){ if(ncs_>=cap) return 0; ca[ncs_]=a;cb[ncs_]=b;ncs_++; npair++; }
  if(useSP && npair>SPER) return 0;
  if(ncs_==0) return 1;
  calls++;
  if(q_<=3 && !FORCECSP){ static int cm[1<<10]; for(int c=0;c<ncs_;c++){ int mk=0; if(ca[c]>=0) mk|=1<<ca[c]; if(cb[c]>=0) mk|=1<<cb[c]; cm[c]=mk; } return interSolve(cm,ncs_,m_,q_)?1:0; }
  for(int i=0;i<m_;i++) memberIdx[i]=-1;
  nmem=0; for(int c=0;c<ncs_;c++){ if(ca[c]>=0&&memberIdx[ca[c]]<0){memberIdx[ca[c]]=1;} if(cb[c]>=0&&memberIdx[cb[c]]<0){memberIdx[cb[c]]=1;} }
  for(int i=0;i<m_;i++) if(memberIdx[i]>=0){ memberIdx[i]=nmem; memLocal[nmem++]=i; }
  for(int p=0;p<=nmem;p++) cons_[p].clear();
  fullc=(1<<q_)-1;
  for(int c=0;c<ncs_;c++) for(int v=0;v<m_;v++){ if(v==ca[c]||v==cb[c]) continue;
    int pos=-1; if(ca[c]>=0) pos=max(pos,memberIdx[ca[c]]); if(cb[c]>=0) pos=max(pos,memberIdx[cb[c]]); if(memberIdx[v]>=0) pos=max(pos,memberIdx[v]);
    if(pos<0){ // empty candidate C: v must be in some pool; v dummy => full ok
      continue; }
    cons_[pos].push_back({ca[c],cb[c],v}); }
  // empty set (W==0): constraint "v in some pool": member v needs code!=0
  if(W==0){ for(int v=0;v<m_;v++) if(memberIdx[v]>=0) cons_[memberIdx[v]].push_back({-1,-1,v}); }
  // order members so that constraint evaluation is early: (members in index order is fine)
  if(q_==0) { for(int c=0;c<ncs_;c++) for(int v=0;v<m_;v++){ if(v==ca[c]||v==cb[c]) continue; return 0; } return 1; }
  return csp_rec(0,0)?1:0;
}
static int poolc[16],cntsig[1<<12],inter[16][16],mW[1<<12];
static void rec(int idx,int minsig){
  nodes++;
  if(idx==n_){ for(int j=0;j<k_;j++) if(poolc[j]<minp_||poolc[j]>maxp_) return; found++;
    printf("FOUND:"); for(int i=0;i<n_;i++) printf(" %d",sig[i]); printf("\n"); fflush(stdout); if(getenv("FIRST")){printf("DONE(first) nodes=%ld\n",nodes);exit(0);} return; }
  int remaining=n_-idx-1;
  for(int t=minsig;t<S_;t++){
    if((t&(t-1))==0 && t>1 && cntsig[t]+1>cntsig[t>>1]) continue;
    bool ok=true;
    for(int j=0;j<k_;j++){ int c=poolc[j]+(t>>j&1); if(c>maxp_||c+remaining<minp_){ok=false;break;} }
    if(!ok) continue;
    if(INTERCAP<1000){ for(int i=0;i<k_&&ok;i++) if(t>>i&1) for(int j=i+1;j<k_;j++) if((t>>j&1)&&inter[i][j]+1>INTERCAP){ok=false;break;} if(!ok) continue; }
    if(useTOP && ((t>>(k_-1))&1) && poolc[k_-1]==0 && idx<n_-maxp_) continue;
    sig[idx]=t;
    if(useFT) for(int W=t;W<S_&&ok;W++) if((W&t)==t){ int T=q_+__builtin_popcount(W); if(T<=8){ int c=0; for(int i=0;i<=idx;i++) if((sig[i]&~W)==0) c++; if(c>FT[T]) ok=false; } }
    for(int W=t;W<S_&&ok;W++) if((W&t)==t) if(!feasibleW(W,idx)) ok=false;
    if(ok){
      for(int j=0;j<k_;j++) poolc[j]+=(t>>j&1);
      for(int i=0;i<k_;i++) if(t>>i&1) for(int j=i+1;j<k_;j++) if(t>>j&1) inter[i][j]++;
      cntsig[t]++;
      rec(idx+1,t);
      cntsig[t]--;
      for(int j=0;j<k_;j++) poolc[j]-=(t>>j&1);
      for(int i=0;i<k_;i++) if(t>>i&1) for(int j=i+1;j<k_;j++) if(t>>j&1) inter[i][j]--;
    }
  }
}
int main(int argc,char**argv){
  n_=atoi(argv[1]);k_=atoi(argv[2]);q_=atoi(argv[3]);minp_=atoi(argv[4]);maxp_=atoi(argv[5]); if(argc>6) INTERCAP=atoi(argv[6]);
  if(getenv("FORCECSP")) FORCECSP=true; if(getenv("NOFT")) useFT=false; if(getenv("NOSP")) useSP=false; if(getenv("NOTOP")) useTOP=false;
  S_=1<<k_; { long a=1; for(int i=1;i<=q_/2;i++) a=a*(q_-i+1)/i; SPER=a; }
  rec(0,0);
  printf("DONE n=%d k=%d q=%d pools[%d,%d] nodes=%ld cspcalls=%ld designs=%ld\n",n_,k_,q_,minp_,maxp_,nodes,calls,found);
}
