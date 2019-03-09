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

//!escape \section{Aho Corasick $\mathcal{O}(|\text{alpha}|\sum{\text{len}})$}

//!begin_codebook
//!start
const int alpha_size = 26;
struct Node {
  Node *nxt[alpha_size]; // May use other structures to move in trie
  Node *suffix;
  Node() { memset(nxt, 0, alpha_size * sizeof(Node *)); }
  int cnt = 0;
};
Node *aho_corasick(vector<vector<char> > &dict) {
  Node *root = new Node;
  root->suffix = 0;
  vector<pair<vector<char> *, Node *> > state;
  for (vector<char> &s : dict) state.emplace_back(&s, root);
  for (int i = 0; !state.empty(); ++i) {
    vector<pair<vector<char> *, Node *> > nstate;
    for (auto &cur : state) {
      Node *nxt = cur.second->nxt[(*cur.first)[i]];
      if (nxt) {
        cur.second = nxt;
      } else {
        nxt = new Node;
        cur.second->nxt[(*cur.first)[i]] = nxt;
        Node *suf = cur.second->suffix;
        cur.second = nxt;
        nxt->suffix = root; // set correct suffix link
        while (suf) {
          if (suf->nxt[(*cur.first)[i]]) {
            nxt->suffix = suf->nxt[(*cur.first)[i]];
            break;
          }
          suf = suf->suffix;
        }
      }
      if (cur.first->size() > i + 1) nstate.push_back(cur);
    }
    state = nstate;
  }
  return root;
}
//!finish
// auxilary functions for searhing and counting
//!start
Node *walk(Node *cur,
  char c) { // longest prefix in dict that is suffix of walked string.
  while (true) {
    if (cur->nxt[c]) return cur->nxt[c];
    if (!cur->suffix) return cur;
    cur = cur->suffix;
  }
}
//!finish
//!start
void cnt_matches(Node *root, vector<char> &match_in) {
  Node *cur = root;
  for (char c : match_in) {
    cur = walk(cur, c);
    ++cur->cnt;
  }
}
//!finish
//!start
void add_cnt(Node *root) { // After counting matches propagete ONCE to
                           // suffixes for final counts
  vector<Node *> to_visit = {root};
  ran(i, 0, to_visit.size()) {
    Node *cur = to_visit[i];
    ran(j, 0, alpha_size) {
      if (cur->nxt[j]) to_visit.push_back(cur->nxt[j]);
    }
  }
  for (int i = to_visit.size() - 1; i > 0; --i)
    to_visit[i]->suffix->cnt += to_visit[i]->cnt;
}
//!finish
int main() {
  //!end_codebook
  // http://codeforces.com/group/s3etJR5zZK/contest/212916/problem/4
  //!begin_codebook
  int n, len;
  scanf("%d %d", &len, &n);
  vector<char> a(len + 1);
  scanf("%s", a.data());
  a.pop_back();
  for (char &c : a) c -= 'a';
  vector<vector<char> > dict(n);
  ran(i, 0, n) {
    scanf("%d", &len);
    dict[i].resize(len + 1);
    scanf("%s", dict[i].data());
    dict[i].pop_back();
    for (char &c : dict[i]) c -= 'a';
  }
  Node *root = aho_corasick(dict);
  cnt_matches(root, a);
  add_cnt(root);
  ran(i, 0, n) {
    Node *cur = root;
    for (char c : dict[i]) cur = walk(cur, c);
    printf("%d\n", cur->cnt);
  }
}
//!end_codebook
