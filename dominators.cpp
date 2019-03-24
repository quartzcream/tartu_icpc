#include <iostream>
#include <vector>

using namespace std;

struct Tree {
  /* insert structure here */
  void set_root (int u) {
    cout << "root is " << u << endl;
  }
  void add_edge (int u, int v) {
    cout << u << "->" << v << endl;
  }
};

struct Graph {
  vector<vector<int>> in_edges, out_edges;
  vector<int> ord, dfs_index, parent;
  vector<int> sdom, idom;
  vector<vector<int>> rsdom; /* inverse of sdom */
  vector<int> dsu; /* slightly modified version of dsu-s root[] */
  vector<int> label;

  void dfs (int vertex, int par, vector<int> &visited) {
    ord.push_back(vertex);
    parent[vertex] = par;
    dfs_index[vertex] = (int) ord.size() - 1;
    visited[vertex] = 1;
    for (int nxt : out_edges[vertex]) {
      in_edges[nxt].push_back(vertex);
      if (!visited[nxt]) {
        dfs(nxt, vertex, visited);
      }
    }
  }

  void add_edge (int u, int v) {
    out_edges[u].push_back(v);
  }

  Graph (int _vertexc) {
    in_edges = vector<vector<int>> (_vertexc, vector<int> (0));
    out_edges = vector<vector<int>> (_vertexc, vector<int> (0));
    ord = vector<int> (0);
    dfs_index = vector<int> (_vertexc, -1);
    parent = vector<int> (_vertexc, -1);
    sdom = vector<int> (_vertexc);
    idom = vector<int> (_vertexc);
    rsdom = vector<vector<int>> (_vertexc, vector<int> (0));
    dsu = vector<int> (_vertexc);
    label = vector<int> (_vertexc);
    for (int i = 0; i < _vertexc; i++) {
      idom[i] = i;
      sdom[i] = i;
      dsu[i] = i;
      label[i] = i;
    }
  }

  int find (int u, int x = 0) {
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

    if (dfs_index[sdom[label[dsu[u]]]] < dfs_index[sdom[label[u]]]) {
      label[u] = label[dsu[u]];
    }
    dsu[u] = v;
    return x ? v : label[u];
  }

  void merge (int u, int v) {
    dsu[v] = u;
  }
  
  Tree dom_tree (int src) {
    vector<int> visited ((int) idom.size(), 0);
    dfs(src, -1, visited);

    for (int i = (int) ord.size() - 1; i >= 0; i--) {
      int u = ord[i];
      for (int v : in_edges[u]) {
        int w = find(v);
        if (dfs_index[sdom[u]] > dfs_index[sdom[w]]) {
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
    
    Tree ans; /* if your constructor needs # of vertices,
               * use (int) idom.size() + 5 for example */
    for (int i = 1; i < (int) ord.size(); i++) {
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

int main () {
  int vertexc, arcc;
  cin >> vertexc >> arcc;

  Graph G (vertexc); /* if using 1-indexed vertices, use
                      * vertexc + 5 or something */
  for (int i = 0; i < arcc; i++) {
    int u, v;
    cin >> u >> v;

    G.add_edge(u, v);
  }

  Tree T = G.dom_tree(0);
}
