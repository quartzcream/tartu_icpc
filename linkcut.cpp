#include <stdio.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
using namespace std;

#define ran(i, a, b) for (auto i = (a); i < (b); i++)
using ll = long long;
using vi = vector<int>;
using ii = pair<int, int>;
#if defined(SHIROKO1_LOCAL) && !defined(NDEBUG)
#define DEBUG(...) cerr << __VA_ARGS__ << endl;
#else
#define DEBUG(...) ((void)0)
#endif

//!escape Splay Tree + Link-Cut O(NlogN)
//!begin_codebook
//!start
struct Tree *treev;
struct Tree {
  struct T {
    int i;
    constexpr T() : i(-1) {}
    T(int _i) : i(_i) {}
    operator int() const { return i; }
    explicit operator bool() const { return i != -1; }
    Tree *operator->() { return treev + i; }
  };
  T c[2], p;
  /* insert monoid here */
  /*ly*/T link;/*ry*/

  Tree() {
    /* init monoid here */
    /*ly*/link = -1;/*ry*/
  }
};
using T = Tree::T;
constexpr T NIL;

void update(T t) { /* recalculate the monoid here */
}

void propagate(T t) {
  assert(t);/*ly*/
  for (T c : t->c)
    if (c) c->link = t->link;/*ry*/
  /* lazily propagate updates here */
}/*lp*/


void lazy_reverse(T t) { /* lazily reverse t here */
}/*rp*/

T splay(T n) {
  for (;;) {
    propagate(n);
    T p = n->p;
    if (p == NIL) break;
    propagate(p);
    ll px = p->c[1] == n;
    assert(p->c[px] == n);
    T g = p->p;
    if (g == NIL) { /* zig */
      p->c[px] = n->c[px ^ 1];
      p->c[px]->p = p;
      n->c[px ^ 1] = p;
      n->c[px ^ 1]->p = n;
      n->p = NIL;
      update(p);
      update(n);
      break;
    }
    propagate(g);
    ll gx = g->c[1] == p;
    assert(g->c[gx] == p);
    T gg = g->p;
    ll ggx = gg && gg->c[1] == g;
    if (gg) assert(gg->c[ggx] == g);
    if (gx == px) { /* zig zig */
      g->c[gx] = p->c[gx ^ 1];
      g->c[gx]->p = g;
      p->c[gx ^ 1] = g;
      p->c[gx ^ 1]->p = p;
      p->c[gx] = n->c[gx ^ 1];
      p->c[gx]->p = p;
      n->c[gx ^ 1] = p;
      n->c[gx ^ 1]->p = n;
    } else { /* zig zag */
      g->c[gx] = n->c[gx ^ 1];
      g->c[gx]->p = g;
      n->c[gx ^ 1] = g;
      n->c[gx ^ 1]->p = n;
      p->c[gx ^ 1] = n->c[gx];
      p->c[gx ^ 1]->p = p;
      n->c[gx] = p;
      n->c[gx]->p = n;
    }
    if (gg) gg->c[ggx] = n;
    n->p = gg;
    update(g);
    update(p);
    update(n);
    if (gg) update(gg);
  }
  return n;
}

T extreme(T t, int x) {
  while (t->c[x]) t = t->c[x];
  return t;
}
T set_child(T t, int x, T a) {
  T o = t->c[x];
  t->c[x] = a;
  update(t);
  o->p = NIL;
  a->p = t;
  return o;
}
//!finish
//!start

/************* Link-Cut Tree: **************/

T expose(T t) {
  set_child(splay(t), 1, NIL);
  T leader = splay(extreme(t, 0));
  if (leader->link == NIL) return t;
  set_child(splay(leader), 0, expose(leader->link));
  return splay(t);
}

void link(T t, T p) {
  assert(t->link == NIL);
  t->link = p;
}
T cut(T t) {
  T p = t->link;
  if (p) expose(p);
  t->link = NIL;
  return p;
}/*lp*/
void make_root(T t) {
  expose(t);
  lazy_reverse(extreme(splay(t), 0));
}/*rp*/
//!finish
//!end_codebook

int main() { return 0; }
