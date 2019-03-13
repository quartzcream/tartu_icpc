//----------------------------------------------------------------
// OMLBEGIN: 2D Segments
// Uses: vector2d.h
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

//----------------------------------------------------------------
// OMLBEGIN: 2D Vectors

const double PI = acos(-1.0L);

struct Vec {
  double x, y;

  Vec& operator-=(Vec r) {
    x -= r.x, y -= r.y;
    return *this;
  }
  Vec operator-(Vec r) { return Vec(*this) -= r; }

  Vec& operator+=(Vec r) {
    x += r.x, y += r.y;
    return *this;
  }
  Vec operator+(Vec r) { return Vec(*this) += r; }
  Vec operator-() { return {-x, -y}; }
  Vec& operator*=(double r) {
    x *= r, y *= r;
    return *this;
  }
  Vec operator*(double r) { return Vec(*this) *= r; }
  Vec& operator/=(double r) {
    x /= r, y /= r;
    return *this;
  }
  Vec operator/(double r) { return Vec(*this) /= r; }

  double operator*(Vec r) { return x * r.x + y * r.y; }
};
ostream& operator<<(ostream& l, Vec r) {
  return l << '(' << r.x << ", " << r.y << ')';
}
double len(Vec a) { return hypot(a.x, a.y); }
double cross(Vec l, Vec r) { return l.x * r.y - l.y * r.x; }
double angle(Vec a) {
  return fmod(atan2(a.y, a.x) + 2 * PI, 2 * PI);
}
Vec normal(Vec a) { return Vec({-a.y, a.x}) / len(a); }
// OMLEND: 2D Vectors
//----------------------------------------------------------------
//!escape \section{Seg-Seg intersection, halfplane intersection area}
//!begin_codebook
//!start
struct Seg {
  Vec a, b;
  Vec d() { return b - a; }
};

Vec intersection(Seg l, Seg r) {
  Vec dl = l.d(), dr = r.d();
  if (cross(dl, dr) == 0) return {nanl(""), nanl("")};

  double h = cross(dr, l.a - r.a) / len(dr);
  double dh = cross(dr, dl) / len(dr);

  return l.a + dl * (h / -dh);
}

// Returns the area bounded by halfplanes
double calc_area(const vector<Seg>& lines) {
  double lb = -HUGE_VAL, ub = HUGE_VAL;

  vector<Seg> slines[2];
  for (auto line : lines) {
    if (line.b.y == line.a.y) {
      if (line.a.x < line.b.x) {
        lb = max(lb, line.a.y);
      } else {
        ub = min(ub, line.a.y);
      }
    } else if (line.a.y < line.b.y) {
      slines[1].push_back(line);
    } else {
      slines[0].push_back({line.b, line.a});
    }
  }

  ran(i, 0, 2) {
    sort(slines[i].begin(), slines[i].end(), [&](Seg l, Seg r) {
      if (cross(l.d(), r.d()) == 0)
        return normal(l.d()) * l.a > normal(r.d()) * r.a;
      return (1 - 2 * i) * cross(l.d(), r.d()) < 0;
    });
  }

  // Now find the application area of the lines and clean up redundant
  // ones
  vector<double> ap_s[2];
  ran(side, 0, 2) {
    vector<double>& apply = ap_s[side];
    vector<Seg> clines;

    for (auto line : slines[side]) {
      while (clines.size() > 0) {
        Seg other = clines.back();

        if (cross(line.d(), other.d()) != 0) {
          double start = intersection(line, other).y;
          if (start > apply.back()) break;
        }

        clines.pop_back();
        apply.pop_back();
      }

      if (clines.size() == 0) {
        apply.push_back(-HUGE_VAL);
      } else {
        apply.push_back(intersection(line, clines.back()).y);
      }
      clines.push_back(line);
    }

    slines[side] = clines;
  }
  ap_s[0].push_back(HUGE_VALL);
  ap_s[1].push_back(HUGE_VALL);

  double result = 0;
  {
    double lb = -HUGE_VALL, ub;
    for (int i = 0, j = 0;
         i < (int)slines[0].size() && j < (int)slines[1].size();
         lb = ub) {
      ub = min(ap_s[0][i + 1], ap_s[1][j + 1]);

      double alb = lb, aub = ub;
      Seg l[2] = {slines[0][i], slines[1][j]};

      if (cross(l[1].d(), l[0].d()) > 0) {
        alb = max(alb, intersection(l[0], l[1]).y);
      } else if (cross(l[1].d(), l[0].d()) < 0) {
        aub = min(aub, intersection(l[0], l[1]).y);
      }
      alb = max(alb, lb);
      aub = min(aub, ub);
      aub = max(aub, alb);

      ran(k, 0, 2) {
        double x1 =
          l[0].a.x + (alb - l[0].a.y) / l[0].d().y * l[0].d().x;
        double x2 =
          l[0].a.x + (aub - l[0].a.y) / l[0].d().y * l[0].d().x;
        result += (-1 + 2 * k) * (aub - alb) * (x1 + x2) / 2;
      }

      if (ap_s[0][i + 1] < ap_s[1][j + 1]) {
        i++;
      } else {
        j++;
      }
    }
  }
  return result;
}
//!finish
//!end_codebook
// OMLEND: 2D Segments
//----------------------------------------------------------------
