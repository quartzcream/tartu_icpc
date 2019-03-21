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
const int mod = 1e9 + 7;
#ifndef M_PI
const double M_PI = acos(-1.0);
#endif

//!escape \section{Convex polygon algorithms}

//!begin_codebook
//!start
typedef pair<int, int> Vec;
typedef pair<Vec, Vec> Seg;
typedef vector<Seg>::iterator SegIt;
#define F first
#define S second
#define MP(x, y) make_pair(x, y)

Vec sub(const Vec &v1, const Vec &v2) {
  return MP(v1.F - v2.F, v1.S - v2.S);
}

ll dot(const Vec &v1, const Vec &v2) {
  return (ll)v1.F * v2.F + (ll)v1.S * v2.S;
}

ll cross(const Vec &v1, const Vec &v2) {
  return (ll)v1.F * v2.S - (ll)v2.F * v1.S;
}

ll dist_sq(const Vec &p1, const Vec &p2) {
  return (ll)(p2.F - p1.F) * (p2.F - p1.F) +
         (ll)(p2.S - p1.S) * (p2.S - p1.S);
}
//!finish
//!start
struct Point;
multiset<Point>::iterator end_node;

struct Point {
  Vec p;
  typename multiset<Point>::iterator get_it() const {
    tuple<void *> tmp = {(void *)this - 32}; // gcc rb_tree dependent
    return *(multiset<Point>::iterator *)(&tmp);
  }
  bool operator<(const Point &rhs) const {
    return (p.F < rhs.p.F); // sort by x
  }
  bool operator<(const Vec &q) const {
    auto nxt = next(get_it());
    if (nxt == end_node) return 0; // nxt == end()
    return q.S * dot(p, {q.F, 1}) <
           q.S * dot(nxt->p, {q.F, 1}); // convex hull trick
  }
};
template <int part> // 1 = upper, -1 = lower
struct HullDynamic : public multiset<Point, less<> > {
  bool bad(iterator y) {
    if (y == begin()) return 0;
    auto x = prev(y);
    auto z = next(y);
    if (z == end()) return y->p.F == x->p.F && y->p.S <= x->p.S;
    return part * cross(sub(y->p, x->p), sub(y->p, z->p)) <= 0;
  }
  void insert_point(int m, int b) {// O(log(N))
    auto y = insert({{m, b}});
    if (bad(y)) {
      erase(y);
      return;
    }
    while (next(y) != end() && bad(next(y))) erase(next(y));
    while (y != begin() && bad(prev(y))) erase(prev(y));
  }
  ll eval(int x) {// O(log(N)) upper maximize dot({x, 1}, v)
    end_node = end(); //       lower minimize dot({x, 1}, v)
    auto it = lower_bound((Vec){x, part});
    return (ll)it->p.F * x + it->p.S;
  }
};
//!finish
//!start
struct Hull {
  vector<Seg> hull;
  SegIt up_beg;
  template <typename It>
  void extend(It beg, It end) { // O(n)
    vector<Vec> r;
    for (auto it = beg; it != end; ++it) {
      if (r.empty() || *it != r.back()) {
        while (r.size() >= 2) {
          int n = r.size();
          Vec v1 = {r[n - 1].F - r[n - 2].F, r[n - 1].S - r[n - 2].S};
          Vec v2 = {it->F - r[n - 2].F, it->S - r[n - 2].S};
          if (cross(v1, v2) > 0) break;
          r.pop_back();
        }
        r.push_back(*it);
      }
    }
    ran(i, 0, (int)r.size() - 1) hull.emplace_back(r[i], r[i + 1]);
  }
  Hull(vector<Vec> &vert) {         // atleast 2 distinct points
    sort(vert.begin(), vert.end()); // O(n log(n))
    extend(vert.begin(), vert.end());
    int diff = hull.size();
    extend(vert.rbegin(), vert.rend());
    up_beg = hull.begin() + diff;
  }
  //!finish
  //!start
  bool contains(Vec p) { // O(log(n))
    if (p < hull.front().F || p > up_beg->F) return false;
    {
      auto it_low = lower_bound(
        hull.begin(), up_beg, MP(MP(p.F, (int)-2e9), MP(0, 0)));
      if (it_low != hull.begin()) --it_low;
      Vec a = {it_low->S.F - it_low->F.F, it_low->S.S - it_low->F.S};
      Vec b = {p.F - it_low->F.F, p.S - it_low->F.S};
      if (cross(a, b) < 0) // < 0 is inclusive, <=0 is exclusive
        return false;
    }
    {
      auto it_up = lower_bound(hull.rbegin(),
        hull.rbegin() + (hull.end() - up_beg),
        MP(MP(p.F, (int)2e9), MP(0, 0)));
      if (it_up - hull.rbegin() == hull.end() - up_beg) --it_up;
      Vec a = {it_up->F.F - it_up->S.F, it_up->F.S - it_up->S.S};
      Vec b = {p.F - it_up->S.F, p.S - it_up->S.S};
      if (cross(a, b) > 0) // > 0 is inclusive, >=0 is exclusive
        return false;
    }
    return true;
  }
  //!finish
  //!start
  // The function can have only one local min and max
  // and may be constant only at min and max.
  template <typename T>
  SegIt max(function<T(Seg &)> f) { // O(log(n))
    auto l = hull.begin();
    auto r = hull.end();
    SegIt b = hull.end();
    T b_v;
    while (r - l > 2) {
      auto m = l + (r - l) / 2;
      T l_v = f(*l);
      T l_n_v = f(*(l + 1));
      T m_v = f(*m);
      T m_n_v = f(*(m + 1));
      if (b == hull.end() || l_v > b_v) {
        b = l; // If max is at l we may remove it from the range.
        b_v = l_v;
      }
      if (l_n_v > l_v) {
        if (m_v < l_v) {
          r = m;
        } else {
          if (m_n_v > m_v) {
            l = m + 1;
          } else {
            r = m + 1;
          }
        }
      } else {
        if (m_v < l_v) {
          l = m + 1;
        } else {
          if (m_n_v > m_v) {
            l = m + 1;
          } else {
            r = m + 1;
          }
        }
      }
    }
    T l_v = f(*l);
    if (b == hull.end() || l_v > b_v) {
      b = l;
      b_v = l_v;
    }
    if (r - l > 1) {
      T l_n_v = f(*(l + 1));
      if (b == hull.end() || l_n_v > b_v) {
        b = l + 1;
        b_v = l_n_v;
      }
    }
    return b;
  }
  //!finish
  //!start
  SegIt closest(Vec p) { // p can't be internal(can be on border),
                         // hull must have atleast 3 points
    Seg &ref_p = hull.front(); // O(log(n))
    return max(function<double(Seg &)>(
      [&p, &ref_p](
        Seg &seg) { // accuracy of used type should be coord^-2
        if (p == seg.F) return 10 - M_PI;
        Vec v1 = {seg.S.F - seg.F.F, seg.S.S - seg.F.S};
        Vec v2 = {p.F - seg.F.F, p.S - seg.F.S};
        ll c_p = cross(v1, v2);
        if (c_p > 0) { // order the backside by angle
          Vec v1 = {ref_p.F.F - p.F, ref_p.F.S - p.S};
          Vec v2 = {seg.F.F - p.F, seg.F.S - p.S};
          ll d_p = dot(v1, v2);
          ll c_p = cross(v2, v1);
          return atan2(c_p, d_p) / 2;
        }
        ll d_p = dot(v1, v2);
        double res = atan2(d_p, c_p);
        if (d_p <= 0 && res > 0) res = -M_PI;
        if (res > 0) {
          res += 20;
        } else {
          res = 10 - res;
        }
        return res;
      }));
  }
  //!finish
  //!start
  template <int DIRECTION> // 1 or -1
  Vec tan_point(Vec p) {   // can't be internal or on border
    //-1 iff CCW rotation of ray from p to res takes it away from
    // polygon?
    Seg &ref_p = hull.front(); // O(log(n))
    auto best_seg = max(function<double(Seg &)>(
      [&p, &ref_p](
        Seg &seg) { // accuracy of used type should be coord^-2
        Vec v1 = {ref_p.F.F - p.F, ref_p.F.S - p.S};
        Vec v2 = {seg.F.F - p.F, seg.F.S - p.S};
        ll d_p = dot(v1, v2);
        ll c_p = DIRECTION * cross(v2, v1);
        return atan2(c_p, d_p); // order by signed angle
      }));
    return best_seg->F;
  }
  //!finish
  //!start
  SegIt max_in_dir(Vec v) { // first is the ans. O(log(n))
    return max(
      function<ll(Seg &)>([&v](Seg &seg) { return dot(v, seg.F); }));
  }
  //!finish
  //!start
  pair<SegIt, SegIt> intersections(Seg l) { // O(log(n))
    int x = l.S.F - l.F.F;
    int y = l.S.S - l.F.S;
    Vec dir = {-y, x};
    auto it_max = max_in_dir(dir);
    auto it_min = max_in_dir(MP(y, -x));
    ll opt_val = dot(dir, l.F);
    if (dot(dir, it_max->F) < opt_val ||
        dot(dir, it_min->F) > opt_val)
      return MP(hull.end(), hull.end());
    SegIt it_r1, it_r2;
    function<bool(const Seg &, const Seg &)> inc_c(
      [&dir](const Seg &lft, const Seg &rgt) {
        return dot(dir, lft.F) < dot(dir, rgt.F);
      });
    function<bool(const Seg &, const Seg &)> dec_c(
      [&dir](const Seg &lft, const Seg &rgt) {
        return dot(dir, lft.F) > dot(dir, rgt.F);
      });
    if (it_min <= it_max) {
      it_r1 = upper_bound(it_min, it_max + 1, l, inc_c) - 1;
      if (dot(dir, hull.front().F) >= opt_val) {
        it_r2 = upper_bound(hull.begin(), it_min + 1, l, dec_c) - 1;
      } else {
        it_r2 = upper_bound(it_max, hull.end(), l, dec_c) - 1;
      }
    } else {
      it_r1 = upper_bound(it_max, it_min + 1, l, dec_c) - 1;
      if (dot(dir, hull.front().F) <= opt_val) {
        it_r2 = upper_bound(hull.begin(), it_max + 1, l, inc_c) - 1;
      } else {
        it_r2 = upper_bound(it_min, hull.end(), l, inc_c) - 1;
      }
    }
    return MP(it_r1, it_r2);
  }
  //!finish
  //!start
  Seg diameter() { // O(n)
    Seg res;
    ll dia_sq = 0;
    auto it1 = hull.begin();
    auto it2 = up_beg;
    Vec v1 = {hull.back().S.F - hull.back().F.F,
      hull.back().S.S - hull.back().F.S};
    while (it2 != hull.begin()) {
      Vec v2 = {(it2 - 1)->S.F - (it2 - 1)->F.F,
        (it2 - 1)->S.S - (it2 - 1)->F.S};
      if (cross(v1, v2) > 0) break;
      --it2;
    }
    while (it2 != hull.end()) { // check all antipodal pairs
      if (dist_sq(it1->F, it2->F) > dia_sq) {
        res = {it1->F, it2->F};
        dia_sq = dist_sq(res.F, res.S);
      }
      Vec v1 = {it1->S.F - it1->F.F, it1->S.S - it1->F.S};
      Vec v2 = {it2->S.F - it2->F.F, it2->S.S - it2->F.S};
      if (cross(v1, v2) == 0) {
        if (dist_sq(it1->S, it2->F) > dia_sq) {
          res = {it1->S, it2->F};
          dia_sq = dist_sq(res.F, res.S);
        }
        if (dist_sq(it1->F, it2->S) > dia_sq) {
          res = {it1->F, it2->S};
          dia_sq = dist_sq(res.F, res.S);
        } // report cross pairs at parallel lines.
        ++it1;
        ++it2;
      } else if (cross(v1, v2) < 0) {
        ++it1;
      } else {
        ++it2;
      }
    }
    return res;
  }
};
//!finish
//!end_codebook

