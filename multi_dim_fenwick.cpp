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

//!escape Templated multi dimensional BIT O(log(n)^d) per query

//!begin_codebook
//!start
// Fully overloaded any dimensional BIT, use any type for
// coordinates, elements, return_value. Includes
// coordinate compression.
template <class E_T, class C_T, C_T n_inf, class R_T>
struct BIT {
  vector<C_T> pos;
  vector<E_T> elems;
  bool act = false;

  BIT() { pos.push_back(n_inf); }
  void init() {
    if (act) {
      for (E_T &c_elem : elems) c_elem.init();
    } else {
      act = true;
      sort(pos.begin(), pos.end());
      pos.resize(
        unique(pos.begin(), pos.end()) - pos.begin());
      elems.resize(pos.size());
    }
  }
  template <typename... loc_form>
  void update(C_T cx, loc_form... args) {
    if (act) {
      int x = lower_bound(pos.begin(), pos.end(), cx) -
              pos.begin();
      for (; x < (int)pos.size(); x += x & -x)
        elems[x].update(args...);
    } else {
      pos.push_back(cx);
    }
  }
  template <typename... loc_form>
  R_T query(
    C_T cx, loc_form... args) { // sum in (-inf, cx)
    R_T res = 0;
    int x = lower_bound(pos.begin(), pos.end(), cx) -
            pos.begin() - 1;
    for (; x > 0; x -= x & -x)
      res += elems[x].query(args...);
    return res;
  }
};

template <typename I_T>
struct wrapped {
  I_T a = 0;
  void update(I_T b) { a += b; }
  I_T query() { return a; }
  // Should never be called, needed for compilation
  void init() { DEBUG('i') }
  void update() { DEBUG('u') }
};

//!finish
int main() {
  // retun type should be same as type inside wrapped
  BIT<BIT<wrapped<ll>, int, INT_MIN, ll>, int, INT_MIN,
    ll>
    fenwick;
  int dim = 2;
  vector<tuple<int, int, ll> > to_insert;
  to_insert.emplace_back(1, 1, 1);
  // set up all pos that are to be used for update
  for (int i = 0; i < dim; ++i) {
    for (auto &cur : to_insert)
      fenwick.update(get<0>(cur), get<1>(cur));
    // May include value which won't be used
    fenwick.init();
  }
  // actual use
  for (auto &cur : to_insert)
    fenwick.update(
      get<0>(cur), get<1>(cur), get<2>(cur));
  cout << fenwick.query(2, 2) << '\n';
}
//!end_codebook
//
