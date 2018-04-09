//!escape \section{gcc ordered set}

//!begin_codebook
//!start
#include<bits/stdc++.h>
typedef long long   ll;
using namespace std;

#include<ext/pb\_ds/assoc\_container.hpp>
#include<ext/pb\_ds/tree\_policy.hpp>
using namespace __gnu_pbds;
template <typename T>
using ordered_set = tree<T, null_type, less<T>, rb_tree_tag, tree_order_statistics_node_update>;
int main(){
	ordered_set<int>  cur;
	cur.insert(1); 
	cur.insert(3);
	cout << cur.order_of_key(2) << endl; // the number of elements in the set less than 2
	cout << *cur.find_by_order(0) << endl; // the 0-th smallest number in the set(0-based)
	cout << *cur.find_by_order(1) << endl; // the 1-th smallest number in the set(0-based)
}
//!finish
//!end_codebook
