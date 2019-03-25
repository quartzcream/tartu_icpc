/**
 * Author: David Rydh, Per Austrin
 * Date: 2003-03-16
 * Description:
 */
#pragma once

//!escape Polynomial roots and O(n^2) interpolation
//!begin_codebook
//!start
struct Poly {
  vector<double> a;
  double operator()(double x) const {
    double val = 0;
    for (int i = (int)a.size(); i--;) (val *= x) += a[i];
    return val;
  }
  void diff() {
    ran(i, 1, (int)a.size()) a[i - 1] = i * a[i];
    a.pop_back();
  }
  void divroot(double x0) {
    double b = a.back(), c;
    a.back() = 0;
    for (int i = (int)a.size() - 1; i--;)
      c = a[i], a[i] = a[i + 1] * x0 + b, b = c;
    a.pop_back();
  }
};
//!finish
//!end_codebook