bool intersects(Seg line,
  Seg seg) { // inclusive
  auto v1 = make_pair(line.S.F - line.F.F, line.S.S - line.F.S);
  auto v2 = make_pair(seg.F.F - line.F.F, seg.F.S - line.F.S);
  auto v3 = make_pair(seg.S.F - line.F.F, seg.S.S - line.F.S);
  ll val1 = cross(v1, v2);
  ll val2 = cross(v1, v3);
  return !((val1 > 0 && val2 > 0) || (val1 < 0 && val2 < 0));
}

double dist(Vec &p1, Vec &p2) {
  return sqrt((ll)(p2.F - p1.F) * (p2.F - p1.F) +
              (ll)(p2.S - p1.S) * (p2.S - p1.S));
}

void test_dynamic_hull() {
  for (int r = 2; r < 1e9; r *= 2) {
    for (int i = 2; i < 200; ++i) {
      for (int t = 0; t < 100; ++t) {
        bool diff = false;
        vector<Vec> points;
        do {
          points.clear();
          ran(j, 0, i) {
            points.emplace_back(
              rand() % (2 * r) - r, rand() % (2 * r) - r);
          }
          ran(j, 0, i) {
            if (points[j] != points[0]) diff = true;
          }
        } while (!diff);
        Hull hull(points);
        HullDynamic<1> hull_dyn;
        ran(j, 0, i) hull_dyn.insert_point(points[j].F, points[j].S);
        auto it1 = hull_dyn.rbegin();
        auto it2 = hull.up_beg;
        while (it1 != hull_dyn.rend() && it2 != hull.hull.end()) {
          assert(it1->p.F == it2->F.F && it1->p.S == it2->F.S);
          ++it1;
          ++it2;
        }
        assert(it2 == hull.hull.end());
        assert(it1 != hull_dyn.rend());
        it2 = hull.hull.begin();
        assert(it1->p.F == it2->F.F && it1->p.S == it2->F.S);
        ++it1;
        assert(it1 == hull_dyn.rend());
        ran(j, 0, 10) {
          int x = rand() % 20 - 10;
          Vec vec = {x, 1};
          ll best = -(1LL << 60);
          for (auto p : points) {
            best = max(best, dot(vec, p));
          }
          ll cand = hull_dyn.eval(x);
          assert(best == cand);
        }
      }
    }
  }
  for (int r = 2; r < 1e9; r *= 2) {
    for (int i = 2; i < 200; ++i) {
      for (int t = 0; t < 100; ++t) {
        bool diff = false;
        vector<Vec> points;
        do {
          points.clear();
          ran(j, 0, i) {
            points.emplace_back(
              rand() % (2 * r) - r, rand() % (2 * r) - r);
          }
          ran(j, 0, i) {
            if (points[j] != points[0]) diff = true;
          }
        } while (!diff);
        Hull hull(points);
        HullDynamic<-1> hull_dyn;
        ran(j, 0, i) hull_dyn.insert_point(points[j].F, points[j].S);
        auto it1 = hull_dyn.begin();
        auto it2 = hull.hull.begin();
        while (it1 != hull_dyn.end() && it2 != hull.up_beg) {
          assert(it1->p.F == it2->F.F && it1->p.S == it2->F.S);
          ++it1;
          ++it2;
        }
        assert(it2 == hull.up_beg);
        assert(it1 != hull_dyn.end());
        assert(it1->p.F == it2->F.F && it1->p.S == it2->F.S);
        ++it1;
        assert(it1 == hull_dyn.end());
        ran(j, 0, 10) {
          int x = rand() % 20 - 10;
          Vec vec = {x, 1};
          ll best = (1LL << 60);
          for (auto p : points) {
            best = min(best, dot(vec, p));
          }
          ll cand = hull_dyn.eval(x);
          assert(best == cand);
        }
      }
    }
  }
}

