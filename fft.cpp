//!escape FFT 10-15M length/sec
//!begin_codebook
// integer c = a*b is accurate if c_i < 2^49
#pragma GCC optimize ("Ofast") //10% performance
#include <complex.h>
extern "C" __complex__ double __muldc3(
    double a, double b, double c, double d){
  return a*c-b*d+I*(a*d+b*c); // 40% performance
}
#include <bits/stdc++.h>
//!pause
#define DEBUG(...) cerr << __VA_ARGS__ << endl;

#ifndef CDEBUG
#undef DEBUG
#define DEBUG(...) ((void)0);
#define NDEBUG
#endif

#define ran(i, a, b) for (auto i = (a); i < (b); i++)

typedef long long ll;
typedef long double ld;
using namespace std;
const int mod = 1e9 + 7;
#ifndef M_PI
const double M_PI = acos(-1.0);
#endif

//!unpause
//!no_col_hash
//!start
typedef complex<double> Comp;
void fft_rec(Comp *arr, Comp *root_pow, int len) {
  if (len != 1) {
    fft_rec(arr,       root_pow, len >> 1);
    fft_rec(arr /*lp*/+ len/*rp*/, root_pow, len >> 1);
  }
  root_pow += len;
  ran(i, 0, len){
    tie(arr[i], arr[i + len]) = pair<Comp, Comp> {
        arr[i] /*lp*/+/*rp*/ root_pow[i] * arr[i + len],
        arr[i] /*lp*/-/*rp*/ root_pow[i] * arr[i + len] };
  }
}
void fft(vector<Comp> &arr, int ord, bool invert) {
  assert(arr.size() == 1 << ord);
  static vector<Comp> root_pow(1);
  static int inc_pow = 1;
  static bool is_inv = false;
  if (inc_pow <= ord) {
    int idx = root_pow.size();
    root_pow.resize(1 << ord);
    for (; inc_pow <= ord; ++inc_pow) {
      for (int idx_p = 0; idx_p < 1 << (ord - 1);
           idx_p += 1 << (ord - inc_pow), ++idx) {
        root_pow[idx] = Comp {
           cos(-idx_p * M_PI / (1 << (ord - 1))),
           sin(-idx_p * M_PI / (1 << (ord - 1))) };
        if (is_inv) root_pow[idx] = conj(root_pow[idx]);
      }
    }
  }
  if (invert != is_inv) {
    is_inv = invert;
    for (Comp &cur : root_pow) cur = conj(cur);
  }
  int j = 0;
  ran(i, 1, (1<<ord)){
    int m = 1 << (ord - 1);
    bool cont = true;
    while (cont) {
      cont = j & m;
      j ^= m;
      m >>= 1;
    }
    if (i < j) swap(arr[i], arr[j]);
  }
  fft_rec(arr.data(), root_pow.data(), 1 << (ord - 1));
  if (invert)
    ran(i, 0, 1<<ord) arr[i] /= (1 << ord);
}
//!finish
//!start
void mult_poly_mod(vector<int> &a, vector<int> &b,
  vector<int> &c) { // c += a*b
  static vector<Comp> arr[4];
  // correct upto 0.5-2M elements(mod ~= 1e9)
  if (c.size() < 400) {
    ran(i, 0, (int)a.size())
      ran(j, 0, min((int)b.size(), (int)c.size()-i))
        c[i + j] = ((ll)a[i] * b[j] + c[i + j]) % mod;
  } else {
    int ord = 32 - __builtin_clz((int)c.size()-1);
    if ((int)arr[0].size() != 1 << ord){
      ran(i, 0, 4) arr[i].resize(1 << ord);
    }
    ran(i, 0, 4)
      fill(arr[i].begin(), arr[i].end(), Comp{});
    for (int &cur : a) if (cur < 0) cur += mod;
    for (int &cur : b) if (cur < 0) cur += mod;
    const int shift = 15;
    const int mask = (1 << shift) - 1;
    ran(i, 0, (int)min(a.size(), c.size())){
      arr[0][i] += a[i] & mask;
      arr[1][i] += a[i] >> shift;
    }
    ran(i, 0, (int)min(b.size(), c.size())){
      arr[0][i] += Comp{0, (b[i] & mask)};
      arr[1][i] += Comp{0, (b[i] >> shift)};
    }
    ran(i, 0, 2) fft(arr[i], ord, false);
    ran(i, 0, 2){
      ran(j, 0, 2){
        int tar = 2 + (i + j) / 2;
        Comp mult = {0, -0.25};
        if (i ^ j) mult = {0.25, 0};
        ran(k, 0, 1<<ord){
          int rev_k = ((1 << ord) - k) % (1 << ord);
          Comp ca = arr[i][k] /*lp*/+/*rp*/ conj(arr[i][rev_k]);
          Comp cb = arr[j][k] /*lp*/-/*rp*/ conj(arr[j][rev_k]);
          arr[tar][k] = arr[tar][k] + mult * ca * cb;
        }
      }
    }
    ran(i, 2, 4){
      fft(arr[i], ord, true);
      ran(k, 0, (int)c.size()){
        c[k] = (c[k] + (((ll)(arr[i][k]/*lp*/.real()/*rp*/+0.5)%mod)
                    << (shift * (2 * (i-2) + /*lp*/0/*rp*/)))) % mod;
        c[k] = (c[k] + (((ll)(arr[i][k]/*lp*/.imag()/*rp*/+0.5)%mod)
                    << (shift * (2 * (i-2) + /*lp*/1/*rp*/)))) % mod;
      }
    }
  }
}
//!finish
//!end_codebook

