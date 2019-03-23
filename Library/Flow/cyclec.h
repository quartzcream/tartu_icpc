//----------------------------------------------------------------
// OMLBEGIN: Min-Cost Flow with Cycle Cancelling
#include <algorithm>
#include <deque>
#include <vector>
using namespace std;

//!escape Min Cost Max Flow with Cycle Cancelling O(Cnm)
//!begin_codebook
//!start
struct Network {
  struct Node;

  struct Edge {
    Node *u, *v;
    int f, c, cost;

    Node* from(Node* pos) {
      if (pos == u) return v;
      return u;
    }
    int getCap(Node* pos) {
      if (pos == u) return c - f;
      return f;
    }
    int addFlow(Node* pos, int toAdd) {
      if (pos == u) {
        f += toAdd;
        return toAdd * cost;
      } else {
        f -= toAdd;
        return -toAdd * cost;
      }
    }
  };
  struct Node {
    vector<Edge*> conn;
    int index;
  };

  deque<Node> nodes;
  deque<Edge> edges;

  Node* addNode() {
    nodes.push_back(Node());
    nodes.back().index = nodes.size() - 1;
    return &nodes.back();
  }
  Edge* addEdge(
    Node* u, Node* v, int f, int c, int cost) {
    edges.push_back({u, v, f, c, cost});
    u->conn.push_back(&edges.back());
    v->conn.push_back(&edges.back());
    return &edges.back();
  }

  // Assumes all needed flow has already been added
  int minCostMaxFlow() {
    int n = nodes.size();
    int result = 0;

    struct State {
      int p;
      Edge* used;
    };

    while (1) {
      vector<vector<State> > state(
        1, vector<State>(n, {0, 0}));

      for (int lev = 0; lev < n; lev++) {
        state.push_back(state[lev]);
        for (int i = 0; i < n; i++) {
          if (lev == 0 ||
              state[lev][i].p < state[lev - 1][i].p) {
            for (Edge* edge : nodes[i].conn) {
              if (edge->getCap(&nodes[i]) > 0) {
                int np =
                  state[lev][i].p + (edge->u == &nodes[i]
                                        ? edge->cost
                                        : -edge->cost);
                int ni = edge->from(&nodes[i])->index;

                if (np < state[lev + 1][ni].p) {
                  state[lev + 1][ni].p = np;
                  state[lev + 1][ni].used = edge;
                }
              }
            }
          }
        }
      }

      // Now look at the last level
      bool valid = false;

      for (int i = 0; i < n; i++)
        if (state[n - 1][i].p > state[n][i].p) {
          valid = true;

          vector<Edge*> path;

          int cap = 1000000000;
          Node* cur = &nodes[i];
          int clev = n;

          vector<bool> explr(n, false);

          while (!explr[cur->index]) {
            explr[cur->index] = true;

            State cstate = state[clev][cur->index];
            cur = cstate.used->from(cur);

            path.push_back(cstate.used);
          }

          reverse(path.begin(), path.end());

          {
            int i = 0;
            Node* cur2 = cur;

            do {
              cur2 = path[i]->from(cur2);
              i++;
            } while (cur2 != cur);

            path.resize(i);
          }

          for (auto edge : path) {
            cap = min(cap, edge->getCap(cur));
            cur = edge->from(cur);
          }

          for (auto edge : path) {
            result += edge->addFlow(cur, cap);
            cur = edge->from(cur);
          }
        }

      if (!valid) break;
    }

    return result;
  }
};
//!finish
//!end_codebook
// OMLEND: Min-Cost Flow with Cycle Cancelling
//----------------------------------------------------------------
