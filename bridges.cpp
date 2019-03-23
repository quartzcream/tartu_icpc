#include <bits/stdc++.h>
using namespace std;

const int nmax = 3e5 + 5;

//!escape Bridges O(n)

//!begin_codebook
//!start
struct vert;

struct edge {
  bool exists = true;
  vert *dest;
  edge *rev;
  edge(vert *_dest) : dest(_dest) { rev = NULL; }
  vert &operator*() { return *dest; }
  vert *operator->() { return dest; }
  bool is_bridge();
};

struct vert {
  deque<edge> con;
  int val = 0;
  int seen;
  int dfs(int upd, edge *ban) { // handles multiple edges
    if (!val) {
      val = upd;
      seen = val;
      for (edge &nxt : con) {
        if (nxt.exists && (&nxt) != ban)
          seen = min(seen, nxt->dfs(upd + 1, nxt.rev));
      }
    }
    return seen;
  }/*ly*/
  void remove_adj_bridges() {
    for (edge &nxt : con) {
      if (nxt.is_bridge()) nxt.exists = false;
    }
  }/*ry*//*lp*/
  int cnt_adj_bridges() {
    int res = 0;
    for (edge &nxt : con) res += nxt.is_bridge();
    return res;
  }/*rp*/
};

bool edge::is_bridge() {
  return exists && (dest->seen > rev->dest->val ||
                     dest->val < rev->dest->seen);
}
//!finish
vert graph[nmax];

int main() { // Mechanics Practice BRIDGES
  int n, m;
  cin >> n >> m;
  for (int i = 0; i < m; ++i) {
    int u, v;
    scanf("%d %d", &u, &v);
    graph[u].con.emplace_back(graph + v);
    graph[v].con.emplace_back(graph + u);
    graph[u].con.back().rev = &graph[v].con.back();
    graph[v].con.back().rev = &graph[u].con.back();
  }
  graph[1].dfs(1, NULL);
  int res = 0;
  for (int i = 1; i <= n; ++i)
    res += graph[i].cnt_adj_bridges();
  cout << res / 2 << endl;
}
//!end_codebook