void test_closest() {
  for (int r = 1; r < 1e9; r *= 2) {
    for (int i = 2; i < 100; i *= 2) {
      for (int t = 0; t < 100; ++t) {
        vector<Vec> from, query;
        for (int j = 0; j < i; ++j) {
          from.emplace_back(rand() % r, rand() % r);
        }
        for (int j = 0; j < 50; ++j) {
          query.emplace_back(rand() % r, rand() % r);
        }
        Hull hull(from);
        if (!hull.hull.empty()) {
          for (int j = 0; j < query.size(); ++j) {
            if (!hull.contains(query[j])) {
              auto tmp = *hull.closest(query[j]);
              double best_dist = dist(tmp.F, query[j]);
              best_dist = min(best_dist, dist(tmp.S, query[j]));
              auto v1 =
                make_pair(tmp.S.F - tmp.F.F, tmp.S.S - tmp.F.S);
              auto v2 =
                make_pair(query[j].F - tmp.F.F, query[j].S - tmp.F.S);
              auto v3 =
                make_pair(query[j].F - tmp.S.F, query[j].S - tmp.S.S);
              if (dot(v1, v2) > 0 && dot(v1, v3) < 0)
                best_dist = cross(v2, v1) / dist(tmp.S, tmp.F);
              for (int k = 0; k < i; ++k) {
                assert(
                  dist(from[k], query[j]) > (1 - 1e-9) * best_dist);
              }
            }
          }
        }
      }
    }
  }
}

