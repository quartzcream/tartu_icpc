#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const int mod = 1e9 + 7;
#ifndef M_PI
#define M_PI acos(-1.0)
#endif
//!escape \section{Triangle centers}

//!begin_codebook
//!start
const double min_delta = 1e-13;
const double coord_max = 1e6;
typedef complex<double> point;
point A, B, C; // vertixes of the triangle
bool collinear() {
  double min_diff = min(abs(A - B), min(abs(A - C), abs(B - C)));
  if (min_diff < coord_max * min_delta) return true;
  point sp = (B - A) / (C - A);
  double ang =
    M_PI / 2 -
    abs(abs(arg(sp)) - M_PI / 2); // positive angle with the real line
  return ang < min_delta;
}
//!finish
//!start
point circum_center() {
  if (collinear()) return point(NAN, NAN);
  // squared lengths of sides
  double a2 = norm(B - C);
  double b2 = norm(A - C);
  double c2 = norm(A - B);
  // barycentric coordinates of the circumcenter
  double c_A = a2 * (b2 + c2 - a2); // sin(2 * alpha) works also
  double c_B = b2 * (a2 + c2 - b2);
  double c_C = c2 * (a2 + b2 - c2);
  double sum = c_A + c_B + c_C;
  c_A /= sum;
  c_B /= sum;
  c_C /= sum;
  return c_A * A + c_B * B + c_C * C; // cartesian
}
//!finish
//!start
point centroid() { // center of mass
  return (A + B + C) / 3.0;
}
point ortho_center() { // euler line
  point O = circum_center();
  return O + 3.0 * (centroid() - O);
};
point nine_point_circle_center() { // euler line
  point O = circum_center();
  return O + 1.5 * (centroid() - O);
};
//!finish
//!start
point in_center() {
  if (collinear()) return point(NAN, NAN);
  double a = abs(B - C); // side lenghts
  double b = abs(A - C);
  double c = abs(A - B);
  // trilinear coordinates are (1,1,1)
  double sum = a + b + c;
  a /= sum;
  b /= sum;
  c /= sum;                     // barycentric
  return a * A + b * B + c * C; // cartesian
}
//!finish
//!end_codebook

int main() {
  A = 0;
  B = 4;
  C = point(0, 3);
  cout << circum_center() << '\n';
  cout << centroid() << '\n';
  cout << ortho_center() << '\n';
  cout << nine_point_circle_center() << '\n';
  cout << in_center() << '\n';
  return 0;
}
