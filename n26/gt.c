// Strict 2-stage group testing: exact evaluation of stage-1 designs + simulated annealing.
// Usage:
//  ./gt eval k q sig0 sig1 ...      (print r(W) for all W, max r)
//  ./gt sa n k q minp maxp seed iters T0 T1 [certfile]
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#define MAXN 28   /* n26 wide build: was 24 in search/gt.c; all arrays indexed by m<=MAXN are sized MAXN+1 (see tests/test_boundary.c) */
#define MAXC 700
#define MAXCONS 20000
static int E[MAXN], m;
static int csa[MAXC], csb[MAXC], ncs;
static int cstart[MAXN+2], ca[MAXCONS], cb[MAXCONS], cv[MAXCONS], ncons;
static int code[MAXN+1];   /* code[m] (the empty-set slot) is written with m up to MAXN: size MAXN+1 (was MAXN: off-by-one) */
static long nodes, nodelimit;
/* Node-limit handling: reaching the limit means UNKNOWN, never "infeasible".
   gt_call_unknown: current feasible() call hit the limit (aborts the recursion).
   gt_unknown_calls: sticky count over the whole run; any nonzero value makes the run INCOMPLETE. */
static int gt_call_unknown=0; static long gt_unknown_calls=0;
#define GT_DEFAULT_NODELIMIT 2000000000L
#include <errno.h>
static long gt_nodelimit_cfg(void){ /* env GT_NODELIMIT overrides (testing only) */
  static long v=-1; if(v<0){ const char*e=getenv("GT_NODELIMIT"); v=(e&&*e)?atol(e):GT_DEFAULT_NODELIMIT; if(v<0)v=GT_DEFAULT_NODELIMIT; } return v; }
/* Prints the completion flag; returns the process exit code (0 complete, 3 incomplete). Call at the end of every run. */
static int gt_report_status(void){
  printf("ARTEFACT: search log. 'designs_found=0' is a NEGATIVE-SEARCH LOG (not an independently checkable certificate); a FOUND line is a candidate POSITIVE WITNESS (dump it with `gt cert` and check it with search/verify.py)\n");
  if(gt_unknown_calls){
    printf("STATUS: INCOMPLETE (node limit %ld reached in %ld CSP call(s); result is UNKNOWN: it must NOT be used as an impossibility claim, and any designs_found/FOUND lines are UNVERIFIED)\n",gt_nodelimit_cfg(),gt_unknown_calls);
    fflush(stdout); return 3; }
  printf("STATUS: COMPLETE (no node limit reached; node limit %ld)\n",gt_nodelimit_cfg()); fflush(stdout); return 0; }
static void gt_check_k(int k){ if(k<1||k>9){ fprintf(stderr,"k=%d outside supported range 1..9 (exh*.c tables are sized 512)\n",k); exit(2);} }
static void gt_check_n(int n){ if(n<1||n>MAXN){ fprintf(stderr,"n=%d outside supported range 1..%d (MAXN)\n",n,MAXN); exit(2);} }


#ifdef WIDE
typedef unsigned __int128 u32;
#define POPC(x) (__builtin_popcountll((uint64_t)(x))+__builtin_popcountll((uint64_t)((x)>>64)))
#define ONE ((u32)1)
#else
typedef uint32_t u32;
#define POPC(x) __builtin_popcount(x)
#define ONE 1u
#endif
static int assigned_[MAXN+1];
static int nco[MAXN+1], colist[MAXN+1][MAXN*MAXN+MAXC];
static int full; static int member[MAXN+1];
// propagate constraint c; returns 0 on failure
static inline int prop(int c,u32*dom){
  int a=ca[c]<0?m:ca[c], b=cb[c]<0?m:cb[c], v=cv[c];
  int ua=!assigned_[a], ub=!assigned_[b], uv=!assigned_[v];
  int u=ua+ub+uv;
  if(u>1){ return 1; }
  if(u==0){ int U=code[a]|code[b]; return (code[v]&~U)!=0; }
  if(uv){ int U=code[a]|code[b]; u32 d=dom[v]; u32 nd=d;
    for(int w=0;w<=full;w++) if(((d>>w)&1) && (w&~U)==0) nd&=~(ONE<<w);
    dom[v]=nd; return nd!=0; }
  int x=ua?a:b, o=ua?b:a; u32 d=dom[x],nd=d;
  for(int w=0;w<=full;w++) if(((d>>w)&1) && (code[v]&~(w|code[o]))==0) nd&=~(ONE<<w);
  dom[x]=nd; return nd!=0;
}
static int bt2(int nass,int used,u32*dom){
  if(nass==m) return 1;
  if(++nodes>nodelimit){ gt_call_unknown=1; return 0; }
  int x=-1,best=1000;
  for(int i=0;i<m;i++) if(!assigned_[i]){ int pc=POPC(dom[i]); if(pc<best){best=pc;x=i;} }
  if(best==0) return 0;
  u32 nd[MAXN+1];
  for(int val=0;val<=full;val++){
    if(!((dom[x]>>val)&1)) continue;
    {int h=val>>used; if(h&(h+1)) continue;}
    memcpy(nd,dom,sizeof(u32)*(m+1));
    code[x]=val; assigned_[x]=1;
    int ok=1;
    for(int t=0;t<nco[x]&&ok;t++) ok=prop(colist[x][t],nd);
    if(ok){ int nu=used; while((1<<nu)<=val) nu++;
      if(bt2(nass+1,nu,nd)) return 1;
      if(gt_call_unknown) return 0; }
    assigned_[x]=0;
  }
  return 0;
}

