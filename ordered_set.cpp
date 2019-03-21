//!escape \section{gcc ordered set, hashtable}

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
//!start
#include <bits/extc++.h>
using namespace __gnu_pbds;

template <typename T, typename U>
using hashmap = gp_hash_table<T, U>;

template <typename T>
using ordered_set = tree<T, null_type, less<T>, rb_tree_tag,
  tree_order_statistics_node_update>;

int main() {
  ordered_set<int> cur;
  cur.insert(1);
  cur.insert(3);
  cout << cur.order_of_key(2)
       << endl; // the number of elements in the set less than 2
  cout << *cur.find_by_order(0)
       << endl; // the 0-th smallest number in the set(0-based)
  cout << *cur.find_by_order(1)
       << endl; // the 1-th smallest number in the set(0-based)

  hashmap<int, int> h({},{},{},{}, {1 << 16});
}
//!finish
//!end_codebook