int solve(int n, vector<Vec> hull_points, vector<Vec> query_points) {
  Hull hull(hull_points); // ACM-ICPC 2016 Finals J
  if (hull.hull.size() < 2) {
    int res = n;
    for (auto cur : query_points) {
      if (cur != hull_points[0] || hull_points.size() == 1) {
        --res;
      }
    }
    return res;
  }
  int always_inc = 0;
  vector<Seg> inc_ang_range; //[start, stop]
  if (hull.hull.size() == 2) {
    auto p1 = hull.hull[0].F;
    auto p2 = hull.hull[0].S;
    for (auto cur : query_points) {
      auto v1 = make_pair(p2.F - p1.F, p2.S - p1.S);
      auto v2 = make_pair(cur.F - p1.F, cur.S - p1.S);
      auto v3 = make_pair(cur.F - p2.F, cur.S - p2.S);
      if (dot(v1, v2) >= 0 && dot(v1, v3) <= 0 &&
          cross(v1, v2) == 0) {
        ++always_inc;
      }
    }
    return n - query_points.size() + always_inc;
  }
  for (auto cur : query_points) {
    if (hull.contains(cur)) {
      ++always_inc;
    } else {
      auto forw = hull.tan_point<1>(cur);
      auto back = hull.tan_point<-1>(cur);
      inc_ang_range.emplace_back(
        make_pair(forw.F - cur.F, forw.S - cur.S),
        make_pair(back.F - cur.F, back.S - cur.S));
      if (inc_ang_range.back().F.F < 0) {
        inc_ang_range.back().F.F *= -1;
        inc_ang_range.back().F.S *= -1;
      }
      if (inc_ang_range.back().S.F < 0) {
        inc_ang_range.back().S.F *= -1;
        inc_ang_range.back().S.S *= -1;
      }
    }
  }
  int cur_not_inc = 0;
  vector<pair<Vec, bool> > inc_events;
  for (auto cur : inc_ang_range) {
    if ((cur.F.F == 0 && cur.S.F != 0) ||
        (!(cur.F.F != 0 && cur.S.F == 0) &&
          (ll)cur.S.F * cur.F.S < (ll)cur.F.F * cur.S.S)) {
      ++cur_not_inc;
    }
    inc_events.emplace_back(cur.F, 0);
    inc_events.emplace_back(cur.S, 1);
  }
  sort(inc_events.begin(), inc_events.end(),
    [](const pair<Vec, bool> &lft, const pair<Vec, bool> &rgt) {
      if (lft.F.F == 0 && rgt.F.F != 0) {
        return true;
      }
      if (lft.F.F != 0 && rgt.F.F == 0) {
        return false;
      }
      /*if(lft.F.F == 0 && rgt.F.F == 0 && (lft.F.S < 0) !=
         (rgt.F.S < 0)){ return lft.F.S <
         rgt.F.S;
              }*/
      if ((ll)rgt.F.F * lft.F.S == (ll)lft.F.F * rgt.F.S) {
        return lft.S < rgt.S;
      }
      return (ll)rgt.F.F * lft.F.S < (ll)lft.F.F * rgt.F.S;
    });
  int max_not_inc = 0;
  for (auto cur : inc_events) {
    max_not_inc = max(max_not_inc, cur_not_inc);
    if (!cur.S) {
      --cur_not_inc;
    } else {
      ++cur_not_inc;
    }
  }
  return n - max_not_inc;
}