static int btold(int pos,int used,int q){
  if(pos==m) return 1;
  if(++nodes>nodelimit){ gt_call_unknown=1; return 0; }
  int lim=1<<q;
  for(int val=0;val<lim;val++){
    {int h=val>>used; if(h&(h+1)) continue;}
    code[pos]=val; int ok=1;
    for(int c=cstart[pos];c<cstart[pos+1];c++){
      int U=(ca[c]>=0?code[ca[c]]:0)|(cb[c]>=0?code[cb[c]]:0);
      if((code[cv[c]]&~U)==0){ok=0;break;}
    }
    if(!ok) continue;
    int nu=used; while((1<<nu)<=val) nu++;
    if(btold(pos+1,nu,q)) return 1;
    if(gt_call_unknown) return 0;
  }
  return 0;
}
static int bt(int pos,int used,int q){
  #ifndef WIDE
  if(q>5) return btold(0,0,q);
#else
  if(q>7) return btold(0,0,q);
#endif
  full=(1<<q)-1;
  u32 dom[MAXN+1]; for(int i=0;i<=m;i++){dom[i]=(full+1>=(int)(8*sizeof(u32)))?~(u32)0:((ONE<<(full+1))-1);assigned_[i]=0;}
  assigned_[m]=1; code[m]=0; int na=0;
  for(int i=0;i<m;i++) if(!member[i]){ code[i]=full; assigned_[i]=1; na++; }
  for(int c=0;c<ncons;c++) if(!prop(c,dom)) return 0;
  return bt2(na,0,dom);
}
// build structure for W; returns ncs
static int build(const int*sig,int n,int W){
  gt_check_n(n);
  m=0; for(int i=0;i<n;i++) if((sig[i]&~W)==0) E[m++]=i;
  ncs=0;
  if(W==0){csa[ncs]=-1;csb[ncs]=-1;ncs++;}
  for(int a=0;a<m;a++) if(sig[E[a]]==W){csa[ncs]=a;csb[ncs]=-1;ncs++;}
  for(int a=0;a<m;a++) for(int b=a+1;b<m;b++) if((sig[E[a]]|sig[E[b]])==W){csa[ncs]=a;csb[ncs]=b;ncs++;}
  if(ncs>MAXC){ fprintf(stderr,"ncs=%d exceeds MAXC=%d\n",ncs,MAXC); exit(2); }
  if(ncs==0) return 0;
  for(int i=0;i<=m;i++) member[i]=0;
  for(int s2=0;s2<ncs;s2++){ if(csa[s2]>=0)member[csa[s2]]=1; if(csb[s2]>=0)member[csb[s2]]=1; }
  // constraints bucketed by completion position
  int cnt[MAXN+2]; memset(cnt,0,sizeof cnt);
  for(int s=0;s<ncs;s++) for(int v=0;v<m;v++){ if(v==csa[s]||v==csb[s]) continue;
    int p=v; if(csa[s]>p)p=csa[s]; if(csb[s]>p)p=csb[s]; cnt[p]++; }
  cstart[0]=0; for(int p=0;p<m;p++) cstart[p+1]=cstart[p]+cnt[p];
  int fill[MAXN+2]; for(int p=0;p<=m;p++) fill[p]=cstart[p];
  for(int s=0;s<ncs;s++) for(int v=0;v<m;v++){ if(v==csa[s]||v==csb[s]) continue;
    int p=v; if(csa[s]>p)p=csa[s]; if(csb[s]>p)p=csb[s]; int c=fill[p]++; ca[c]=csa[s];cb[c]=csb[s];cv[c]=v; }
  ncons=cstart[m];
  if(ncons>MAXCONS){ fprintf(stderr,"ncons=%d exceeds MAXCONS=%d\n",ncons,MAXCONS); exit(2); }
  for(int i=0;i<=m;i++)nco[i]=0;
  for(int c=0;c<ncons;c++){int vs[3]={ca[c],cb[c],cv[c]};for(int t=0;t<3;t++) if(vs[t]>=0){ colist[vs[t]][nco[vs[t]]++]=c; }}
  return ncs;
}
// is r(W)<=q ?  Returns 1 (feasible), 0 (proved infeasible) or -1 (UNKNOWN: node limit hit).
// Callers: `!feasible(q)` is true ONLY for a proved 0 as long as -1 is tested separately; the exh*.c callers
// use `feasible(q)==0` to prune, so UNKNOWN never prunes.  The run is flagged INCOMPLETE via gt_unknown_calls.
static int feasible(int q){ nodes=0;
  if(ncs==0) return 1;
  if(ncs>(1<<q)) return 0;
  nodes=0; nodelimit=gt_nodelimit_cfg(); gt_call_unknown=0;
  int r=bt(0,0,q);
  if(r) return 1;              /* a solution was found: genuinely feasible even if the limit was also touched */
  if(gt_call_unknown){ gt_unknown_calls++; return -1; }
  return 0;
}
// exact r(W) up to cap (returns cap+1 if > cap)
static int rW(const int*sig,int n,int W,int cap){
  if(!build(sig,n,W)) return 0;
  for(int q=0;q<=cap;q++){ int f=feasible(q); if(f<0) return -1; if(f) return q; }
  return cap+1;
}
// cost: number of infeasible W at q (+ size penalties handled by caller)
static int badcount(const int*sig,int n,int k,int q,int*maxr){
  int bad=0;
  for(int W=0;W<(1<<k);W++){
    if(!build(sig,n,W)) continue;
    int f=feasible(q); if(getenv("PROF")) fprintf(stderr,"W=%d m=%d ncs=%d feas=%d nodes=%ld\n",W,m,ncs,f,nodes); if(f<0) bad+=1000; else if(!f) bad++;  /* UNKNOWN counts as bad (SA heuristic only; run is flagged INCOMPLETE) */
  }
  return bad;
}
static void dump_cert(const char*fn,const int*sig,int n,int k,int q){
  /* Writes to fn.tmp and renames only when EVERY outcome W has a stage-2 solution within the budget q.
     Any failure (needs > q tests, or solver UNKNOWN/limit hit) removes fn and fn.tmp and exits nonzero. */
  char tmp[600]; snprintf(tmp,sizeof tmp,"%s.tmp",fn);
  remove(fn);
  FILE*f=fopen(tmp,"w");
  if(!f){ fprintf(stderr,"dump_cert: cannot open %s\n",tmp); exit(4); }
  fprintf(f,"{\"artefact\":\"positive-strategy-witness\",\"n\":%d,\"k\":%d,\"sigs\":[",n,k);
  for(int i=0;i<n;i++){fprintf(f,"%s[",i?",":"");int first=1;for(int j=0;j<k;j++) if(sig[i]>>j&1){fprintf(f,"%s%d",first?"":",",j);first=0;}fprintf(f,"]");}
  fprintf(f,"],\"stage2\":{");
  for(int W=0;W<(1<<k);W++){
    fprintf(f,"%s\"%d\":[",W?",":"",W);
    if(build(sig,n,W)){
      int qq=-1, fe=0;
      for(int t=0;t<=q;t++){ fe=feasible(t); if(fe<0) break; if(fe){ qq=t; break; } }
      if(fe<0){ fprintf(stderr,"dump_cert: node limit hit (UNKNOWN) at W=%d; no certificate written\n",W); fclose(f); remove(tmp); remove(fn); exit(3); }
      if(qq<0){ fprintf(stderr,"dump_cert: outcome W=%d needs more than the budget of %d stage-2 tests; no certificate written\n",W,q); fclose(f); remove(tmp); remove(fn); exit(4); }
      /* re-solve to fill code */
      nodes=0; gt_call_unknown=0;
      if(ncs>0 && !bt(0,0,qq)){ fprintf(stderr,"dump_cert: re-solve failed at W=%d (limit/UNKNOWN); no certificate written\n",W); fclose(f); remove(tmp); remove(fn); exit(3); }
      for(int j=0;j<qq;j++){ fprintf(f,"%s[",j?",":""); int first=1;
        for(int a=0;a<m;a++) if(code[a]>>j&1){fprintf(f,"%s%d",first?"":",",E[a]);first=0;}
        fprintf(f,"]"); }
    }
    fprintf(f,"]");
  }
  fprintf(f,"}}\n");
  if(fclose(f)||rename(tmp,fn)){ fprintf(stderr,"dump_cert: write failed\n"); remove(tmp); remove(fn); exit(4); }
}
static int FTAB[10]={0,1,2,3,4,5,6,9,13,1000}; // f(T)=max n with t(n,2,2)<=T (computed exhaustively here for T<=8)
static int cntpen(const int*sig,int n,int k,int q){
  int pen=0;
  for(int W=0;W<(1<<k);W++){ int mW=0; for(int i=0;i<n;i++) if((sig[i]&~W)==0) mW++;
    int T=q+__builtin_popcount(W); int g=T>9?1000:FTAB[T]; if(mW>g) pen+=mW-g; }
  return pen;
}
static uint64_t rs=88172645463325252ULL;
static uint64_t rnd(){rs^=rs<<13;rs^=rs>>7;rs^=rs<<17;return rs;}
static double urand(){return (rnd()>>11)*(1.0/9007199254740992.0);}

