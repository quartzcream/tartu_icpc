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

//!escape \section{PRNGs and Hash functions}
/*
//!begin_codebook
//!start
mt19937 gen;
uint64_t rand64() { return gen() ^ ((uint64_t)gen() << 32); }
//!finish
//!end_codebook
*/
//!begin_codebook
//!start
uint64_t rand64() {
  static uint64_t x = 1; //x != 0
  x ^= x >> 12;
  x ^= x << 25;
  x ^= x >> 27;
  return x * 0x2545f4914f6cdd1d; //can remove mult
}
//!finish
//!end_codebook
/*
//!begin_codebook
//!start
uint64_t mix(uint64_t x){ //can replace constant with variable
        uint64_t mem[2] = { x, 0xdeadbeeffeebdaedull };
        asm volatile (
                "pxor %%xmm0, %%xmm0;"
                "movdqa (%0), %%xmm1;"
                "aesenc %%xmm0, %%xmm1;"
                "movdqa %%xmm1, (%0);"
                :
                : "r" (&mem[0])
                : "memory"
        );
        return mem[0]; // use both slots for 128 bit hash
}
//!finish
//!end_codebook
*/
//!begin_codebook
//!start
uint64_t mix(uint64_t x) { //x != 0
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
  x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
  x = x ^ (x >> 31);
  return x;
}
//!finish
//!start
uint64_t unmix(uint64_t x) {
  x = (x ^ (x >> 31) ^ (x >> 62)) * 0x319642b2d24d8ec3;
  x = (x ^ (x >> 27) ^ (x >> 54)) * 0x96de1b173f119089;
  x = x ^ (x >> 30) ^ (x >> 60);
  return x;
}
//!finish
//!start
uint64_t combine(uint64_t x, uint64_t y) {
  if (y < x) swap(x, y); // remove for ord
  return mix(mix(x) + y);
}
//!finish
//!end_codebook

int main() {
  {
    uint64_t x = 1;
    auto y = x;
    cout << x << '\n';
    ran(i, 0, 10) {
      x = mix(x);
      cout << x << '\n';
    }
    ran(i, 0, 10) {
      x = unmix(x);
      cout << x << '\n';
    }
    assert(x == y);
  }
    ran(i, 0, 10) {
      cout << rand64() << '\n';
    }
  return 0;
}
