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

//!escape 2-Sat O(n) and SCC O(n)
//!begin_codebook
//!start
struct Graph {
  int n;
  vector<vector<int> > con;

  Graph(int nsize) {
    n = nsize;
    con.resize(n);
  }

  void add_edge(int u, int v) { con[u].push_back(v); }

  void top_dfs(int pos, vector<int> &result,
    vector<bool> &explr, vector<vector<int> > &revcon) {
    if (explr[pos]) return;
    explr[pos] = true;
    for (auto next : revcon[pos])
      top_dfs(next, result, explr, revcon);
    result.push_back(pos);
  }

  vector<int> topsort() {
    vector<vector<int> > revcon(n);
    ran(u, 0, n) {
      for (auto v : con[u]) revcon[v].push_back(u);
    }

    vector<int> result;
    vector<bool> explr(n, false);
    ran(i, 0, n) top_dfs(i, result, explr, revcon);
    reverse(result.begin(), result.end());
    return result;
  }/*ly*/

  void dfs(
    int pos, vector<int> &result, vector<bool> &explr) {
    if (explr[pos]) return;
    explr[pos] = true;
    for (auto next : con[pos]) dfs(next, result, explr);
    result.push_back(pos);
  }/*ry*//*lp*/
  vector<vector<int> > scc() {
    //!pause
    // tested on
    // https://www.hackerearth.com/practice/algorithms/graphs/strongly-connected-comps/practice-problems/algorithm/a-walk-to-remember-qualifier2/
    //!unpause
    vector<int> order = topsort();
    reverse(order.begin(), order.end());
    vector<bool> explr(n, false);
    vector<vector<int> > res;
    for (auto it = order.rbegin(); it != order.rend();
         ++it) {
      vector<int> comp;
      top_dfs(*it, comp, explr, con);
      sort(comp.begin(), comp.end());
      res.push_back(comp);
    }
    sort(res.begin(), res.end());
    return res;
  }/*rp*/
};
//!finish

int main() {
  //!pause
  ios_base::sync_with_stdio(false);
  cin.tie(0);
  // Solution for:
  // http://codeforces.com/group/PjzGiggT71/contest/221700/problem/C
  //!unpause

  int n, m;
  cin >> n >> m;

  Graph g(2 * m);

  ran(i, 0, n) {
    int a, sa, b, sb;
    cin >> a >> sa >> b >> sb;
    a--, b--;

    g.add_edge(2 * a + 1 - sa, 2 * b + sb);
    g.add_edge(2 * b + 1 - sb, 2 * a + sa);
  }

  vector<int> state(2 * m, 0);
  {
    vector<int> order = g.topsort();
    vector<bool> explr(2 * m, false);

    for (auto u : order) {
      vector<int> traversed;
      g.dfs(u, traversed, explr);

      if (traversed.size() > 0 &&
          !state[traversed[0] ^ 1]) {
        for (auto c : traversed) state[c] = 1;
      }
    }
  }

  ran(i, 0, m) {
    if (state[2 * i] == state[2 * i + 1]) {
      cout << "IMPOSSIBLE\n";
      return 0;
    }
  }

  ran(i, 0, m) cout << state[2 * i + 1] << '\n';
  return 0;
}
//!end_codebook
