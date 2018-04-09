#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
#ifndef M_PI
const double M_PI = acos(-1.0);
#endif

//!escape \section{Convex polygon algorithms}

//!begin_codebook
//!start
ll dot(const pair< int, int > &v1, const pair< int, int > &v2) {
  return (ll)v1.first * v2.first + (ll)v1.second * v2.second;
}

ll cross(const pair< int, int > &v1, const pair< int, int > &v2) {
  return (ll)v1.first * v2.second - (ll)v2.first * v1.second;
}

ll dist_sq(const pair< int, int > &p1, const pair< int, int > &p2) {
  return (ll)(p2.first - p1.first) * (p2.first - p1.first) +
         (ll)(p2.second - p1.second) * (p2.second - p1.second);
}
//!finish

//!start
struct Hull {
  vector< pair< pair< int, int >, pair< int, int > > > hull;
  vector< pair< pair< int, int >, pair< int, int > > >::iterator upper_begin;
  template < typename Iterator >
  void extend_hull(Iterator begin, Iterator end) {  // O(n)
    vector< pair< int, int > > res;
    for (auto it = begin; it != end; ++it) {
      if (res.empty() || *it != res.back()) {
        while (res.size() >= 2) {
          auto v1 = make_pair(res[res.size() - 1].first - res[res.size() - 2].first,
                              res[res.size() - 1].second - res[res.size() - 2].second);
          auto v2 = make_pair(it->first - res[res.size() - 2].first,
                              it->second - res[res.size() - 2].second);
          if (cross(v1, v2) > 0)
            break;
          res.pop_back();
        }
        res.push_back(*it);
      }
    }
    for (int i = 0; i < res.size() - 1; ++i)
      hull.emplace_back(res[i], res[i + 1]);
  }
  Hull(vector< pair< int, int > > &vert) {  // atleast 2 distinct points
    sort(vert.begin(), vert.end());         // O(n log(n))
    extend_hull(vert.begin(), vert.end());
    int diff = hull.size();
    extend_hull(vert.rbegin(), vert.rend());
    upper_begin = hull.begin() + diff;
  }
//!finish
//!start
  bool contains(pair< int, int > p) {  // O(log(n))
    if (p < hull.front().first || p > upper_begin->first) return false;
    {
      auto it_low = lower_bound(hull.begin(), upper_begin,
                                make_pair(make_pair(p.first, (int)-2e9), make_pair(0, 0)));
      if (it_low != hull.begin())
        --it_low;
      auto v1 = make_pair(it_low->second.first - it_low->first.first,
                          it_low->second.second - it_low->first.second);
      auto v2 = make_pair(p.first - it_low->first.first, p.second - it_low->first.second);
      if (cross(v1, v2) < 0)  // < 0 is inclusive, <=0 is exclusive
        return false;
    }
    {
      auto it_up = lower_bound(hull.rbegin(), hull.rbegin() + (hull.end() - upper_begin),
                               make_pair(make_pair(p.first, (int)2e9), make_pair(0, 0)));
      if (it_up - hull.rbegin() == hull.end() - upper_begin)
        --it_up;
      auto v1 = make_pair(it_up->first.first - it_up->second.first,
                          it_up->first.second - it_up->second.second);
      auto v2 = make_pair(p.first - it_up->second.first, p.second - it_up->second.second);
      if (cross(v1, v2) > 0)  // > 0 is inclusive, >=0 is exclusive
        return false;
    }
    return true;
  }
//!finish
//!start
  template < typename T >  // The function can have only one local min and max and may be constant
                           // only at min and max.
  vector< pair< pair< int, int >, pair< int, int > > >::iterator max(
      function< T(const pair< pair< int, int >, pair< int, int > > &) > f) {  // O(log(n))
    auto l = hull.begin();
    auto r = hull.end();
    vector< pair< pair< int, int >, pair< int, int > > >::iterator best = hull.end();
    T best_val;
    while (r - l > 2) {
      auto mid = l + (r - l) / 2;
      T l_val = f(*l);
      T l_nxt_val = f(*(l + 1));
      T mid_val = f(*mid);
      T mid_nxt_val = f(*(mid + 1));
      if (best == hull.end() ||
          l_val > best_val) {  // If max is at l we may remove it from the range.
        best = l;
        best_val = l_val;
      }
      if (l_nxt_val > l_val) {
        if (mid_val < l_val) {
          r = mid;
        } else {
          if (mid_nxt_val > mid_val) {
            l = mid + 1;
          } else {
            r = mid + 1;
          }
        }
      } else {
        if (mid_val < l_val) {
          l = mid + 1;
        } else {
          if (mid_nxt_val > mid_val) {
            l = mid + 1;
          } else {
            r = mid + 1;
          }
        }
      }
    }
    T l_val = f(*l);
    if (best == hull.end() || l_val > best_val) {
      best = l;
      best_val = l_val;
    }
    if (r - l > 1) {
      T l_nxt_val = f(*(l + 1));
      if (best == hull.end() || l_nxt_val > best_val) {
        best = l + 1;
        best_val = l_nxt_val;
      }
    }
    return best;
  }
//!finish
//!start
  vector< pair< pair< int, int >, pair< int, int > > >::iterator closest(
      pair< int, int >
          p) {  // p can't be internal(can be on border), hull must have atleast 3 points
    const pair< pair< int, int >, pair< int, int > > &ref_p = hull.front();  // O(log(n))
    return max(function< double(const pair< pair< int, int >, pair< int, int > > &) >(
        [&p, &ref_p](const pair< pair< int, int >, pair< int, int > >
                         &seg) {  // accuracy of used type should be coord^-2
          if (p == seg.first) return 10 - M_PI;
          auto v1 =
              make_pair(seg.second.first - seg.first.first, seg.second.second - seg.first.second);
          auto v2 = make_pair(p.first - seg.first.first, p.second - seg.first.second);
          ll cross_prod = cross(v1, v2);
          if (cross_prod > 0) {  // order the backside by angle
            auto v1 = make_pair(ref_p.first.first - p.first, ref_p.first.second - p.second);
            auto v2 = make_pair(seg.first.first - p.first, seg.first.second - p.second);
            ll dot_prod = dot(v1, v2);
            ll cross_prod = cross(v2, v1);
            return atan2(cross_prod, dot_prod) / 2;
          }
          ll dot_prod = dot(v1, v2);
          double res = atan2(dot_prod, cross_prod);
          if (dot_prod <= 0 && res > 0) res = -M_PI;
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
  pair< int, int > forw_tan(pair< int, int > p) {  // can't be internal or on border
    const pair< pair< int, int >, pair< int, int > > &ref_p = hull.front();  // O(log(n))
    auto best_seg = max(function< double(const pair< pair< int, int >, pair< int, int > > &) >(
        [&p, &ref_p](const pair< pair< int, int >, pair< int, int > >
                         &seg) {  // accuracy of used type should be coord^-2
          auto v1 = make_pair(ref_p.first.first - p.first, ref_p.first.second - p.second);
          auto v2 = make_pair(seg.first.first - p.first, seg.first.second - p.second);
          ll dot_prod = dot(v1, v2);
          ll cross_prod = cross(v2, v1);       // cross(v1, v2) for back_tan !!!
          return atan2(cross_prod, dot_prod);  // order by signed angle
        }));
    return best_seg->first;
  }
//!finish
  //!end_codebook
  pair< int, int > back_tan(pair< int, int > p) {  // can't be internal or on border
    const pair< pair< int, int >, pair< int, int > > &ref_p = hull.front();  // O(log(n))
    auto best_seg = max(function< double(const pair< pair< int, int >, pair< int, int > > &) >(
        [&p, &ref_p](const pair< pair< int, int >, pair< int, int > >
                         &seg) {  // accuracy of used type should be coord^-2
          auto v1 = make_pair(ref_p.first.first - p.first, ref_p.first.second - p.second);
          auto v2 = make_pair(seg.first.first - p.first, seg.first.second - p.second);
          ll dot_prod = dot(v1, v2);
          ll cross_prod = cross(v1, v2);
          return atan2(cross_prod, dot_prod);
        }));
    return best_seg->first;
  }
  //!begin_codebook
//!start
  vector< pair< pair< int, int >, pair< int, int > > >::iterator max_in_dir(
      pair< int, int > v) {  // first is the ans. O(log(n))
    return max(function< ll(const pair< pair< int, int >, pair< int, int > > &) >(
        [&v](const pair< pair< int, int >, pair< int, int > > &seg) { return dot(v, seg.first); }));
  }
  pair< vector< pair< pair< int, int >, pair< int, int > > >::iterator,
        vector< pair< pair< int, int >, pair< int, int > > >::iterator >
//!finish
//!start
  intersections(pair< pair< int, int >, pair< int, int > > line) {  // O(log(n))
    int x = line.second.first - line.first.first;
    int y = line.second.second - line.first.second;
    auto dir = make_pair(-y, x);
    auto it_max = max_in_dir(dir);
    auto it_min = max_in_dir(make_pair(y, -x));
    ll opt_val = dot(dir, line.first);
    if (dot(dir, it_max->first) < opt_val || dot(dir, it_min->first) > opt_val)
      return make_pair(hull.end(), hull.end());
    vector< pair< pair< int, int >, pair< int, int > > >::iterator it_r1, it_r2;
    function< bool(const pair< pair< int, int >, pair< int, int > > &,
                   const pair< pair< int, int >, pair< int, int > > &) >
        inc_comp([&dir](const pair< pair< int, int >, pair< int, int > > &lft,
                        const pair< pair< int, int >, pair< int, int > > &rgt) {
          return dot(dir, lft.first) < dot(dir, rgt.first);
        });
    function< bool(const pair< pair< int, int >, pair< int, int > > &,
                   const pair< pair< int, int >, pair< int, int > > &) >
        dec_comp([&dir](const pair< pair< int, int >, pair< int, int > > &lft,
                        const pair< pair< int, int >, pair< int, int > > &rgt) {
          return dot(dir, lft.first) > dot(dir, rgt.first);
        });
    if (it_min <= it_max) {
      it_r1 = upper_bound(it_min, it_max + 1, line, inc_comp) - 1;
      if (dot(dir, hull.front().first) >= opt_val) {
        it_r2 = upper_bound(hull.begin(), it_min + 1, line, dec_comp) - 1;
      } else {
        it_r2 = upper_bound(it_max, hull.end(), line, dec_comp) - 1;
      }
    } else {
      it_r1 = upper_bound(it_max, it_min + 1, line, dec_comp) - 1;
      if (dot(dir, hull.front().first) <= opt_val) {
        it_r2 = upper_bound(hull.begin(), it_max + 1, line, inc_comp) - 1;
      } else {
        it_r2 = upper_bound(it_min, hull.end(), line, inc_comp) - 1;
      }
    }
    return make_pair(it_r1, it_r2);
  }
//!finish
//!start
  pair< pair< int, int >, pair< int, int > > diameter() {  // O(n)
    pair< pair< int, int >, pair< int, int > > res;
    ll dia_sq = 0;
    auto it1 = hull.begin();
    auto it2 = upper_begin;
    auto v1 = make_pair(hull.back().second.first - hull.back().first.first,
                        hull.back().second.second - hull.back().first.second);
    while (it2 != hull.begin()) {
      auto v2 = make_pair((it2 - 1)->second.first - (it2 - 1)->first.first,
                          (it2 - 1)->second.second - (it2 - 1)->first.second);
      ll decider = cross(v1, v2);
      if (decider > 0) break;
      --it2;
    }
    while (it2 != hull.end()) {  // check all antipodal pairs
      if (dist_sq(it1->first, it2->first) > dia_sq) {
        res = make_pair(it1->first, it2->first);
        dia_sq = dist_sq(res.first, res.second);
      }
      auto v1 =
          make_pair(it1->second.first - it1->first.first, it1->second.second - it1->first.second);
      auto v2 =
          make_pair(it2->second.first - it2->first.first, it2->second.second - it2->first.second);
      ll decider = cross(v1, v2);
      if (decider == 0) {  // report cross pairs at parallel lines.
        if (dist_sq(it1->second, it2->first) > dia_sq) {
          res = make_pair(it1->second, it2->first);
          dia_sq = dist_sq(res.first, res.second);
        }
        if (dist_sq(it1->first, it2->second) > dia_sq) {
          res = make_pair(it1->first, it2->second);
          dia_sq = dist_sq(res.first, res.second);
        }
        ++it1;
        ++it2;
      } else if (decider < 0) {
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

bool intersects(pair< pair< int, int >, pair< int, int > > line,
                pair< pair< int, int >, pair< int, int > > seg) {  // inclusive
  auto v1 = make_pair(line.second.first - line.first.first, line.second.second - line.first.second);
  auto v2 = make_pair(seg.first.first - line.first.first, seg.first.second - line.first.second);
  auto v3 = make_pair(seg.second.first - line.first.first, seg.second.second - line.first.second);
  ll val1 = cross(v1, v2);
  ll val2 = cross(v1, v3);
  return !((val1 > 0 && val2 > 0) || (val1 < 0 && val2 < 0));
}

double dist(const pair< int, int > &p1, const pair< int, int > &p2) {
  return sqrt((ll)(p2.first - p1.first) * (p2.first - p1.first) +
              (ll)(p2.second - p1.second) * (p2.second - p1.second));
}

void test_closest() {
  for (int r = 1; r < 1e9; r *= 2) {
    for (int i = 2; i < 100; i *= 2) {
      for (int t = 0; t < 100; ++t) {
        vector< pair< int, int > > from, query;
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
              double best_dist = dist(tmp.first, query[j]);
              best_dist = min(best_dist, dist(tmp.second, query[j]));
              auto v1 = make_pair(tmp.second.first - tmp.first.first,
                                  tmp.second.second - tmp.first.second);
              auto v2 =
                  make_pair(query[j].first - tmp.first.first, query[j].second - tmp.first.second);
              auto v3 =
                  make_pair(query[j].first - tmp.second.first, query[j].second - tmp.second.second);
              if (dot(v1, v2) > 0 && dot(v1, v3) < 0)
                best_dist = cross(v2, v1) / dist(tmp.second, tmp.first);
              for (int k = 0; k < i; ++k) {
                assert(dist(from[k], query[j]) > (1 - 1e-9) * best_dist);
              }
            }
          }
        }
      }
    }
  }
}

int solve(int n, vector< pair< int, int > > hull_points, vector< pair< int, int > > query_points) {
  Hull hull(hull_points);  // ACM-ICPC 2016 Finals J
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
  vector< pair< pair< int, int >, pair< int, int > > > inc_ang_range;  //[start, stop]
  if (hull.hull.size() == 2) {
    auto p1 = hull.hull[0].first;
    auto p2 = hull.hull[0].second;
    for (auto cur : query_points) {
      auto v1 = make_pair(p2.first - p1.first, p2.second - p1.second);
      auto v2 = make_pair(cur.first - p1.first, cur.second - p1.second);
      auto v3 = make_pair(cur.first - p2.first, cur.second - p2.second);
      if (dot(v1, v2) >= 0 && dot(v1, v3) <= 0 && cross(v1, v2) == 0) {
        ++always_inc;
      }
    }
    return n - query_points.size() + always_inc;
  }
  for (auto cur : query_points) {
    if (hull.contains(cur)) {
      ++always_inc;
    } else {
      auto forw = hull.forw_tan(cur);
      auto back = hull.back_tan(cur);
      inc_ang_range.emplace_back(make_pair(forw.first - cur.first, forw.second - cur.second),
                                 make_pair(back.first - cur.first, back.second - cur.second));
      if (inc_ang_range.back().first.first < 0) {
        inc_ang_range.back().first.first *= -1;
        inc_ang_range.back().first.second *= -1;
      }
      if (inc_ang_range.back().second.first < 0) {
        inc_ang_range.back().second.first *= -1;
        inc_ang_range.back().second.second *= -1;
      }
    }
  }
  int cur_not_inc = 0;
  vector< pair< pair< int, int >, bool > > inc_events;
  for (auto cur : inc_ang_range) {
    if ((cur.first.first == 0 && cur.second.first != 0) ||
        (!(cur.first.first != 0 && cur.second.first == 0) &&
         (ll)cur.second.first * cur.first.second < (ll)cur.first.first * cur.second.second)) {
      ++cur_not_inc;
    }
    inc_events.emplace_back(cur.first, 0);
    inc_events.emplace_back(cur.second, 1);
  }
  sort(inc_events.begin(), inc_events.end(),
       [](const pair< pair< int, int >, bool > &lft, const pair< pair< int, int >, bool > &rgt) {
         if (lft.first.first == 0 && rgt.first.first != 0) {
           return true;
         }
         if (lft.first.first != 0 && rgt.first.first == 0) {
           return false;
         }
         /*if(lft.first.first == 0 && rgt.first.first == 0 && (lft.first.second < 0) !=
            (rgt.first.second < 0)){ return lft.first.second < rgt.first.second;
                 }*/
         if ((ll)rgt.first.first * lft.first.second == (ll)lft.first.first * rgt.first.second) {
           return lft.second < rgt.second;
         }
         return (ll)rgt.first.first * lft.first.second < (ll)lft.first.first * rgt.first.second;
       });
  int max_not_inc = 0;
  for (auto cur : inc_events) {
    max_not_inc = max(max_not_inc, cur_not_inc);
    if (!cur.second) {
      --cur_not_inc;
    } else {
      ++cur_not_inc;
    }
  }
  return n - max_not_inc;
}

int solve_brute(int n, vector< pair< int, int > > hull_points,
                vector< pair< int, int > > query_points) {
  int max_not_inc = 0;
  vector< pair< pair< int, int >, int > > all_points;
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
           [&i, &j](const pair< pair< int, int >, int > &lft,
                    const pair< pair< int, int >, int > &rgt) {
             int lft_val = lft.first.first * i + lft.first.second * j;
             int rgt_val = rgt.first.first * i + rgt.first.second * j;
             if (lft_val == rgt_val) {
               return lft.second < rgt.second;
             }
             return lft_val < rgt_val;
           });
      int cur_not_inc = 0;
      for (auto it = all_points.begin(); it->second == 0; ++it) {
        ++cur_not_inc;
      }
      for (auto it = all_points.rbegin(); it->second == 0; ++it) {
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
          vector< pair< int, int > > hull_points;
          for (int k = 0; k < i; ++k) {
            hull_points.emplace_back(rand() % r, rand() % r);
          }
          vector< pair< int, int > > query_points;
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
        vector< pair< int, int > > from, query;
        for (int j = 0; j < i; ++j) {
          from.emplace_back(rand() % r, rand() % r);
        }
        for (int j = 0; j < 100; ++j) {
          query.emplace_back(rand() % r, rand() % r);
        }
        Hull hull(from);
        if (!hull.hull.empty()) {
          for (int j = 0; j < query.size(); ++j) {
            auto best = hull.max_in_dir(query[j])->first;
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
        vector< pair< int, int > > from;
        for (int j = 0; j < i; ++j) {
          from.emplace_back(rand() % r, rand() % r);
        }
        Hull hull(from);
        if (!hull.hull.empty()) {
          auto best = hull.diameter();
          ll dia_sq = dist_sq(best.first, best.second);
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
        vector< pair< int, int > > from, query1, query2;
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
            auto best = hull.intersections(make_pair(query1[j], query2[j]));
            if (best.first == hull.hull.end()) {
              for (auto cur : hull.hull) {
                assert(!intersects(make_pair(query1[j], query2[j]), cur));
              }
            } else {
              assert(intersects(make_pair(query1[j], query2[j]), *best.first));
              assert(intersects(make_pair(query1[j], query2[j]), *best.second));
            }
          }
        }
      }
    }
  }
}

int main() {
  test_closest();
  test_tan();
  test_in_dir();
  test_dia();
  test_intersects();
  return 0;
  int n;
  cin >> n;
  vector< pair< int, int > > hull_points;
  vector< pair< int, int > > query_points;
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
