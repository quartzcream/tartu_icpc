//----------------------------------------------------------------
//OMLBEGIN: Modular Integers
#include<utility>
using namespace std;


//!escape \section{MOD int, extended Euctlidean}
//!begin_codebook
pair<int, int> extendedEuclideanAlgorithm(int a, int b) {
	if(b == 0)
		return make_pair(1, 0);
	pair<int, int> ret = extendedEuclideanAlgorithm(b, a%b);
	return {ret.second, ret.first - a/b * ret.second};
}


struct Modint {
	static const int MOD = 1000000007;
	int val;
	
	Modint(int nval = 0) {
		val = nval;
	}
	
	Modint& operator+=(Modint r) {
		val = (val + r.val) % MOD;
		return *this;
	}
	Modint operator+(Modint r) {return Modint(*this) += r;}
	
	Modint& operator-=(Modint r) {
		val = (val + MOD - r.val) % MOD;
		return *this;
	}
	Modint operator-(Modint r) {return Modint(*this) -= r;}
	
	Modint& operator*=(Modint r) {
		val = 1LL * val * r.val % MOD;
		return *this;
	}
	Modint operator*(Modint r) {return Modint(*this) *= r;}
	
	Modint inverse() {
		int ret = extendedEuclideanAlgorithm(val, MOD).first;
		if(ret < 0)
			ret += MOD;
		return ret;
	}
	
	Modint& operator/=(Modint r) {
		return operator*=(r.inverse() );
	}
	Modint operator/(Modint r) {return Modint(*this) /= r;}
};
//!end_codebook


//OMLEND: Modular Integers
//----------------------------------------------------------------
