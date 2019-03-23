//!escape gcc ordered set, hashtable

//!begin_codebook
//!start
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
//!finish
#pragma GCC optimize("Ofast") // better vectorization
#pragma GCC target("avx,avx2")
// double vectorized performance
//!start
#include <bits/extc++.h>
using namespace __gnu_pbds;

template <typename T, typename U>
using hashmap = gp_hash_table<T, U>;
// dumb, 3x faster than stl

template <typename T>
using ordered_set = tree<T, null_type, less<T>,
  rb_tree_tag, tree_order_statistics_node_update>;
//!finish

int main() {
  ordered_set<int> cur;
  cur.insert(1);
  cur.insert(3);
  cout << cur.order_of_key(2) << endl;
  // the number of elements in the set less than 2
  cout << *cur.find_by_order(1) << endl;
  // the 1-st smallest number in the set(0-based)

  ordered_set<int> oth;
  oth.insert(5); // to join: cur < oth
  cur.join(oth); // cur = {1, 3, 5}, oth = {}
  //!end_codebook
  for (auto x : cur) {
    cout << x << ' ';
  }
  cout << endl;
  for (auto x : oth) {
    cout << x << ' ';
  }
  cout << endl;
  cout << endl;
  //!begin_codebook
  cur.split(1, oth); // cur = {1}, oth = {3, 5}
  //!end_codebook
  for (auto x : cur) {
    cout << x << ' ';
  }
  cout << endl;
  for (auto x : oth) {
    cout << x << ' ';
  }
  cout << endl;
  cout << endl;
  //!begin_codebook
  hashmap<int, int> h({}, {}, {}, {}, {1 << 16});
  //!end_codebook
  h[1] = 1;
  h[2] = 2;
  h[3] = 3;
  h[4] = 4;
  h[5] = 5;
  for (auto elem : h) {
    cout << elem.first << ' ' << elem.second << '\n';
  }
  //!begin_codebook
}
//!end_codebook
