#include <bits/stdc++.h>
using namespace std;
typedef long long               ll;
const int mod = 1e9 + 7;
#ifndef M_PI
#define M_PI acos(-1.0)
#endif
//!escape \section{Triangle centers}

//!begin_codebook
const double min_delta = 1e-13;
const double coord_max = 1e6;
typedef complex < double > point;
point A, B, C; // vertixes of the triangle
bool collinear(){
	double min_diff = min(abs(A - B), min(abs(A - C), abs(B - C)));
	if(min_diff < coord_max * min_delta)
		return true;
	point sp = (B - A) / (C - A);
	double ang = M_PI/2-abs(abs(arg(sp))-M_PI/2); //positive angle with the real line
	return ang < min_delta;
}
point circum_center(){
	if(collinear())
		return point(NAN,NAN);
	//squared lengths of sides
	double a2, b2, c2;
	a2 = norm(B - C);
	b2 = norm(A - C);
	c2 = norm(A - B);
	//barycentric coordinates of the circumcenter
	double c_A, c_B, c_C;
	c_A = a2 * (b2 + c2 - a2);//sin(2 * alpha) may be used as well
	c_B = b2 * (a2 + c2 - b2);
	c_C = c2 * (a2 + b2 - c2);
	double sum = c_A + c_B + c_C;
	c_A /= sum;
	c_B /= sum;
	c_C /= sum;
	// cartesian coordinates of the circumcenter
	return c_A * A + c_B * B + c_C * C;
}
point centroid(){ //center of mass
	return (A + B + C) / 3.0;
}
point ortho_center(){ //euler line
	point O = circum_center();
	return O + 3.0 * (centroid() - O);
};
point nine_point_circle_center(){ //euler line
	point O = circum_center();
	return O + 1.5 * (centroid() - O);
};
point in_center(){
	if(collinear())
		return point(NAN,NAN);
	double a, b, c; //side lengths
	a = abs(B - C);
	b = abs(A - C);
	c = abs(A - B);
	//trilinear coordinates are (1,1,1)
	//barycentric coordinates
	double c_A = a, c_B = b, c_C = c;
	double sum = c_A + c_B + c_C;
	c_A /= sum;
	c_B /= sum;
	c_C /= sum;
	// cartesian coordinates of the incenter
	return c_A * A + c_B * B + c_C * C;
}
//!end_codebook

int main(){
	A = 0;
	B = 4;
	C = point(0,3);
	cout<<circum_center()<<'\n';
	cout<<centroid()<<'\n';
	cout<<ortho_center()<<'\n';
	cout<<nine_point_circle_center()<<'\n';
	cout<<in_center()<<'\n';
	return 0;
}
