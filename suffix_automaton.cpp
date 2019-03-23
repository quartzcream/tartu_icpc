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

//!escape Suffix automaton and tree O((n+q)log(|alpha|))

//!begin_codebook
//!start
struct Node {
  map<char, Node *> nxt_char;
  // Map is faster than hashtable and unsorted arrays
  int len; // Length of longest suffix in equivalence
           // class.
  Node *suf;
  bool has_nxt(char c) const {
    return nxt_char.count(c);
  }
  Node *nxt(char c) {
    if (!has_nxt(c)) return NULL;
    return nxt_char[c];
  }
  void set_nxt(char c, Node *node) {
    nxt_char[c] = node;
  }
  Node *split(int new_len, char c) {
    Node *new_n = new Node;
    new_n->nxt_char = nxt_char;
    new_n->len = new_len;
    new_n->suf = suf;
    suf = new_n;
    return new_n;
  }
  //!finish
  //!start
  // Extra functions for matching and counting
  Node *lower(int depth) {
    // move to longest suf of current with a maximum
    // length of depth.
    if (suf->len >= depth) return suf->lower(depth);
    return this;
  }
  Node *walk(char c, int depth, int &match_len) {
    // move to longest suffix of walked path that is a
    // substring
    match_len = min(match_len, len);
    // includes depth limit(needed for finding matches)
    if (has_nxt(
          c)) { // as suffixes are in classes match_len
                // must be tracked externally
      ++match_len;
      return nxt(c)->lower(depth);
    }
    if (suf) return suf->walk(c, depth, match_len);
    return this;
  }
  //!finish
  //!start
  int paths_to_end = 0;
  void set_as_end() { // All suffixes of current node are
                      // marked as ending nodes.
    paths_to_end += 1;
    if (suf) suf->set_as_end();
  }
  bool vis = false;
  void calc_paths() {
    /* Call ONCE from ROOT. For each node  calculates
     * number of ways to reach an end node. paths_to_end
     * is ocurence count for any strings in current
     * suffix equivalence class. */
    if (!vis) {
      vis = true;
      for (auto cur : nxt_char) {
        cur.second->calc_paths();
        paths_to_end += cur.second->paths_to_end;
      }
    }
  }
  // Transform into suffix tree of reverse string/*ly*/
  map<char, Node *> tree_links;
  int end_dist = 1 << 30;
  int calc_end_dist() {
    if (end_dist == 1 << 30) {
      if (nxt_char.empty()) end_dist = 0;
      for (auto cur : nxt_char)
        end_dist =
          min(end_dist, 1 + cur.second->calc_end_dist());
    }
    return end_dist;
  }
  bool vis_t = false;
  void build_suffix_tree(
    string &s) { // Call ONCE from ROOT.
    if (!vis_t) {
      vis_t = true;
      if (suf)
        suf->tree_links[s[s.size() - end_dist -
                          suf->len - 1]] = this;
      for (auto cur : nxt_char)
        cur.second->build_suffix_tree(s);
    }
  }/*ry*/
};
//!finish
//!start
struct SufAuto {
  Node *last;
  Node *root;
  void extend(char new_c) {
    Node *nlast = new Node;
    //!pause
    // The equivalence class containing the whole new
    // string
    //!unpause
    nlast->len = last->len + 1;
    Node *swn = last;
    while (swn && !swn->has_nxt(new_c)) {
      //!pause
      /* The whole old string class is turned into the
       * longest suffix which can be turned into a
       * substring of old state by appending new_c */
      //!unpause
      swn->set_nxt(new_c, nlast);
      swn = swn->suf;
    }
    if (!swn) {
      //!pause
      // The new character isn't part of the old string
      //!unpause
      nlast->suf = root;
    } else {
      Node *max_sbstr = swn->nxt(new_c);
      //!pause
      // Equivalence class containing longest substring
      // which is a suffix of the new state.
      //!unpause
      if (swn->len + 1 == max_sbstr->len) {
        //!pause
        // Check whether splitting is needed
        //!unpause
        nlast->suf = max_sbstr;
      } else { // remove for minimal DFA that matches
               // suffixes and crap
        Node *eq_sbstr =
          max_sbstr->split(swn->len + 1, new_c);
        nlast->suf = eq_sbstr;
        //!pause
        // Make suffixes of suf_w_nxt point to eq_sbstr
        // instead of mox_sbstr
        //!unpause
        Node *x = swn; // x = with_edge_to_eq_sbstr
        while (x != 0 && x->nxt(new_c) == max_sbstr) {
          x->set_nxt(new_c, eq_sbstr);
          x = x->suf;
        }
      }
    }
    last = nlast;
  }
  SufAuto(string &s) {
    root = new Node;
    root->len = 0;
    root->suf = NULL;
    last = root;
    for (char c : s) extend(c);/*ly*/
    root->calc_end_dist(); // To build suffix tree use
                           // reversed string
    root->build_suffix_tree(s);/*ry*/
  }
};
//!finish
//!end_codebook

// http://codeforces.com/contest/235/problem/C
int main() {
  cin.sync_with_stdio(false);
  string s;
  int n;
  cin >> s;
  SufAuto s_suf(s);
  s_suf.last->set_as_end();
  s_suf.root->calc_paths();
  cin >> n;
  for (int i = 0; i < n; ++i) {
    string x;
    int res = 0;
    cin >> x;
    int len_for_match = x.size();
    int match_len = 0;
    Node *cur = s_suf.root;
    for (int j = 1; j < x.size(); ++j) {
      cur = cur->walk(x[j], len_for_match, match_len);
    }
    unordered_set<Node *> matches;
    for (char c : x) {
      cur = cur->walk(c, len_for_match, match_len);
      if (match_len >= len_for_match) {
        matches.insert(cur);
      }
    }
    for (Node *match : matches) {
      res += match->paths_to_end;
    }
    cout << res << '\n';
  }
  return 0;
}
