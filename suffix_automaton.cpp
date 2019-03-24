#define DEBUG(...) cerr << __VA_ARGS__ << endl;

#ifndef CDEBUG
#undef DEBUG
#define DEBUG(...) ((void)0);
#define NDEBUG
#endif

#define ran(i, a, b) for (auto i = (a); i < (b); i++)

#include <bits/stdc++.h>
#ifndef ONLINE_JUDGE
#include <allocator.h>
#endif
typedef long long ll;
typedef long double ld;
using namespace std;
const int mod = 1e9 + 7;
#ifndef M_PI
const double M_PI = acos(-1.0);
#endif

#ifdef ONLINE_JUDGE
const int BLOCK = 8;
const int MEM_SIZE = 1<<26;
char glob_buf[MEM_SIZE];
int glob_idx;
vector<bool> glob_used;

void init_mem(){
  glob_used.resize(MEM_SIZE/BLOCK);
  glob_used[0] = true;
}

template<typename T>
struct Ptr{
  unsigned idx;
  explicit Ptr(T *tar){
    idx = (char *)tar-glob_buf;
  }
  Ptr(){idx = 0;}
  template<typename... Args>
  void construct(Args... args) {
    new (glob_buf+idx) T(args...);
  }
  T *operator->() {
    assert(idx);
    return (T *)(glob_buf+idx);
  }
  T &operator*() {
    return *operator->(); 
  }
  bool operator==(const Ptr &oth) const {
    return idx == oth.idx;
  }
  operator unsigned() const{ return idx; }
  Ptr &operator+=(int diff){ idx += diff*sizeof(T); return *this;}
  Ptr operator+(int diff){
    Ptr res;
    res.idx = idx;
    return res += diff;
  }
  T &operator[](int diff){
    return *operator+(diff);
  }
};

template<typename T, typename... Args>
Ptr<T> alloc(int n, Args... args){//TLE if running low on mem
  unsigned len = 0;
  while(len < sizeof(T)*n){
    if(!glob_idx) glob_idx = MEM_SIZE/BLOCK;
    if(glob_used[--glob_idx]){
      len = 0;
    } else {
      len += BLOCK;
    }
  }
  ran(i, 0, ((int)sizeof(T)*n+BLOCK-1)/BLOCK)
    glob_used[glob_idx+i] = true;
  Ptr<T> res;
  if(n)  res.idx = BLOCK*glob_idx;
  ran(i, 0, n) (res+i).construct(args...);
  return res;
}

template<typename T>
void dealloc(Ptr<T> ptr, int n){
  ran(i, 0, ((int)sizeof(T)*n+BLOCK-1)/BLOCK)
    glob_used[ptr.idx/BLOCK+i] = false;
}

template<typename T>
struct hash<Ptr<T> > {
  std::size_t operator()(const Ptr<T>& cur) const {
    return cur.idx;
  }
};
#endif

//!escape Suffix automaton and tree O((n+q)log(|alpha|)) - 10+M length/s

//!begin_codebook
//!start
struct Node;

typedef Ptr<Node> P;

