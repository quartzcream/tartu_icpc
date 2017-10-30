//----------------------------------------------------------------
//OMLBEGIN: Polynomials
#include<complex>
#include<vector>
#include<cmath>
#include<iostream>
using namespace std;

//!escape \section{FFT $\mathcal{O}(n\log(n))$}
//!begin_codebook
//Assumes a is a power of two
vector<complex<long double> > fastFourierTransform(vector<complex<long double> > a, bool inverse) {
	const long double PI = acos(-1.0L);
	int n = a.size();
	//Precalculate w
	vector<complex<long double> > w(n, 0.0L);
	w[0] = 1;
	for(int tpow = 1; tpow < n; tpow *= 2)
		w[tpow] = polar(1.0L, 2*PI * tpow/n * (inverse ? -1 : 1) );
	for(int i=3, last = 2;i<n;i++) {
		if(w[i] == 0.0L)
			w[i] = w[last] * w[i-last];
		else
			last = i;
	}
	
	//Rearrange a
	for(int block = n; block > 1; block /= 2) {
		int half = block/2;
		vector<complex<long double> > na(n);
		for(int s=0; s < n; s += block)
			for(int i=0;i<block;i++)
				na[s + half*(i%2) + i/2] = a[s+i];
		a = na;
	}
	
	//Now do the calculation
	for(int block = 2; block <= n; block *= 2) {
		vector<complex<long double> > na(n);
		int wb = n/block, half = block/2;
		
		for(int s=0; s < n; s += block)
			for(int i=0;i<half; i++) {
				na[s+i] = a[s+i] + w[wb*i] * a[s+half+i];
				na[s+half+i] = a[s+i] - w[wb*i] * a[s+half+i];
			}
		a = na;
	}
	
	return a;
}


struct Polynomial {
	vector<long double> a;
	
	long double& operator[](int ind) {
		return a[ind];
	}
	
	Polynomial& operator*=(long double r) {
		for(auto &c : a)
			c *= r;
		return *this;
	}
	Polynomial operator*(long double r) {return Polynomial(*this) *= r;}
	
	Polynomial& operator/=(long double r) {
		for(auto &c : a)
			c /= r;
		return *this;
	}
	Polynomial operator/(long double r) {return Polynomial(*this) /= r;}
	
	Polynomial& operator+=(Polynomial r) {
		if(a.size() < r.a.size())
			a.resize(r.a.size(), 0.0L);
		for(int i=0;i<(int)r.a.size();i++)
			a[i] += r[i];
		return *this;
	}
	Polynomial operator+(Polynomial r) {return Polynomial(*this) += r;}
	
	Polynomial& operator-=(Polynomial r) {
		if(a.size() < r.a.size())
			a.resize(r.a.size(), 0.0L);
		for(int i=0;i<(int)r.a.size();i++)
			a[i] -= r[i];
		return *this;
	}
	Polynomial operator-(Polynomial r) {return Polynomial(*this) -= r;}
	
	Polynomial operator*(Polynomial r) {
		int n = 1;
		while(n < (int)(a.size() + r.a.size() - 1) )
			n *= 2;
		
		vector<complex<long double> > fl(n, 0.0L), fr(n, 0.0L);
		for(int i=0;i<(int)a.size();i++)
			fl[i] = a[i];
		for(int i=0;i<(int)r.a.size();i++)
			fr[i] = r[i];
		
		fl = fastFourierTransform(fl, false);
		fr = fastFourierTransform(fr, false);
		
		vector<complex<long double> > ret(n);
		for(int i=0;i<n;i++)
			ret[i] = fl[i] * fr[i];
		ret = fastFourierTransform(ret, true);
		
		Polynomial result;
		result.a.resize(a.size() + r.a.size() - 1);
		for(int i=0;i<(int)result.a.size();i++)
			result[i] = ret[i].real() / n;
		return result;
	}
};
//!end_codebook

ostream& operator<<(ostream& out, Polynomial f) {
	for(int i=f.a.size()-1; i >= 0; i--) {
		out << f[i] << "*x_" << i;
		if(i > 0) out << " + ";
	}
	return out;
}


//OMLEND: Polynomials
//----------------------------------------------------------------
