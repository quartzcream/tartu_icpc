/**
 * Author: Per Austrin
 * Date: 2004-02-08
 * License: CC0
 */
//!begin_codebook
/* Description: Finds the real roots to a polynomial.
 * Usage: poly_roots({{2,-3,1}},-1e9,1e9) // solve
 * x^2-3x+2 = 0 Time: O(n^2 \log(1/\epsilon)) */
//!pause
#pragma once

#include "Polynomial.h"
//!unpause

//!start
vector<double> poly_roots(
  Poly p, double xmin, double xmax) {
  if (sz(p.a) == 2) return {-p.a[0] / p.a[1]};
  vector<double> ret;
  Poly der = p;
  der.diff();
  auto dr = poly_roots(der, xmin, xmax);
  dr.push_back(xmin - 1);
  dr.push_back(xmax + 1);
  sort(dr.begin(), dr.end());
  ran(i, 0, (int)dr.size() - 1) {
    double l = dr[i], h = dr[i + 1];
    bool sign = p(l) > 0;
    if (sign ^ (p(h) > 0)) {
      ran(it, 0, 60) { // while (h - l > 1e-8)
        double m = (l + h) / 2, f = p(m);
        if ((f <= 0) ^ sign) {
          l = m;
        } else {
          h = m;
        }
      }
      ret.push_back((l + h) / 2);
    }
  }
  return ret;
}
//!finish
//!end_codebook
