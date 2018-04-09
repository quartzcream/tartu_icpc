#include <bits/stdc++.h>
using namespace std;
typedef long long               ll;

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
    Node *par;
    vector<pair<int, int> > out_cands; //Circ, edge idx
    vector<pair<ll, Edge *> > con;
    bool in_use;
    ll w; //extra to add to edges in con
    Node(){
      par = NULL;
      in_use = false;
      w = 0;
    }
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
        for(int i=0; i<out_cands.size(); ++i){
          pair<int, int> &cur = out_cands[i];
          to_process[cur.first].clean(cur.second);
        }
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
        pair<ll, Edge *> nxt = con.back();
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
          out_cands.push_back(make_pair((int)to_process.size()-1, (int)to_process.back().contents.size()-1));
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
        for(int i=0; i<con.size(); ++i){
          pair<ll, Edge *> &cur = con[i];
          super->con.push_back(make_pair(cur.first - super->w + w, cur.second));
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
  DMST(int n, vector<EdgeDesc> &desc, int r){
    graph.resize(n);
    cur_root = &graph[r];
    for(int i=0; i<desc.size(); ++i){
      EdgeDesc &cur = desc[i];
      edges.push_back(Edge{&graph[cur.to], &graph[cur.from], cur.w});
    }
    for(int i=0; i<desc.size(); ++i)
      graph[desc[i].to].con.push_back(make_pair((ll)desc[i].w, &edges[i]));
    for(int i=0; i<n; ++i)
      make_heap(graph[i].con.begin(), graph[i].con.end(), comp);
  }
  bool find(){
    root = cur_root;
    no_dmst = false;
    for(int i=0; i<graph.size(); ++i){
      Node &cur = graph[i];
      cur.con_to_root();
      to_process.clear();
      if(no_dmst) return false;
    }
    return true;
  }
  ll weight(){
    ll res = 0;
    for(int i=0; i<edges.size(); ++i){
      Edge &cur = edges[i];
      if(cur.inc)
        res += cur.w;
    }
    return res;
  }
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

int main(){ //http://lightoj.com/volume_showproblem.php?problem=1380
  int t, n, m, k;
  cin>>t;
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
      cout<<"impossible";
    }
    cout<<'\n';
  }
	return 0;
}