int main() {
  int n = 20;
  int s = 1 << n;
  vector<Comp> p1(2 * s), p2(2 * s), tmp(2 * s);
  vector<int> i1(s), i2(s), i3(2 * s);
  vector<ll> i4(2 * s), i5(2 * s);
  for (int i = 0; i < s; ++i) {
    i1[i] = rand() % (int)(1 << 18);
    i2[i] = rand() % (int)(1 << 18);
    p1[i] = {(double)i1[i], (double)i2[i]};
  }
  /*
  for (int i = 0; i < s; ++i) {
    for (int j = 0; j < s; ++j) {
      i4[i + j] += (ll)i1[i] * i2[j];
    }
  }
  */
  fft(p1, n + 1, false);
  Comp mult = {0, -0.25};
  for (int i = 0; i < 2 * s; ++i) {
    int rev_i = (2 * s - i) % (2 * s);
    Comp ca = p1[i] + conj(p1[rev_i]);
    Comp cb = p1[i] - conj(p1[rev_i]);
    p2[i] = mult * ca * cb;
  }
  fft(p2, n + 1, true);
  for (int i = 0; i < 2 * s; ++i) {
    i5[i] = p2[i].real() + 0.5;
  }
  ll max_err = 0;
  for (int i = 0; i < 2 * s; ++i) {
    max_err = max(max_err, abs(i5[i] - i4[i]));
  }
  cout << max_err << '\n';
  for (int i = 0; i < s; ++i) {
    i1[i] = rand();
    i2[i] = rand();
  }
  fill(i3.begin(), i3.end(), 0);
  mult_poly_mod(i1, i2, i3);
  fill(i4.begin(), i4.end(), 0);
  /*
  for (int i = 0; i < s; ++i) {
    for (int j = 0; j < s; ++j) {
      i4[i + j] = ((ll)i1[i] * i2[j] + i4[i + j]) % mod;
    }
  }
  */
  max_err = 0;
  for (int i = 0; i < 2 * s; ++i) {
    max_err = max(max_err, abs(i3[i] - i4[i]));
  }
  cout << max_err << '\n';
  return 0;
}
