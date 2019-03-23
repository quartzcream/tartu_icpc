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

//!escape Delaunay triangulation O(nlogn)

//!begin_codebook
//!start
const int max_co = (1 << 28) - 5;
struct Vec {
  int x, y;
  bool operator==(const Vec &oth) {
    return x == oth.x && y == oth.y;
  }
  bool operator!=(const Vec &oth) {
    return !operator==(oth);
  }
  Vec operator-(const Vec &oth) {
    return {x - oth.x, y - oth.y};
  }
};
ll cross(Vec a, Vec b) {
  return (ll)a.x * b.y - (ll)a.y * b.x;
}
ll dot(Vec a, Vec b) {
  return (ll)a.x * b.x + (ll)a.y * b.y;
}
struct Edge {
  Vec tar;
  Edge *nxt;
  Edge *inv = NULL;
  Edge *rep = NULL;
  bool vis = false;
};
struct Seg {
  Vec a, b;
  bool operator==(const Seg &oth) {
    return a == oth.a && b == oth.b;
  }
  bool operator!=(const Seg &oth) {
    return !operator==(oth);
  }
};
ll orient(Vec a, Vec b, Vec c) {
  return (ll)a.x * (b.y - c.y) + (ll)b.x * (c.y - a.y) +
         (ll)c.x * (a.y - b.y);
}
//!finish
//!start
bool in_c_circle(Vec *arr, Vec d) {
  if (cross(arr[1] - arr[0], arr[2] - arr[0]) == 0)
    return true; // degenerate
  ll m[3][3];
  ran(i, 0, 3) {
    m[i][0] = arr[i].x - d.x;
    m[i][1] = arr[i].y - d.y;
    m[i][2] = m[i][0] * m[i][0];
    m[i][2] += m[i][1] * m[i][1];
  }
  __int128 res = 0;
  res +=
    (__int128)(m[0][0] * m[1][1] - m[0][1] * m[1][0]) *
    m[2][2];
  res +=
    (__int128)(m[1][0] * m[2][1] - m[1][1] * m[2][0]) *
    m[0][2];
  res -=
    (__int128)(m[0][0] * m[2][1] - m[0][1] * m[2][0]) *
    m[1][2];
  return res > 0;
}
//!finish
//!start
Edge *add_triangle(Edge *a, Edge *b, Edge *c) {
  Edge *old[] = {a, b, c};
  Edge *tmp = new Edge[3];
  ran(i, 0, 3) {
    old[i]->rep = tmp + i;
    tmp[i] = {
      old[i]->tar, tmp + (i + 1) % 3, old[i]->inv};
    if (tmp[i].inv) tmp[i].inv->inv = tmp + i;
  }
  return tmp;
}

Edge *add_point(
  Vec p, Edge *cur) { // returns outgoing edge
  Edge *triangle[] = {cur, cur->nxt, cur->nxt->nxt};
  ran(i, 0, 3) {
    if (orient(triangle[i]->tar,
          triangle[(i + 1) % 3]->tar, p) < 0)
      return NULL;
  }
  ran(i, 0, 3) {
    if (triangle[i]->rep) {
      Edge *res = add_point(p, triangle[i]->rep);
      if (res)
        return res; // unless we are on last layer we
                    // must exit here
    }
  }
  Edge p_as_e{p};
  Edge tmp{cur->tar};
  tmp.inv = add_triangle(&p_as_e, &tmp, cur = cur->nxt);
  Edge *res = tmp.inv->nxt;
  tmp.tar = cur->tar;
  tmp.inv = add_triangle(&p_as_e, &tmp, cur = cur->nxt);
  tmp.tar = cur->tar;
  res->inv = add_triangle(&p_as_e, &tmp, cur = cur->nxt);
  res->inv->inv = res;
  return res;
}