int main(int argc,char**argv){
  if(argc<2) return 1;
  if(!strcmp(argv[1],"eval")){
    int k=atoi(argv[2]),q=atoi(argv[3]); int n=argc-4; int sig[MAXN]; gt_check_n(n);
    for(int i=0;i<n;i++) sig[i]=atoi(argv[4+i]);
    int mx=0;
    gt_check_n(n);
    for(int W=0;W<(1<<k);W++){ int r=rW(sig,n,W,10); build(sig,n,W);
      if(r<0) printf("W=%d cand_elems=%d cand_sets=%d r=UNKNOWN\n",W,m,ncs);
      else printf("W=%d cand_elems=%d cand_sets=%d r=%d\n",W,m,ncs,r); if(r>mx)mx=r;}
    printf("max_r=%d total_cost=%d\n",mx,k+mx);
    return gt_report_status();
  }
  if(!strcmp(argv[1],"cert")){
    int k=atoi(argv[2]),q=atoi(argv[3]); int n=argc-5; int sig[MAXN]; gt_check_n(n);
    for(int i=0;i<n;i++) sig[i]=atoi(argv[5+i]);
    gt_check_n(n); dump_cert(argv[4],sig,n,k,q); return gt_report_status();
  }
  if(!strcmp(argv[1],"sa")){
    int n=atoi(argv[2]),k=atoi(argv[3]),q=atoi(argv[4]),minp=atoi(argv[5]),maxp=atoi(argv[6]);
    rs^=(uint64_t)atoll(argv[7])*0x9E3779B97F4A7C15ULL; for(int i=0;i<10;i++)rnd();
    long iters=atol(argv[8]); double T0=atof(argv[9]),T1=atof(argv[10]);
    const char*cert=argc>11?argv[11]:NULL;
    int S=1<<k; int sig[MAXN]; gt_check_n(n);
    for(int i=0;i<n;i++) sig[i]=rnd()%S;
    #define SIZEPEN(sg) ({int pen=0;for(int j=0;j<k;j++){int c=0;for(int i=0;i<n;i++)c+=(sg[i]>>j)&1; if(c<minp)pen+=minp-c; if(c>maxp)pen+=c-maxp;} pen;})
    double cur=badcount(sig,n,k,q,NULL)*1.0+2.0*SIZEPEN(sig)+cntpen(sig,n,k,q); double best=cur;
    long found=0;
    for(long it=0;it<iters;it++){
      double T=T0*pow(T1/T0,(double)it/iters);
      int i=rnd()%n, old=sig[i], nw;
      int r=rnd()%3;
      if(r==0) nw=rnd()%S; else if(r==1) nw=old^(1<<(rnd()%k)); else {int j=rnd()%n; nw=sig[j];}
      if(nw==old) continue;
      sig[i]=nw;
      int sp=SIZEPEN(sig);
      double c=cntpen(sig,n,k,q)+2.0*sp; if(c<=cur+3*T+5) c+=badcount(sig,n,k,q,NULL); else c+=100;
      if(c<=cur || urand()<exp((cur-c)/T)){ cur=c; if(c<best){best=c;
          fprintf(stderr,"it=%ld best=%g\n",it,best);}
        if(c==0){ found++;
          printf("FOUND n=%d k=%d q=%d sigs:",n,k,q); for(int x=0;x<n;x++)printf(" %d",sig[x]); printf("\n"); fflush(stdout);
          if(cert) dump_cert(cert,sig,n,k,q);
          return gt_report_status(); }
      } else sig[i]=old;
    }
    printf("NOTFOUND n=%d k=%d q=%d best=%g iters=%ld (heuristic search: NOTFOUND is never an impossibility claim)\n",n,k,q,best,iters);
    return gt_report_status();
  }
  return 1;
}
