#include <bits/stdc++.h>
typedef long long ll;
using namespace std;
const int mod = 998244353;
#ifndef M_PI
const double M_PI = acos(-1.0L);
#endif

//! escape \section{FFT 5M length/sec}
//! begin_codebook
//! start
struct Complex {
  double a = 0, b = 0;
  Complex &operator/=(const int &oth) {
    a /= oth;
    b /= oth;
    return *this;
  }
};
Complex operator+(const Complex &lft, const Complex &rgt) {
  return Complex{lft.a + rgt.a, lft.b + rgt.b};
}
Complex operator-(const Complex &lft, const Complex &rgt) {
  return Complex{lft.a - rgt.a, lft.b - rgt.b};
}
Complex operator*(const Complex &lft, const Complex &rgt) {
  return Complex{lft.a * rgt.a - lft.b * rgt.b, lft.a * rgt.b + lft.b * rgt.a};
}
void fft_rec(Complex *arr, Complex *root_pow, int len) {
  if (len != 1) {
    fft_rec(arr, root_pow, len >> 1);
    fft_rec(arr + len, root_pow, len >> 1);
  }
  root_pow += len;
  for (int i = 0; i < len; ++i) {
    Complex tmp = arr[i] + root_pow[i] * arr[i + len];
    arr[i + len] = arr[i] - root_pow[i] * arr[i + len];
    arr[i] = tmp;
  }
}
void fft(vector< Complex > &arr, int ord, bool invert) {
  assert(arr.size() == 1 << ord);
  static vector< Complex > root_pow(1);
  static int inc_pow = 1;
  static bool is_inv = false;
  if (inc_pow <= ord) {
    int idx = root_pow.size();
    root_pow.resize(1 << ord);
    for (; inc_pow <= ord; ++inc_pow) {
      for (int idx_p = 0; idx_p < 1 << (ord - 1); idx_p += 1 << (ord - inc_pow), ++idx) {
        root_pow[idx] =
            Complex{cos(-idx_p * M_PI / (1 << (ord - 1))), sin(-idx_p * M_PI / (1 << (ord - 1)))};
        if (is_inv) root_pow[idx].b = -root_pow[idx].b;
      }
    }
  }
  if (invert != is_inv) {
    is_inv = invert;
    for (Complex &cur : root_pow) cur.b = -cur.b;
  }
  int rev = 0;
  for (int i = 0; i < (1 << ord) - 1; ++i) {
    if (i < rev) swap(arr[i], arr[rev]);
    int j = 0;
    do {
      rev ^= 1 << (ord - 1 - j);
    } while (i & 1 << (j++));
  }
  fft_rec(arr.data(), root_pow.data(), 1 << (ord - 1));
  if (invert)
    for (int i = 0; i < (1 << ord); ++i) arr[i] /= (1 << ord);
}
//! finish
//! start
void mult_poly_mod(vector< int > &a, vector< int > &b, vector< int > &c) {  // c += a*b
  static vector< Complex > arr[7];  // correct upto 0.5-2M elements(mod ~= 1e9)
  if (c.size() < 400) {
    for (int i = 0; i < a.size(); ++i)
      for (int j = 0; j < b.size() && i + j < c.size(); ++j)
        c[i + j] = ((ll)a[i] * b[j] + c[i + j]) % mod;
  } else {
    int fft_ord = 32 - __builtin_clz(c.size());
    if (arr[0].size() < 1 << fft_ord)
      for (int i = 0; i < 7; ++i) arr[i].resize(1 << fft_ord);
    for (int i = 0; i < 7; ++i) fill(arr[i].begin(), arr[i].end(), Complex{});
    for (int &cur : a)
      if (cur < 0) cur += mod;
    for (int &cur : b)
      if (cur < 0) cur += mod;
    const int shift = 15;
    const int mask = (1 << shift) - 1;
    for (int i = 0; i < min(a.size(), c.size()); ++i) {
      arr[0][i].a = a[i] & mask;
      arr[1][i].a = a[i] >> shift;
    }
    for (int i = 0; i < min(b.size(), c.size()); ++i) {
      arr[2][i].a = b[i] & mask;
      arr[3][i].a = b[i] >> shift;
    }
    for (int i = 0; i < 4; ++i) fft(arr[i], fft_ord, false);
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 2; ++j) {
        int tar = 4 + i + j;
        for (int k = 0; k < (1 << fft_ord); ++k)
          arr[tar][k] = arr[tar][k] + arr[i][k] * arr[2 + j][k];
      }
    }
    for (int i = 4; i < 7; ++i) {
      fft(arr[i], fft_ord, true);
      for (int k = 0; k < (int)c.size(); ++k)
        c[k] = (c[k] + (((ll)(arr[i][k].a + 0.5) % mod) << (shift * (i - 4)))) % mod;
    }
  }
}
//! finish
//! end_codebook

int main() {
  int n = 15;
  int s = 1 << n;
  vector< Complex > p1(2 * s), p2(2 * s), p3(2 * s), tmp(2 * s);
  vector< int > i1(s), i2(s), i3(2 * s);
  vector< ll > i4(2 * s), i5(2 * s);
  for (int i = 0; i < s; ++i) {
    i1[i] = rand() % (int)(1 << 18);
    i2[i] = rand() % (int)(1 << 18);
    p1[i] = {(double)i1[i], 0};
    p2[i] = {(double)i2[i], 0};
  }
  for (int i = 0; i < s; ++i) {
    for (int j = 0; j < s; ++j) {
      i4[i + j] += (ll)i1[i] * i2[j];
    }
  }
  fft(p1, n + 1, false);
  fft(p2, n + 1, false);
  for (int i = 0; i < 2 * s; ++i) {
    p3[i] = p2[i] * p1[i];
  }
  fft(p3, n + 1, true);
  for (int i = 0; i < 2 * s; ++i) {
    i5[i] = p3[i].a + 0.5;
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
  for (int i = 0; i < s; ++i) {
    for (int j = 0; j < s; ++j) {
      i4[i + j] = ((ll)i1[i] * i2[j] + i4[i + j]) % mod;
    }
  }
  max_err = 0;
  for (int i = 0; i < 2 * s; ++i) {
    max_err = max(max_err, abs(i3[i] - i4[i]));
  }
  cout << max_err << '\n';
  return 0;
}