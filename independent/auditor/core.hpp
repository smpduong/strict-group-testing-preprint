// Audit code, written from scratch. Independent of repo code.
#include <bits/stdc++.h>
using namespace std;
typedef unsigned long long u64;
struct Struct { // outcome-W structure
  vector<int> E;            // item indices with sig subset of W
  vector<int> cs;           // candidate sets as bitmasks over LOCAL indices (0..m-1)
  int m;
};
static Struct buildW(const vector<int>& sig, int W){
  Struct s; for(int i=0;i<(int)sig.size();i++) if((sig[i]&~W)==0) s.E.push_back(i);
  s.m=s.E.size(); int m=s.m;
  if(W==0) s.cs.push_back(0);
  for(int a=0;a<m;a++) if(sig[s.E[a]]==W) s.cs.push_back(1<<a);
  for(int a=0;a<m;a++) for(int b=a+1;b<m;b++) if((sig[s.E[a]]|sig[s.E[b]])==W) s.cs.push_back((1<<a)|(1<<b));
  return s;
}
// Semantic brute force: min q such that q pools (subsets of E) make transcripts separate every
// set of size<=2 in class W from every other set (any size) with union of signatures == W.
// Returns q, or cap+1 if > cap. m must be <= ~8.
static int semanticQ(const vector<int>& sig,int W,int cap){
  Struct s=buildW(sig,W); int m=s.m; if(s.cs.empty()) return 0;
  vector<int> cls; // all subsets P of E whose union = W
  for(int P=0;P<(1<<m);P++){ int u=0; for(int a=0;a<m;a++) if(P>>a&1) u|=sig[s.E[a]]; if(u==W) cls.push_back(P);}
  auto ok=[&](const vector<int>& pools){
    unordered_map<int,pair<int,int>> mp; // out -> (count, small count)
    for(int P:cls){ int out=0; for(size_t j=0;j<pools.size();j++) if(P&pools[j]) out|=1<<j;
      auto &e=mp[out]; e.first++; if(__builtin_popcount(P)<=2) e.second++; }
    for(auto&kv:mp) if(kv.second.second>0 && kv.second.first>1) return false;
    return true; };
  for(int q=0;q<=cap;q++){
    vector<int> pools(q,0);
    // enumerate nondecreasing tuples
    function<bool(int,int)> rec=[&](int idx,int lo)->bool{
      if(idx==q) return ok(pools);
      for(int p=lo;p<(1<<m);p++){ pools[idx]=p; if(rec(idx+1,p)) return true; }
      return false; };
    if(rec(0,0)) return q;
  }
  return cap+1;
}
// Closed-pool set cover solver (different from repo): returns min q <=cap, fills pools (local bitmasks) or cap+1
static int coverQ(const Struct& s,int cap,vector<int>* outPools=nullptr){
  int m=s.m, nc=s.cs.size(); if(nc==0) return 0; if(nc>(1<<cap)) return cap+1; /* distinct cand sets need distinct transcripts */
  // pairs (c,v) v notin C, index c*m+v
  int NP=nc*m; int words=(NP+63)/64;
  // candidate pools: for every subset of candidate sets, A=union, pool=E\A (maximal for that choice)
  set<int> poolset;
  if(nc<=20){ for(int sub=0;sub<(1<<nc);sub++){ int A=0; for(int c=0;c<nc;c++) if(sub>>c&1) A|=s.cs[c]; poolset.insert(((1<<m)-1)&~A);} }
  else { for(int c=0;c<nc;c++) poolset.insert(((1<<m)-1)&~s.cs[c]); }
  vector<int> pools(poolset.begin(),poolset.end());
  vector<vector<u64>> cov(pools.size(), vector<u64>(words,0));
  vector<u64> need(words,0);
  for(int c=0;c<nc;c++) for(int v=0;v<m;v++) if(!(s.cs[c]>>v&1)){ int id=c*m+v; need[id/64]|=1ULL<<(id%64);
    for(size_t p=0;p<pools.size();p++) if((pools[p]>>v&1) && !(pools[p]&s.cs[c])) cov[p][id/64]|=1ULL<<(id%64); }
  bool any=false; for(auto w:need) if(w) any=true; if(!any) return 0;
  vector<int> chosen;
  function<bool(vector<u64>&,int)> rec=[&](vector<u64>& cur,int left)->bool{
    int first=-1; for(int w=0;w<words;w++){ u64 r=need[w]&~cur[w]; if(r){ first=w*64+__builtin_ctzll(r); break; } }
    if(first<0) return true; if(left==0) return false;
    for(size_t p=0;p<pools.size();p++) if(cov[p][first/64]>>(first%64)&1){
      vector<u64> nxt=cur; for(int w=0;w<words;w++) nxt[w]|=cov[p][w];
      chosen.push_back(pools[p]); if(rec(nxt,left-1)) return true; chosen.pop_back(); }
    return false; };
  for(int q=1;q<=cap;q++){ chosen.clear(); vector<u64> z(words,0); if(rec(z,q)){ if(outPools)*outPools=chosen; return q; } }
  return cap+1;
}
