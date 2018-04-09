#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef unsigned long long ull;

//!escape \section{Rabbin Miller prime check}

//!begin_codebook
//!start
__int128 pow_mod(__int128 a, ll n, __int128 mod) {
  __int128 res = 1;
  for (ll i = 0; i < 64; ++i) {
    if (n & (1LL << i))
      res = (res * a) % mod;
    a = (a * a) % mod;
  }
  return res;
}

bool is_prime(ll n) { //guaranteed for 64 bit numbers
  if (n == 2 || n == 3) return true;
  if (!(n & 1) || n == 1) return false;
  static vector< char > witnesses = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
  ll s = __builtin_ctz(n - 1);
  ll d = (n - 1) >> s;
  __int128 mod = n;
  for (__int128 a : witnesses) {
    if (a >= mod) break;
    a = pow_mod(a, d, mod);
    if (a == 1 || a == mod - 1) continue;
    for (ll r = 1; r < s; ++r) {
      a = a * a % mod;
      if (a == 1) return false;
      if (a == mod - 1) break;
    }
    if (a != mod - 1) return false;
  }
  return true;
}
//!finish
//!end_codebook

int main() {
  ll ten_to_pow = 10;
  for (int i = 1; i <= 19; ++i) {
    ll cand = ten_to_pow;
    for (; !is_prime(cand); ++cand) {
    }
    cout << "First prime comes " << cand - ten_to_pow << " after 10^" << i << "\n";
    ten_to_pow *= 10;
  }
  ll cnt = 0;
  for (ll i = 1; i < 1e7; ++i) {
    cnt += is_prime(i);
  }
  cout << "There are " << cnt << " primes below 10^7\n";
  return 0;
}
