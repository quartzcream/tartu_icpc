//----------------------------------------------------------------
//OMLBEGIN: 2D Vectors
#include<vector>
#include<cmath>
#include<iostream>
using namespace std;

//!escape \section{2D line segment}
//!begin_codebook
const long double PI = acos(-1.0L);

struct Vec {
	long double x, y;
	
	Vec& operator-=(Vec r) {
		x -= r.x, y -= r.y;
		return *this;
	}
	Vec operator-(Vec r) {return Vec(*this) -= r;}
	
	Vec& operator+=(Vec r) {
		x += r.x, y += r.y;
		return *this;
	}
	Vec operator+(Vec r) {return Vec(*this) += r;}
	Vec operator-() {return {-x, -y};}
	Vec& operator*=(long double r) {
		x *= r, y *= r;
		return *this;
	}
	Vec operator*(long double r) {return Vec(*this) *= r;}
	Vec& operator/=(long double r) {
		x /= r, y /= r;
		return *this;
	}
	Vec operator/(long double r) {return Vec(*this) /= r;}
	
	long double operator*(Vec r) {
		return x * r.x + y * r.y;
	}
};
ostream& operator<<(ostream& l, Vec r) {
	return l << '(' << r.x << ", " << r.y << ')';
}
long double len(Vec a) {
	return hypot(a.x, a.y);
}
long double cross(Vec l, Vec r) {
	return l.x * r.y - l.y * r.x;
}
long double angle(Vec a) {
	return fmod(atan2(a.y, a.x)+2*PI, 2*PI);
}
Vec normal(Vec a) {
	return Vec({-a.y, a.x}) / len(a);
}
//!end_codebook
//OMLEND: 2D Vectors
//----------------------------------------------------------------
