#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

//!escape \section{Suffix automaton $O((n+q)\log(|\text{alpha}|))$}

//!begin_codebook
class AutoNode {
 private:
  map< char, AutoNode * > nxt_char;  // Map is faster than hashtable and unsorted arrays
 public:
  int len; //Length of longest suffix in equivalence class.
  AutoNode *suf;
  bool has_nxt(char c) const { 
		return nxt_char.count(c); 
	}
  AutoNode *nxt(char c) {
    if (!has_nxt(c)) 
			return NULL;
    return nxt_char[c];
  }
  void set_nxt(char c, AutoNode *node) { 
		nxt_char[c] = node; 
	}
  AutoNode *split(int new_len, char c) {
    AutoNode *new_n = new AutoNode;
    new_n->nxt_char = nxt_char;
    new_n->len = new_len;
    new_n->suf = suf;
    suf = new_n;
    return new_n;
  }
  // Extra functions for matching and counting
  AutoNode *lower_depth(int depth) { //move to longest suffix of current with a maximum length of depth.
    if (suf->len >= depth) 
			return suf->lower_depth(depth);
    return this;
  }
  AutoNode *walk(char c, int depth, int &match_len) { //move to longest suffix of walked path that is a substring
    match_len = min(match_len, len);									//includes depth limit(needed for finding matches)
    if (has_nxt(c)) {																	//as suffixes are in classes match_len must be tracte eternally
      ++match_len;
      return nxt(c)->lower_depth(depth);
    }
    if (suf) 
			return suf->walk(c, depth, match_len);
    return this;
  }
  int paths_to_end = 0;
  void set_as_end() { //All suffixes of current node are marked as ending nodes. 
    paths_to_end = 1;
    if (suf) suf->set_as_end();
  }
  bool vis = false;
  void calc_paths_to_end() { 	//Call ONCE from ROOT. For each node calculates number of ways to reach an end node.
    if (!vis) {							 	//paths_to_end is ocurence count for any strings in current suffix equivalence class.
      vis = true;
      for (auto cur : nxt_char) {
        cur.second->calc_paths_to_end();
        paths_to_end += cur.second->paths_to_end;
      }
    }
  }
};
struct SufAutomaton {
  AutoNode *last;
  AutoNode *root;
  void extend(char new_c) {
    AutoNode *new_end = new AutoNode;  // The equivalence class containing the whole new string
    new_end->len = last->len + 1;
    AutoNode *suf_w_nxt = last;                        // The whole old string class
    while (suf_w_nxt && !suf_w_nxt->has_nxt(new_c)) {  // is turned into the longest suffix which
                                                       // can be turned into a substring of old state
                                                       // by appending new_c
      suf_w_nxt->set_nxt(new_c, new_end);
      suf_w_nxt = suf_w_nxt->suf;
    }
    if (!suf_w_nxt) {  // The new character isn't part of the old string
      new_end->suf = root;
    } else {
      AutoNode *max_sbstr = suf_w_nxt->nxt(new_c);  // Equivalence class containing longest
                                                    // substring which is a suffix of the new state.
      if (suf_w_nxt->len + 1 == max_sbstr->len) { // Check whether splitting is needed
        new_end->suf = max_sbstr;
      } else {
        AutoNode *eq_sbstr = max_sbstr->split(suf_w_nxt->len + 1, new_c);
        new_end->suf = eq_sbstr;
        // Make suffixes of suf_w_nxt point to eq_sbstr instead of mox_sbstr
        AutoNode *w_edge_to_eq_sbstr = suf_w_nxt;
        while (w_edge_to_eq_sbstr != 0 && w_edge_to_eq_sbstr->nxt(new_c) == max_sbstr) {
          w_edge_to_eq_sbstr->set_nxt(new_c, eq_sbstr);
          w_edge_to_eq_sbstr = w_edge_to_eq_sbstr->suf;
        }
      }
    }
    last = new_end;
  }
  SufAutomaton(string to_suffix) {
    root = new AutoNode;
    root->len = 0;
    root->suf = NULL;
    last = root;
    for (char c : to_suffix) extend(c);
  }
};
//!end_codebook

// http://codeforces.com/contest/235/problem/C
int main() {
  cin.sync_with_stdio(false);
  string s;
  int n;
  cin >> s;
  SufAutomaton s_suf(s);
  s_suf.last->set_as_end();
  s_suf.root->calc_paths_to_end();
  cin >> n;
  for (int i = 0; i < n; ++i) {
    string x;
    int res = 0;
    cin >> x;
    int len_for_match = x.size();
    int match_len = 0;
    AutoNode *cur = s_suf.root;
    for (int j = 1; j < x.size(); ++j) {
      cur = cur->walk(x[j], len_for_match, match_len);
    }
    unordered_set< AutoNode * > matches;
    for (char c : x) {
      cur = cur->walk(c, len_for_match, match_len);
      if (match_len >= len_for_match) {
        matches.insert(cur);
      }
    }
    for (AutoNode *match : matches) {
      res += match->paths_to_end;
    }
    cout << res << '\n';
  }
  return 0;
}