int solve_brute(
  int n, vector<Vec> hull_points, vector<Vec> query_points) {
  int max_not_inc = 0;
  vector<pair<Vec, int> > all_points;
  for (auto cur : hull_points) {
    all_points.emplace_back(cur, -1);
    if (hull_points.size() > 1) {
      all_points.emplace_back(cur, 1);
    }
  }
  for (auto cur : query_points) {
    all_points.emplace_back(cur, 0);
  }
  for (int i = -100; i < 100; ++i) {
    for (int j = 0; j < 100; ++j) {
      sort(all_points.begin(), all_points.end(),
        [&i, &j](
          const pair<Vec, int> &lft, const pair<Vec, int> &rgt) {
          int lft_val = lft.F.F * i + lft.F.S * j;
          int rgt_val = rgt.F.F * i + rgt.F.S * j;
          if (lft_val == rgt_val) {
            return lft.S < rgt.S;
          }
          return lft_val < rgt_val;
        });
      int cur_not_inc = 0;
      for (auto it = all_points.begin(); it->S == 0; ++it) {
        ++cur_not_inc;
      }
      for (auto it = all_points.rbegin(); it->S == 0; ++it) {
        ++cur_not_inc;
      }
      max_not_inc = max(max_not_inc, cur_not_inc);
    }
  }
  return n - max_not_inc;
}

