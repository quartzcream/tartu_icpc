#define DEBUG(...) cerr << __VA_ARGS__ << endl;

#ifndef CDEBUG
#undef DEBUG
#define DEBUG(...) ((void)0);
#endif

#define ran(i, a, b) for (auto i = (a); i < (b); i++)

#include <bits/stdc++.h>
typedef long long ll;
typedef long double ld;
typedef unsigned char uchar;
using namespace std;
//!escape \section{Radixsort 50M 64 bit integers as single array in 1 sec}
//!begin_codebook
//!start

template <typename T>
void rsort(T *a, T *b, int size, int d = sizeof(T) - 1) {
  int b_s[256]{};
  ran(i, 0, size) { ++b_s[(a[i] >> (d * 8)) & 255]; }
  // ++b_s[*((uchar *)(a + i) + d)];
  T *mem[257];
  mem[0] = b;
  T **l_b = mem + 1;
  l_b[0] = b;
  ran(i, 0, 255) { l_b[i + 1] = l_b[i] + b_s[i]; }
  for (T *it = a; it != a + size; ++it) {
    T id = ((*it) >> (d * 8)) & 255;
    *(l_b[id]++) = *it;
  }
  l_b = mem;
  if (d) {
    T *l_a[256];
    l_a[0] = a;
    ran(i, 0, 255) l_a[i + 1] = l_a[i] + b_s[i];
    ran(i, 0, 256) {
      if (l_b[i + 1] - l_b[i] < 100) {
        sort(l_b[i], l_b[i + 1]);
        if (d & 1) copy(l_b[i], l_b[i + 1], l_a[i]);
      } else {
        rsort(l_b[i], l_a[i], b_s[i], d - 1);
      }
    }
  }
}
//!finish

const int nmax = 5e7;

ll arr[nmax], tmp[nmax];

int main() {
  for (int i = 0; i < nmax; ++i) arr[i] = ((ll)rand() << 32) | rand();
  rsort(arr, tmp, nmax);
  assert(is_sorted(arr, arr + nmax));
}
//!end_codebook