Edge *delaunay(vector<Vec> &points) {
  random_shuffle(points.begin(), points.end());
  Vec arr[] = {{4 * max_co, 4 * max_co},
    {-4 * max_co, max_co}, {max_co, -4 * max_co}};
  Edge *res = new Edge[3];
  ran(i, 0, 3) res[i] = {arr[i], res + (i + 1) % 3};
  for (Vec &cur : points) {
    Edge *loc = add_point(cur, res);
    Edge *out = loc;
    arr[0] = cur;
    while (true) {
      arr[1] = out->tar;
      arr[2] = out->nxt->tar;
      Edge *e = out->nxt->inv;
      if (e && in_c_circle(arr, e->nxt->tar)) {
        Edge tmp{cur};
        tmp.inv = add_triangle(&tmp, out, e->nxt);
        tmp.tar = e->nxt->tar;
        tmp.inv->inv =
          add_triangle(&tmp, e->nxt->nxt, out->nxt->nxt);
        out = tmp.inv->nxt;
        continue;
      }
      out = out->nxt->nxt->inv;
      if (out->tar == loc->tar) break;
    }
  }
  return res;
}
//!finish
//!start
void extract_triangles(
  Edge *cur, vector<vector<Seg> > &res) {
  if (!cur->vis) {
    bool inc = true;
    Edge *it = cur;
    do {
      it->vis = true;
      if (it->rep) {
        extract_triangles(it->rep, res);
        inc = false;
      }
      it = it->nxt;
    } while (it != cur);
    if (inc) {
      Edge *triangle[3] = {cur, cur->nxt, cur->nxt->nxt};
      res.resize(res.size() + 1);
      vector<Seg> &tar = res.back();
      ran(i, 0, 3) {
        if ((abs(triangle[i]->tar.x) < max_co &&
              abs(triangle[(i + 1) % 3]->tar.x) <
                max_co))
          tar.push_back({triangle[i]->tar,
            triangle[(i + 1) % 3]->tar});
      }
      if (tar.empty()) res.pop_back();
    }
  }
}
//!finish
//!end_codebook

void extract_build(Edge *cur, bool val,
  vector<vector<vector<Seg> > > &res, int d = 0) {
  if (cur->vis != val) {
    bool at_s = true;
    for (Edge *it = cur; at_s || it != cur;
         it = it->nxt, at_s = false) {
      it->vis = val;
      if (it->rep) {
        extract_build(it->rep, val, res, d + 1);
      }
    }
    while (res.size() <= d) {
      res.resize(res.size() + 1);
    }
    auto &cur_res = res[d];
    Edge *triangle[3] = {cur, cur->nxt, cur->nxt->nxt};
    cur_res.resize(cur_res.size() + 1);
    vector<Seg> &tar = cur_res.back();
    for (int i = 0; i < 3; ++i) {
      tar.push_back(
        {triangle[i]->tar, triangle[(i + 1) % 3]->tar});
    }
  }
}

bool internal_intersect(Seg a, Seg b) {
  ll v1 = cross(a.b - a.a, b.a - a.a);
  if (!v1 && dot(b.a - a.a, b.a - a.b) < 0) return true;
  ll v2 = cross(a.b - a.a, b.b - a.a);
  if (!v2 && dot(b.b - a.a, b.b - a.b) < 0) return true;
  ll v3 = cross(b.b - b.a, a.a - b.a);
  if (!v3 && dot(a.a - b.a, a.a - b.b) < 0) return true;
  ll v4 = cross(b.b - b.a, a.b - b.a);
  if (!v4 && dot(a.b - b.a, a.b - b.b) < 0) return true;
  return ((v1 > 0 && v2 < 0 || v1 < 0 && v2 > 0) &&
          (v3 > 0 && v4 < 0 || v3 < 0 && v4 > 0));
}

bool clean_intersect(Seg a, Seg b) {
  ll v1 = cross(a.b - a.a, b.a - a.a);
  ll v2 = cross(a.b - a.a, b.b - a.a);
  ll v3 = cross(b.b - b.a, a.a - b.a);
  ll v4 = cross(b.b - b.a, a.b - b.a);
  if (!v1 || !v2 || !v3 || !v4) return false;
  return ((v1 > 0 && v2 < 0 || v1 < 0 && v2 > 0) &&
          (v3 > 0 && v4 < 0 || v3 < 0 && v4 > 0));
}

bool shared_seg(Seg a, Seg b) {
  if (!cross(a.b - a.a, b.a - a.a) &&
      dot(a.b - a.a, b.a - a.a) > 0 &&
      dot(a.a - a.b, b.a - a.b) > 0)
    return true;
  if (!cross(a.b - a.a, b.b - a.a) &&
      dot(a.b - a.a, b.b - a.a) > 0 &&
      dot(a.a - a.b, b.b - a.b) > 0)
    return true;
  if (!cross(b.b - b.a, a.a - b.a) &&
      dot(b.b - b.a, a.a - b.a) > 0 &&
      dot(b.a - b.b, a.a - b.b) > 0)
    return true;
  if (!cross(b.b - b.a, a.b - b.a) &&
      dot(b.b - b.a, a.b - b.a) > 0 &&
      dot(b.a - b.b, a.b - b.b) > 0)
    return true;
  return false;
}

void print_triangulation(
  vector<vector<Seg> > &triangles) {
  for (int i = 0; i < triangles.size() && i < 30; ++i) {
    auto cur = triangles[i];
    cout << "{\n";
    for (auto seg : cur) {
      cout << "(" << seg.a.x << "," << seg.a.y << ") ";
      cout << "(" << seg.b.x << "," << seg.b.y << ")\n";
    }
    cout << "}\n";
  }
  cout << "\n\n\n";
}

