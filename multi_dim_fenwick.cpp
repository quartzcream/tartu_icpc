#include <bits/stdc++.h>

using namespace std;

typedef long long ll;
typedef unsigned int uint;
typedef unsigned long long ull;

const int mod = 1e9 + 7;
const int nmax = 1e5 + 5;

//!escape \section{Templated multi dimensional BIT $\mathcal{O}(\log(n)^\text{dim})$}

//!begin_codebook
// Fully overloaded any dimensional BIT, use any type for coordinates, elements, return_value.
// Includes coordinate compression.
template < typename elem_t, typename coord_t, coord_t n_inf, typename ret_t >
class BIT {
  vector< coord_t > positions;
  vector< elem_t > elems;
  bool initiated = false;

 public:
  BIT() {
		positions.push_back(n_inf);
	}
  void initiate() {
    if (initiated) {
      for (elem_t &c_elem : elems)
				c_elem.initiate();
    } else {
      initiated = true;
      sort(positions.begin(), positions.end());
      positions.resize(unique(positions.begin(), positions.end()) - positions.begin());
      elems.resize(positions.size());
    }
  }
  template < typename... loc_form >
  void update(coord_t cord, loc_form... args) {
    if (initiated) {
      int pos = lower_bound(positions.begin(), positions.end(), cord) - positions.begin();
      for (; pos < positions.size(); pos += pos & -pos)
				elems[pos].update(args...);
    } else {
      positions.push_back(cord);
    }
  }
  template < typename... loc_form >
  ret_t query(coord_t cord, loc_form... args) { //sum in open interval (-inf, cord)
    ret_t res = 0;
    int pos = (lower_bound(positions.begin(), positions.end(), cord) - positions.begin())-1;
    for (; pos > 0; pos -= pos & -pos)
			res += elems[pos].query(args...);
    return res;
  }
};
template < typename internal_type >
struct wrapped {
  internal_type a = 0;
  void update(internal_type b) {
		a += b;
	}
  internal_type query() {
		return a;
	}
  // Should never be called, needed for compilation
  void initiate() {
		cerr << 'i' << endl; 
	}
  void update() { 
		cerr << 'u' << endl;
	}
};
int main() {
  // retun type should be same as type inside wrapped
  BIT< BIT< wrapped< ll >, int, INT_MIN, ll >, int, INT_MIN, ll > fenwick;
  int dim = 2;
  vector< tuple< int, int, ll > > to_insert;
	to_insert.emplace_back(1, 1, 1);
  // set up all positions that are to be used for update
  for (int i = 0; i < dim; ++i) {
    for (auto &cur : to_insert)
      fenwick.update(get< 0 >(cur), get< 1 >(cur));  // May include value which won't be used
    fenwick.initiate();
  }
  // actual use
  for (auto &cur : to_insert) 
		fenwick.update(get< 0 >(cur), get< 1 >(cur), get< 2 >(cur));
  cout << fenwick.query(2, 2)<<'\n';
}
//!end_codebook
//
