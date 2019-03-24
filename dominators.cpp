#define DEBUG(...) cerr << __VA_ARGS__ << endl;

#ifndef CDEBUG
#undef DEBUG
#define DEBUG(...) ((void)0);
#define NDEBUG
#endif

#define ran(i, a, b) for (auto i = (a); i < (b); i++)

#include <iostream>
#include <vector>
#include <queue>

typedef long long ll;
typedef long double ld;
using namespace std;
//!escape Dominator tree
//!begin_codebook
//!start
/*ly*/struct Tree {
  /* insert structure here */
  //!pause
  int root;
  vector<vector<int>> adj;
  //!unpause
  void set_root(int u) {
    //!pause
#if 0
    //!unpause
    /*lp*/cout << "root is " << u << endl;/*rp*/
    //!pause
#endif
    root = u;
    //!unpause
  }
  void add_edge(int u, int v) {
    //!pause
#if 0
    //!unpause
    /*lp*/cout << u << "->" << v << endl;/*rp*/
    //!pause
#endif
    adj[u].push_back(v);
    //!unpause
  }
  //!pause
  int solve_dfs (int vertex, int &ans) {
    int sum = 1;
    for (int nxt : adj[vertex]) {
      sum += solve_dfs(nxt, ans);
    }

    if (vertex != root) {
      ans = max(ans, sum);
    }
    return sum;
  }

  int solve () {
    int ans = 0;
    solve_dfs(root, ans);
    return ans;
  }
  
  Tree (int _vertexc) {
    root = -1;
    adj = vector<vector<int>> (_vertexc + 5, vector<int> (0));
  }
  //!unpause
};/*ry*/

struct Graph {
  vector<vector<int>> in_edges, out_edges;
  vector<int> ord, dfs_idx, parent;
  vector<int> sdom, idom;
  vector<vector<int>> rsdom; /* inverse of sdom */
  /* slightly modified version of dsu-s root[] */
  vector<int> dsu;
  vector<int> label;

  void dfs(int cur, int par, vector<int> &vis) {
    ord.push_back(cur);
    parent[cur] = par;
    dfs_idx[cur] = (int)ord.size() - 1;
    vis[cur] = 1;
    for (int nxt : out_edges[cur]) {
      if (!vis[nxt])
        dfs(nxt, cur, vis);
    }
  }

  void add_edge(int u, int v) {
    in_edges[v].push_back(u);
    out_edges[u].push_back(v);
  }

  Graph(int n) {
    in_edges.resize(n, vector<int>(0));
    out_edges.resize(n, vector<int>(0));
    rsdom.resize(n, vector<int>(0));
    dfs_idx.resize(n, -1);
    parent.resize(n, -1);
    ran(i, 0, n) {
      sdom.push_back(i);
      idom.push_back(i);
      dsu.push_back(i);
      label.push_back(i);
    }
  }

  int find(int u, int x = 0) {
    if (u == dsu[u]) {
      if (x) {
        return -1;
      } else {
        return u;
      }
    }

    int v = find(dsu[u], x + 1);
    if (v < 0) {
      return u;
    }

    if (dfs_idx[sdom[label[dsu[u]]]] <
        dfs_idx[sdom[label[u]]]) {
            label[u] = label[dsu[u]];
    }
    dsu[u] = v;
    return x ? v : label[u];
  }

  void merge(int u, int v) { dsu[v] = u; }

  Tree dom_tree(int src) {
    vector<int> vis(idom.size(), 0);
    dfs(src, -1, vis);

    for (int i = (int)ord.size() - 1; i >= 0; --i) {
      int u = ord[i];
      for (int v : in_edges[u]) {
        int w = find(v);
        if (dfs_idx[sdom[u]] > dfs_idx[sdom[w]]) {
          sdom[u] = sdom[w];
        }
      }

      if (i > 0) {
        rsdom[sdom[u]].push_back(u);
      }

      for (int w : rsdom[u]) {
        int v = find(w);
        if (sdom[v] == sdom[w]) {
          idom[w] = sdom[w];
        } else {
          idom[w] = v;
        }
      }

      if (i > 0) {
        merge(parent[u], u);
      }
    }

    //!pause
#if 0
    //!unpause
    Tree ans;    
    //!pause
#endif
    /*ly*/Tree ans (sdom.size() + 5);/*ry*/
    //!unpause
    ran(i, 1, (int)ord.size()) {
      int u = ord[i];
      if (idom[u] != sdom[u]) {
        idom[u] = idom[idom[u]];
      }
      ans.add_edge(idom[u], u);
    }
    ans.set_root(src);
    return ans;
  }
};
//!finish
//!end_codebook
// solves:
// https://codeforces.com/contest/757/problem/F
const int maxn = 200005;
const ll inf = 1LL << 60;
vector<pair<int, ll>> adj [maxn];
ll dist [maxn];
vector<int> preds [maxn];
bool vis [maxn];

int main () {
  int vertexc, edgec, src;
  cin >> vertexc >> edgec >> src;

  for (int i = 0; i < edgec; i++) {
    int u, v, w;
    cin >> u >> v >> w;

    adj[u].push_back(make_pair(v, w));
    adj[v].push_back(make_pair(u, w));
  }

  for (int i = 1; i <= vertexc; i++) {
    dist[i] = inf;
  }

  Graph SPDAG (vertexc + 5);
  priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> Q;
  dist[src] = 0;
  Q.push(make_pair(dist[src], src));
  while (!Q.empty()) {
    int u = Q.top().second;
    Q.pop();

    if (!vis[u]) {
      vis[u] = 1;

      for (int v : preds[u]) {
        SPDAG.add_edge(v, u);
      }
      
      for (pair<int, ll> nxt : adj[u]) {
        if (dist[u] + nxt.second < dist[nxt.first]) {
          dist[nxt.first] = dist[u] + nxt.second;
          preds[nxt.first] = vector<int> (1, u);
          Q.push(make_pair(dist[nxt.first], nxt.first));
        } else if (dist[u] + nxt.second == dist[nxt.first]) {
          preds[nxt.first].push_back(u);
        }
      }
    }
  }

  Tree dom = SPDAG.dom_tree(src);
  cout << dom.solve() << endl;
}