bool is_triangulation(
  vector<vector<Seg> > &triangles, int n, int h = 0) {
  if (n == 0) {
    return triangles.size() == 0;
  }
  Vec arr[3];
  int t = 0;
  for (int i = 0; i < triangles.size(); ++i) {
    if (triangles[i].size() == 1) {
      ++h;
    } else {
      ++t;
    }
    if (triangles[i].size() != 1 &&
        triangles[i].size() != 3) {
      cerr << "size\n";
      return false;
    }
    for (int j = 0; j < triangles[i].size(); ++j) {
      for (int k = 0; k < triangles.size(); ++k) {
        for (int l = 0; l < triangles[k].size(); ++l) {
          arr[l] = triangles[k][l].a;
          if (triangles[i][j] != triangles[k][l] &&
              (triangles[i][j].a != triangles[k][l].b ||
                triangles[i][j].b !=
                  triangles[k][l].a) &&
              (clean_intersect(
                 triangles[i][j], triangles[k][l]) ||
                shared_seg(
                  triangles[i][j], triangles[k][l]))) {
            print_triangulation(triangles);
            cerr << "intersect\n";
            return false;
          }
        }
      }
    }
  }
  if (t != 2 * n - h - 2) {
    cerr << "euler\n";
    cerr << "n=" << n << ",h=" << h << ",t=" << t
         << '\n';
  }
  return t == 2 * n - h - 2;
}

bool is_delaunay(vector<vector<Seg> > &triangles) {
  // TODO check if triangulation is properly connected
  Vec arr[3];
  for (int i = 0; i < triangles.size(); ++i) {
    if (triangles[i].size() == 3 &&
        orient(triangles[i][0].a, triangles[i][1].a,
          triangles[i][2].a) <= 0) {
      print_triangulation(triangles);
      cerr << "orientation\n";
      return false;
    }
    for (int j = 0; j < triangles[i].size(); ++j) {
      for (int k = 0; k < triangles.size(); ++k) {
        for (int l = 0; l < triangles[k].size(); ++l) {
          arr[l] = triangles[k][l].a;
          if (triangles[i][j] != triangles[k][l] &&
              (triangles[i][j].a != triangles[k][l].b ||
                triangles[i][j].b !=
                  triangles[k][l].a) &&
              internal_intersect(
                triangles[i][j], triangles[k][l])) {
            print_triangulation(triangles);
            cerr << "intersect\n";
            return false;
          }
        }
        if (triangles[k].size() == 3) {
          if (in_c_circle(arr, triangles[i][j].a) ||
              in_c_circle(arr, triangles[i][j].b)) {
            print_triangulation(triangles);
            cerr << "circle\n";
            return false;
          }
        } else if (triangles[k].size() != 1) {
          print_triangulation(triangles);
          cerr << "size\n";
          return false;
        }
      }
    }
  }
  return true;
}

void test_delaunay(int n) {
  for (int i = 0; i <= n; ++i) {
    cout << "test " << i << '\n';
    for (int t = 0; t < 1000; ++t) {
      vector<Vec> points;
      for (int j = 0; j < i; ++j) {
        points.push_back(
          {rand() % (2 * max_co - 1) - max_co + 1,
            rand() % (2 * max_co - 1) - max_co + 1});
        bool err;
        do {
          err = false;
          for (int k = 0; k < j; ++k) {
            if (points[k] == points[j]) {
              points[j] = {
                rand() % (2 * max_co - 1) - max_co + 1,
                rand() % (2 * max_co - 1) - max_co + 1};
              err = true;
              break;
            }
          }
        } while (err);
      }
      vector<vector<Seg> > triangulation;
      Edge *start = delaunay(points);
      extract_triangles(start, triangulation);
      if (!is_triangulation(triangulation, i) ||
          !is_delaunay(triangulation)) {
        for (int j = 0; j < i; ++j) {
          cout << points[j].x << ' ' << points[j].y
               << '\n';
        }
        exit(0);
      }
    }
  }
}

int main() {
  vector<Vec> points;
  /*
  for (int i = 0; i < 2e5; ++i) {
    // points.push_back(Vec {0, i});
    points.push_back(Vec{rand() % (2 * max_co - 1) -
  max_co + 1, rand() % (2 * max_co - 1) - max_co + 1});
  }
  */
  Edge *cur = delaunay(points);
  test_delaunay(100);
  vector<vector<Seg> > triangulation;
  extract_triangles(cur, triangulation);
  print_triangulation(triangulation);
  return 0;
}
