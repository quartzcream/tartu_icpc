#define DEBUG(...) cerr << __VA_ARGS__ << endl;

#ifndef CDEBUG
#undef DEBUG
#define DEBUG(...) ((void)0);
#define NDEBUG
#endif

#define ran(i, a, b) for (auto i = (a); i < (b); i++)

#include <bits/stdc++.h>
typedef long long ll;
typedef long double ld;
using namespace std;
const int mod = 1e9 + 7;
#ifndef M_PI
const double M_PI = acos(-1.0);
#endif
//!escape \section{Dinic}
//!begin_codebook
struct MaxFlow {
  const static ll INF = 1e18;
  int source, sink;
  vector<int> start, now, lvl, adj, rcap, cap_loc, bfs;
  vector<int> cap, orig_cap;
  /*ly*/ll sink_pot = 0;
  vector<bool> visited;
  vector<ll> cost;
  priority_queue<pair<ll, int>, vector<pair<ll, int> >,
    greater<pair<ll, int> > >
    dist_que;/*ry*/
  void add_flow(int idx, ll flow, bool cont = true) {
    cap[idx] -= flow;
    if (cont) add_flow(rcap[idx], -flow, false);
  }
  MaxFlow(
    const vector<tuple<int, int, ll, ll /*ly*/, ll /*ry*/> > &edges) {
    for (auto &cur : edges) { // from, to, cap, rcap/*ly*/, cost/*ry*/
      start.resize(
        max(max(get<0>(cur), get<1>(cur)) + 2, (int)start.size()));
      ++start[get<0>(cur) + 1];
      ++start[get<1>(cur) + 1];
    }
    ran(i, 1, (int)start.size()) start[i] += start[i - 1];
    now = start;
    adj.resize(start.back());
    cap.resize(start.back());
    rcap.resize(start.back());
    /*ly*/ cost.resize(start.back()); /*ry*/
    for (auto &cur : edges) {
      int u, v;
      ll c, rc /*ly*/, c_cost /*ry*/;
      tie(u, v, c, rc /*ly*/, c_cost /*ry*/) = cur;
      assert(u != v);
      adj[now[u]] = v;
      adj[now[v]] = u;
      rcap[now[u]] = now[v];
      rcap[now[v]] = now[u];
      cap_loc.push_back(now[u]);
      /*ly*/ cost[now[u]] = c_cost;
      cost[now[v]] = -c_cost; /*ry*/
      cap[now[u]++] = c;
      cap[now[v]++] = rc;
      orig_cap.push_back(c);
    }
  }
  bool dinic_bfs(int min_cap) {
    lvl.clear();
    lvl.resize(start.size());
    bfs.clear();
    bfs.resize(1, source);
    now = start;
    lvl[source] = 1;
    ran(i, 0, (int)bfs.size()) {
      int u = bfs[i];
      while (now[u] < start[u + 1]) {
        int v = adj[now[u]];
        if (/*ly*/cost[now[u]] == 0 && /*ry*/cap[now[u]] >= min_cap && lvl[v] == 0) {
          lvl[v] = lvl[u] + 1;
          if(v==sink) return true;
          bfs.push_back(v);
        }
        ++now[u];
      }
    }
    return false;
  }
  ll dinic_dfs(int u, ll flow, int min_cap) {
    if (u == sink) return flow;
    if (lvl[u] == lvl[sink]) return 0;
    ll res = 0;
    while (now[u] < start[u + 1]) {
      int v = adj[now[u]];
      if (lvl[v] == lvl[u] + 1/*ly*/ && cost[now[u]] == 0 /*ry*/&&
          cap[now[u]] >= min_cap) {
        ll cur = dinic_dfs(v, min(flow, (ll)cap[now[u]]), min_cap);
        if (cur) {
          add_flow(now[u], cur);
          flow -= cur;
          res += cur;
          if(!flow) break;
        }
      }
      ++now[u];
    }
    return res;
  }
  /*ly*/bool recalc_dist(bool check_imp = false) {
    now = start;
    visited.clear();
    visited.resize(start.size());
    dist_que.emplace(0, source);
    /*lp*/bool imp = false;/*rp*/
    while (!dist_que.empty()) {
      int u;
      ll dist;
      tie(dist, u) = dist_que.top();
      dist_que.pop();
      if (!visited[u]) {
        visited[u] = true;
        /*lp*/if (check_imp && dist != 0) imp = true;/*rp*/
        if (u == sink) sink_pot += dist;
        while (now[u] < start[u + 1]) {
          int v = adj[now[u]];
          if (!visited[v] && cap[now[u]])
            dist_que.emplace(dist + cost[now[u]], v);
          cost[now[u]] += dist;
          cost[rcap[now[u]++]] -= dist;
        }
      }
    }
    /*lp*/if (check_imp) return imp;/*rp*/
    return visited[sink];
  } /*ry*/
  /*lp*/ bool recalc_dist_bellman_ford() { // return whether there is
                                           // a negative cycle
    int i = 0;
    for (; i < (int)start.size() - 1 && recalc_dist(true); ++i) {
    }
    return i == (int)start.size() - 1;
  } /*rp*/
  /*ly*/pair<ll, /*ry*/ll/*ly*/>/*ry*/ calc_flow(int _source, int _sink) {
    source = _source;
    sink = _sink;
    assert(max(source, sink) < start.size() - 1);
    /*ly*/ll tot_flow = 0;
    ll tot_cost = 0;/*ry*/
    /*lp*/if (recalc_dist_bellman_ford()) {
      assert(false);
    } else { /*rp*/ 
/*ly*/      while (recalc_dist()){/*ry*/
        ll flow = 0;
        for(int min_cap = 1<<30; min_cap; min_cap >>= 1){
          while (dinic_bfs(min_cap)) {
            now = start;
            ll cur;
            while (cur = dinic_dfs(source, INF, min_cap)) flow += cur;
          }
        }
        /*ly*/tot_flow += flow;
        tot_cost += sink_pot * flow;/*ry*/
      }
    }
    return /*ly*/{tot_/*ry*/flow/*ly*/, tot_cost}/*ry*/;
  }
  ll flow_on_edge(int idx) {
    assert(idx < cap.size());
    return orig_cap[idx] - cap[cap_loc[idx]];
  }
};
const int nmax = 1055;
int main() {
  int t;
  scanf("%d", &t);
  for (int i = 0; i < t; ++i) {
    vector<tuple<int, int, ll, ll, ll> > edges;
    int n;
    scanf("%d", &n);
    for (int j = 1; j <= n; ++j) {
      edges.emplace_back(j, 2 * n + 1, 1, 0, 0);
    }
    for (int j = 1; j <= n; ++j) {
      int card;
      scanf("%d", &card);
      edges.emplace_back(0, card, 1, 0, 0);
    }
    int ex_c;
    scanf("%d", &ex_c);
    for (int j = 0; j < ex_c; ++j) {
      int a, b;
      scanf("%d %d", &a, &b);
      if (b < a) swap(a, b);
      edges.emplace_back(a, b, nmax, 0, 1);
      edges.emplace_back(b, n + b, nmax, 0, 0);
      edges.emplace_back(n + b, a, nmax, 0, 1);
    }
    int v = 2 * n + 2;
    MaxFlow mf(edges);
    printf("%d\n", (int)mf.calc_flow(0, v - 1).second);
    //cout << mf.flow_on_edge(edge_index) << endl;
  }
}
//!end_codebook

/*
   int n,m;
   cin >> n >> m;
vector<tuple<int,
int, ll, ll> > edges; for(int i = 0; i < m; ++i){ int a,b; ll c;
scanf("%d %d %lld", &a, &b, &c);
if(a != b){
edges.emplace_back(a, b, c, c); //(a, b, c, 0) for directed
}
}
MaxFlow mf(edges);
cout << mf.calc_flow(1, n) << '\n';
this edge
*/

// solves http://www.spoj.com/problems/GREED
// solves http://www.spoj.com/problems/FASTFLOW/