void test_tan() {
  for (int r = 1; r <= 50; r *= 2) {
    for (int i = 1; i <= 10; ++i) {
      for (int j = 0; j <= 10; ++j) {
        for (int t = 0; t <= 10; ++t) {
          int n = i + j;
          vector<Vec> hull_points;
          for (int k = 0; k < i; ++k) {
            hull_points.emplace_back(rand() % r, rand() % r);
          }
          vector<Vec> query_points;
          for (int k = 0; k < j; ++k) {
            query_points.emplace_back(rand() % r, rand() % r);
          }
          int ans = solve(n, hull_points, query_points);
          int brute_ans = solve_brute(n, hull_points, query_points);
          assert(ans == brute_ans);
        }
      }
    }
  }
}

void test_in_dir() {
  for (int r = 1; r < 1e9; r *= 2) {
    for (int i = 2; i < 1000; i *= 2) {
      for (int t = 0; t < 100; ++t) {
        vector<Vec> from, query;
        for (int j = 0; j < i; ++j) {
          from.emplace_back(rand() % r, rand() % r);
        }
        for (int j = 0; j < 100; ++j) {
          query.emplace_back(rand() % r, rand() % r);
        }
        Hull hull(from);
        if (!hull.hull.empty()) {
          for (int j = 0; j < query.size(); ++j) {
            auto best = hull.max_in_dir(query[j])->F;
            for (auto cur : from) {
              assert(dot(best, query[j]) >= dot(cur, query[j]));
            }
          }
        }
      }
    }
  }
}

void test_dia() {
  for (int r = 1; r < 1e9; r *= 2) {
    for (int i = 2; i < 1000; i *= 2) {
      for (int t = 0; t < 100; ++t) {
        vector<Vec> from;
        for (int j = 0; j < i; ++j) {
          from.emplace_back(rand() % r, rand() % r);
        }
        Hull hull(from);
        if (!hull.hull.empty()) {
          auto best = hull.diameter();
          ll dia_sq = dist_sq(best.F, best.S);
          for (auto p1 : from) {
            for (auto p2 : from) {
              assert(dia_sq >= dist_sq(p1, p2));
            }
          }
        }
      }
    }
  }
}

void test_intersects() {
  for (int r = 1; r < 1e9; r *= 2) {
    for (int i = 2; i < 1000; i *= 2) {
      for (int t = 0; t < 100; ++t) {
        vector<Vec> from, query1, query2;
        for (int j = 0; j < i; ++j) {
          from.emplace_back(rand() % r, rand() % r);
        }
        for (int j = 0; j < 100; ++j) {
          query1.emplace_back(rand() % r, rand() % r);
          query2.emplace_back(rand() % r, rand() % r);
        }
        Hull hull(from);
        if (!hull.hull.empty()) {
          for (int j = 0; j < query1.size(); ++j) {
            auto best =
              hull.intersections(make_pair(query1[j], query2[j]));
            if (best.F == hull.hull.end()) {
              for (auto cur : hull.hull) {
                assert(
                  !intersects(make_pair(query1[j], query2[j]), cur));
              }
            } else {
              assert(
                intersects(make_pair(query1[j], query2[j]), *best.F));
              assert(
                intersects(make_pair(query1[j], query2[j]), *best.S));
            }
          }
        }
      }
    }
  }
}

int main() {
  int seed;
  scanf("%d", &seed);
  srand(seed);
  vector<Vec> points;
  int cord_lim = sqrt(2e9) - 1;
  printf("%d\n", cord_lim);
  int cord_offset = 1e9;
  HullDynamic<1> hull_dyn;
  const int lim = 1e5;
  ran(i, 0, lim) {
    int x = rand() % (2 * cord_lim) - cord_lim;
    hull_dyn.insert_point(x, cord_offset - x * x - (rand() & 1));
  }
  ll res = rand();
  ran(i, 0, lim) {
    int x = rand() % (2 * cord_lim) - cord_lim;
    res ^= hull_dyn.eval(x);
  }
  printf("%lld", res);
  test_dynamic_hull();
  test_closest();
  test_tan();
  test_in_dir();
  test_dia();
  test_intersects();
  return 0;
  int n;
  cin >> n;
  vector<Vec> hull_points;
  vector<Vec> query_points;
  for (int i = 0; i < n; ++i) {
    int a, b, c;
    scanf("%d %d %d", &a, &b, &c);
    if (c) {
      hull_points.emplace_back(a, b);
    } else {
      query_points.emplace_back(a, b);
    }
  }
  cout << solve(n, hull_points, query_points) << endl;
  return 0;
}
