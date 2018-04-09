#include <bits/stdc++.h>
using namespace std;
typedef long long               ll;

//!escape \section{DMST $\mathcal{O}(E \log V)$}

//!begin_codebook
//!start
struct EdgeDesc{
  int from, to, w;
};

struct DMST{
  struct Node;

  struct Edge{
    Node *from;
    Node *tar;
    int w;
    bool inc;
  };

  struct Circle{
    bool vis = false;
    vector<Edge *> contents;
    void clean(int idx);
  };

  const static greater<pair<ll, Edge *> > comp; //Can use inline static since C++17
  static vector<Circle> to_process;
  static bool no_dmst;
  static Node *root;

  struct Node{
    Node *par = NULL;
    vector<pair<int, int> > out_cands; //Circ, edge idx
    vector<pair<ll, Edge *> > con;
    bool in_use = false;
    ll w = 0; //extra to add to edges in con
    Node *anc(){
      if(!par)
        return this;
      while(par->par)
        par = par->par;
      return par;
    }
    void clean(){
      if(!no_dmst){
        in_use = false;
        for(auto &cur : out_cands)
          to_process[cur.first].clean(cur.second);
      }
    }
    Node *con_to_root(){
      if(anc() == root)
        return root;
      in_use = true;
      Node *super = this; //Will become root or the first Node encountered in a loop.
      while(super == this){
        while(!con.empty() && con.front().second->tar->anc() == anc()){
          pop_heap(con.begin(), con.end(), comp);
          con.pop_back();
        }
        if(con.empty()){
          no_dmst = true;
          return root;
        }
        pop_heap(con.begin(), con.end(), comp);
        auto nxt = con.back();
        con.pop_back();
        w = -nxt.first;
        if(nxt.second->tar->in_use){ //anc() wouldn't change anything
          super = nxt.second->tar->anc();
          to_process.resize(to_process.size()+1);
        } else {
          super = nxt.second->tar->con_to_root();
        }
        if(super != root){
          to_process.back().contents.push_back(nxt.second);
          out_cands.emplace_back(to_process.size()-1, to_process.back().contents.size()-1);
        } else { //Clean circles
          nxt.second->inc = true;
          nxt.second->from->clean();
        }
      }
      if(super != root){ //we are some loops non first Node.
        if(con.size() > super->con.size()){
          swap(con, super->con); //Largest con in loop should not be copied.
          swap(w, super->w);
        }
        for(auto cur : con){
          super->con.emplace_back(cur.first - super->w + w, cur.second);
          push_heap(super->con.begin(), super->con.end(), comp);
        }
      }
      par = super; //root or anc() of first Node encountered in a loop
      return super;
    }
  };

  Node *cur_root;
  vector<Node> graph;
  vector<Edge> edges;

  DMST(int n, vector<EdgeDesc> &desc, int r){ //Self loops and multiple edges are okay.
    graph.resize(n);
    cur_root = &graph[r];
    for(auto &cur : desc) //Edges are reversed internally
      edges.push_back(Edge{&graph[cur.to], &graph[cur.from], cur.w});
    for(int i=0; i<desc.size(); ++i)
      graph[desc[i].to].con.emplace_back(desc[i].w, &edges[i]);
    for(int i=0; i<n; ++i)
      make_heap(graph[i].con.begin(), graph[i].con.end(), comp);
  }
  bool find(){
    root = cur_root;
    no_dmst = false;
    for(auto &cur : graph){
      cur.con_to_root();
      to_process.clear();
      if(no_dmst) return false;
    }
    return true;
  }
//!finish
//!end_codebook
  ll brute_find(){
    root = cur_root;
    ll best_w=-1;
    int best;
    for(int mask=0; mask<(1<<edges.size()); ++mask){
      ll w=0;
      for(int i=0; i<edges.size(); ++i){
        edges[i].inc = mask & (1<<i);
        if(edges[i].inc)
          w += edges[i].w;
      }
      if(best_w != -1 && w>=best_w)
        continue;
      for(int i=0; i<graph.size(); ++i)
        graph[i].in_use = false;
      cur_root->in_use = true;
      for(int i=0; i<graph.size(); ++i){
        for(int j=0; j<graph.size(); ++j){
          for(auto &cur : graph[j].con){
            if(cur.second->inc && cur.second->tar->in_use){
              graph[j].in_use = true;
            }
          }
        }
      }
      bool ok = true;
      for(int i=0; i<graph.size(); ++i)
        ok &= graph[i].in_use;
      if(ok){
        best_w = w;
        best = mask;
      }
    }
    return best_w;
  }
//!begin_codebook
//!start
  ll weight(){
    ll res = 0;
    for(auto &cur : edges){
      if(cur.inc)
        res += cur.w;
    }
    return res;
  }
//!finish
//!start
};

void DMST::Circle::clean(int idx){
  if(!vis){
    vis = true;
    for(int i=0; i<contents.size(); ++i){
      if(i != idx){
        contents[i]->inc = true;
        contents[i]->from->clean();
      }
    }
  }
}

const greater<pair<ll, DMST::Edge *> > DMST::comp;
vector<DMST::Circle> DMST::to_process;
bool DMST::no_dmst;
DMST::Node *DMST::root;
//!finish
//!end_codebook

void test(){
  for(int n=1; ; ++n){
    for(int m=0; m<min(2*n*n, 15); ++m){
      for(int t=0; t<100; ++t){
        vector<EdgeDesc> desc(m);
        for(int i=0; i<m; ++i){
          desc[i].from = rand()%n;
          desc[i].to = rand()%n;
          desc[i].w = rand()%10;
        }
        int k=rand()%n;
        DMST dmst(n, desc, k);
        DMST brute(n, desc, k);
        ll cand_w = dmst.find();
        if(cand_w){
          cand_w = dmst.weight();
        } else{
          cand_w = -1;
        }
        ll real_w = brute.brute_find();
        if(cand_w != real_w){
          DMST dmst2(n, desc, k);
          dmst2.find();
          assert(false);
        }
      }
    }
  }
}

int main(){
  test();
  int t, n, m, k; //http://lightoj.com/volume_showproblem.php?problem=1380
  cin>>t; //As the site does not support C++11 need to use compat version of this file
  for(int ct=0; ct<t; ++ct){
    cin>>n>>m>>k;
    vector<EdgeDesc> desc(m);
    for(int i=0; i<m; ++i){
      scanf("%d %d %d", &desc[i].from, &desc[i].to, &desc[i].w);
    }
    DMST dmst(n, desc, k);
    cout<<"Case "<<ct+1<<": ";
    if(dmst.find()){
      cout<<dmst.weight();
    } else {
      cout<<"Impossible";
    }
    cout<<'\n';
  }
	return 0;
}
