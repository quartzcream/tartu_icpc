#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

//!escape \section{Suffix automaton and tree $\mathcal{O}((n+q)\log(|\text{alpha}|))$}

//!begin_codebook
//!start
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
//!finish
//!start
  // Extra functions for matching and counting
  AutoNode *lower_depth(int depth) { //move to longest suffix of current with a maximum length of depth.
    if (suf->len >= depth) 
			return suf->lower_depth(depth);
    return this;
  }
  AutoNode *walk(char c, int depth, int &match_len) { //move to longest suffix of walked path that is a substring
    match_len = min(match_len, len); //includes depth limit(needed for finding matches)
    if (has_nxt(c)) {	//as suffixes are in classes match\_len must be tracked externally
      ++match_len;
      return nxt(c)->lower_depth(depth);
    }
    if (suf) 
			return suf->walk(c, depth, match_len);
    return this;
  }
//!finish
//!start
  int paths_to_end = 0;
  void set_as_end() { //All suffixes of current node are marked as ending nodes. 
    paths_to_end += 1;
    if (suf) suf->set_as_end();
  }
  bool vis = false;
  void calc_paths_to_end() { //Call ONCE from ROOT. For each node calculates number of ways to reach an end node.
    if (!vis) {	//paths\_to\_end is ocurence count for any strings in current suffix equivalence class.
      vis = true;
      for (auto cur : nxt_char) {
        cur.second->calc_paths_to_end();
        paths_to_end += cur.second->paths_to_end;
      }
    }
  }
//!finish
//!start
  //Transform into suffix tree of reverse string
  map<char, AutoNode * > tree_links;
  int end_dist = 1<<30;
  int calc_end_dist(){
    if(end_dist == 1<<30){
      if(nxt_char.empty())
        end_dist = 0;
      for (auto cur : nxt_char)
        end_dist = min(end_dist, 1+cur.second->calc_end_dist());
    }
    return end_dist;
  }
  bool vis_t = false;
  void build_suffix_tree(string &s) { //Call ONCE from ROOT.
    if (!vis_t) {	
      vis_t = true;
      if(suf)
        suf->tree_links[s[s.size()-end_dist-suf->len-1]] = this;
      for (auto cur : nxt_char)
        cur.second->build_suffix_tree(s);
    }
  }
};
struct SufAutomaton {
  AutoNode *last;
  AutoNode *root;
  void extend(char new_c) {
    AutoNode *new_end = new AutoNode; 
//!end_codebook
    //The equivalence class containing the whole new string
//!begin_codebook
    new_end->len = last->len + 1;
    AutoNode *suf_w_nxt = last;                       
    while (suf_w_nxt && !suf_w_nxt->has_nxt(new_c)) { 
//!end_codebook
    /* The whole old string class is turned into the longest suffix which can be turned into a substring of old state by appending new\_c */
//!begin_codebook
      suf_w_nxt->set_nxt(new_c, new_end);
      suf_w_nxt = suf_w_nxt->suf;
    }
    if (!suf_w_nxt) { 
//!end_codebook
      // The new character isn't part of the old string
//!begin_codebook
      new_end->suf = root;
    } else {
      AutoNode *max_sbstr = suf_w_nxt->nxt(new_c);
//!end_codebook
      //Equivalence class containing longest substring which is a suffix of the new state.
//!begin_codebook
      if (suf_w_nxt->len + 1 == max_sbstr->len) {
//!end_codebook
        // Check whether splitting is needed
//!begin_codebook
        new_end->suf = max_sbstr;
      } else {
        AutoNode *eq_sbstr = max_sbstr->split(suf_w_nxt->len + 1, new_c);
        new_end->suf = eq_sbstr;
//!end_codebook
        // Make suffixes of suf\_w\_nxt point to eq\_sbstr instead of mox\_sbstr
//!begin_codebook
        AutoNode *w_edge_to_eq_sbstr = suf_w_nxt;
        while (w_edge_to_eq_sbstr != 0 && w_edge_to_eq_sbstr->nxt(new_c) == max_sbstr) {
          w_edge_to_eq_sbstr->set_nxt(new_c, eq_sbstr);
          w_edge_to_eq_sbstr = w_edge_to_eq_sbstr->suf;
        }
      }
    }
    last = new_end;
  }
//!finish
//!start
  SufAutomaton(string &s) {
    root = new AutoNode;
    root->len = 0;
    root->suf = NULL;
    last = root;
    for (char c : s) extend(c);
    root->calc_end_dist(); //To build suffix tree use reversed string
    root->build_suffix_tree(s);
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
