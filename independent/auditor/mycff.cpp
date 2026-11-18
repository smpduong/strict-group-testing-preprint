// Own 2-cover-free family search on t points: max number of nonempty subsets of [t] such that no member is contained in the union of two OTHER members
// (hence also no member contained in another). usage: mycff t N  -> exhaustive: does a family of N sets exist?
// Symmetry: members sorted by (size, mask); smallest member WLOG = {0..s-1}. (Weaker than repo's; independent.)
#include <bits/stdc++.h>
using namespace std;
static int t_,N_; static int fam[20]; static long nodes=0; static int sz[1<<10];
static vector<int> cand; // all masks sorted by (size,mask)
static bool okadd(int idx,int m){
  for(int y=0;y<idx;y++){
    int Y=fam[y];
    if((m&~Y)==0) return false;          // m subset of y
    if((Y&~m)==0) return false;          // y subset of m
    for(int z=y+1;z<idx;z++){
      int Z=fam[z];
      if((m&~(Y|Z))==0) return false;
      if((Y&~(m|Z))==0) return false;
      if((Z&~(m|Y))==0) return false;
    }
  }
  return true;
}
static bool rec(int idx,int from){
  nodes++;
  if(idx==N_) return true;
  // need N-idx more sets from cand[from..]
  for(int ci=from;ci<(int)cand.size();ci++){
    int m=cand[ci];
    if(idx==0){ int s=sz[m]; if(m!=((1<<s)-1)) continue; }
    if(!okadd(idx,m)) continue;
    fam[idx]=m;
    if(rec(idx+1,ci+1)) return true;
  }
  return false;
}
int main(int argc,char**argv){ t_=atoi(argv[1]); N_=atoi(argv[2]);
  for(int m=1;m<(1<<t_);m++){ sz[m]=__builtin_popcount(m); cand.push_back(m);} 
  sort(cand.begin(),cand.end(),[&](int a,int b){ return sz[a]!=sz[b]?sz[a]<sz[b]:a<b;});
  bool r=rec(0,0);
  printf("t=%d N=%d exists=%d nodes=%ld\n",t_,N_,(int)r,nodes);
  if(r){ for(int i=0;i<N_;i++) printf(" %d",fam[i]); printf("\n"); }
}