struct Node {
  int act = 0;
  Ptr<P> out;
  int len; // Length of longest suffix in equivalence
  P suf;   // class.
  char size = 0;
  char cap = 0;
  Node(int _len) : len(_len) {};
  Node(int &_act, Ptr<P> &_out, int &_len, P &_suf,
       int _size, int _cap) : act(_act), len(_len),
       suf(_suf), size(_size), cap(_cap) {
    out = alloc<P>(cap);
    ran(i, 0, size)
      out[i]  = _out[i];
  }
  int has_nxt(char c) {
    return act & (1<<(c-'a'));
  }
  P nxt(char c) {
    return
      out[__builtin_popcount(act & ((1<<(c-'a'))-1))];
  }
  void set_nxt(char c, P nxt) {
    int idx = __builtin_popcount(act & ((1<<(c-'a'))-1));
    if(has_nxt(c)){
      out[idx] = nxt;
    } else{
      if(size == cap){
        cap *= 2;
        if(!size)
          cap = 2;
        Ptr<P> nout = alloc<P>(cap);
        ran(i, 0, idx)
          nout[i] = out[i];
        ran(i, idx, size)
          nout[i+1] = out[i];
        dealloc(out, size);
        out = nout;
      } else {
        for(int i=size; i>idx; --i)
          out[i] = out[i-1];
      }
      act |= (1<<(c-'a'));
      out[idx] = nxt;
      ++size;
    }
  }
  P split(int new_len) {
    return suf = alloc<Node>(1, act, out, new_len,
        suf, size, cap);
  }
  //!finish
  //!start
  // Extra functions for matching and counting
  P lower(int depth) {
    // move to longest suf of current with a maximum
    // length of depth.
    if (suf->len >= depth) return suf->lower(depth);
    return (P)this;
  }
  P walk(char c, int depth, int &match_len) {
    // move to longest suffix of walked path that is a
    // substring
    match_len = min(match_len, len);
    // includes depth limit(needed for finding matches)
    if (has_nxt(c)) { // as suffixes are in classes,
             // match_len must be tracked externally
      ++match_len;
      return nxt(c)->lower(depth);
    }
    if (suf) return suf->walk(c, depth, match_len);
    return (P)this;
  }
  //!finish
  //!start
  bool vis = false;
  /*ly*/bool vis_t = false;/*ry*/
  /*lp*/int paths_to_end = 0;
  void set_as_end() { // All suffixes of current node are
    paths_to_end += 1;// marked as ending nodes.
    if (suf) suf->set_as_end();
  }
  void calc_paths() {
    /* Call ONCE from ROOT. For each node  calculates
     * number of ways to reach an end node. paths_to_end
     * is ocurence count for any strings in current
     * suffix equivalence class. */
    if (!vis) {
      vis = true;
      ran(i, 0, size){
        out[i]->calc_paths();
        paths_to_end += out[i]->paths_to_end;
      }
    }
  }/*rp*/
  //!pause
#if 0
  //!unpause
  /*ly*/// Transform into suffix tree of reverse string
  P tree_links[26];
  int end_d_v = 1 << 30;
  int end_d() {
    if (end_d_v == 1 << 30) {
      ran(i, 0, size){
        end_d_v = min(end_d_v, 1 + out[i]->end_d());
      }
      if (end_d_v == 1 << 30)
        end_d_v = 0;
    }
    return end_d_v;
  }
  void build_suffix_tree(
    string &s) { // Call ONCE from ROOT.
    if (!vis_t) {
      vis_t = true;
      if (suf)
        suf->tree_links[s[(int)s.size() - end_d() -
                          suf->len - 1]-'a'] = (P)this;
      ran(i, 0, size){
        out[i]->build_suffix_tree(s);
      }
    }
  }/*ry*/
  //!pause
#endif
  //!unpause
};
//!finish
//!start
struct SufAuto {
  P last;
  P root;
  void extend(char new_c) {
    P nlast = alloc<Node>(1, last->len + 1);
    //!pause
    // The equivalence class containing the whole new
    // string
    //!unpause
    P swn = last;
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
      P max_sbstr = swn->nxt(new_c);
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
        P eq_sbstr = max_sbstr->split(swn->len + 1);
        nlast->suf = eq_sbstr;
        //!pause
        // Make suffixes of suf_w_nxt point to eq_sbstr
        // instead of mox_sbstr
        //!unpause
        P x = swn; // x = with_edge_to_eq_sbstr
        while (x != 0 && x->nxt(new_c) == max_sbstr) {
          x->set_nxt(new_c, eq_sbstr);
          x = x->suf;
        }
      }
    }
    last = nlast;
  }
  SufAuto(string &s) {
    last = root = alloc<Node>(1, 0);
    for (char c : s) extend(c);/*ly*/
    // To build suffix tree use reversed string
  //!pause
#if 0
  //!unpause
    root->build_suffix_tree(s);/*ry*/
  //!pause
#endif
  //!unpause
  }
};
//!finish
//!end_codebook

// http://codeforces.com/contest/235/problem/C
int main() {
  init_mem();
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
    P cur = s_suf.root;
    for (int j = 1; j < (int)x.size(); ++j) {
      cur = cur->walk(x[j], len_for_match, match_len);
    }
    unordered_set<P> matches;
    for (char c : x) {
      cur = cur->walk(c, len_for_match, match_len);
      if (match_len >= len_for_match) {
        matches.insert(cur);
      }
    }
    for (P match : matches) {
      res += match->paths_to_end;
    }
    cout << res << '\n';
  }
  return 0;
}
