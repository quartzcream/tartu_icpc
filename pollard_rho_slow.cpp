#include <bits/stdc++.h>
using namespace std;
typedef long long               ll;

//!escape \section{Rabbin Miller prime check, Pollard rho factorization $\mathcal{O}(\sqrt{p})$}

//!begin_codebook
//!start
static vector< char > small_primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

__int128 pow_mod(__int128 a, ll n, __int128 mod) {
  __int128 res = 1;
  for (ll i = 1; i <= n; i<<=1) {
    if (n & i)
      res = (res * a) % mod;
    a = (a * a) % mod;
  }
  return res;
}

bool is_prime(ll n) { //guaranteed for 64 bit numbers
  if (n == 2 || n == 3) return true;
  if (!(n & 1) || n == 1) return false;
  ll s = __builtin_ctz(n - 1);
  ll d = (n - 1) >> s;
  __int128 mod = n;
  for (__int128 a : small_primes) {
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

//!start
ll sqp1(ll x, ll n){
  return ((__int128)x*x+1)%n;
};

ll pollard_rho(ll n){
  while(true){
    ll lv = rand()%n;
    ll v = sqp1(lv, n);
    int tar = 1;
    for(int i=1; ; ++i){
      ll g = __gcd(abs(v-lv), n);
      if(g == n){
        break;
      } else if(g != 1){
        return g;
      }
      if(i == tar){
        lv = v;
        tar <<= 1;
      }
      v = sqp1(v, n);
    }
  }
}

void prime_factor(ll n, map<ll, int> &res){
  while(!(n&1)){
    ++res[2];
    n >>=1;
  }
  for(int i=3; n>1 && i<=1000; i += 2){
    while(!(n%i)){
      ++res[i];
      n /= i;
    }
  }
  if(n != 1){
    if(is_prime(n)){
      ++res[n];
    } else {
      ll factor = pollard_rho(n);
      prime_factor(factor, res);
      prime_factor(n/factor, res);
    }
  }
}
//!finish
//!end_codebook

int main(){
  /*
  int lim = 1e5;
  vector<bool> sieve(lim, true);
  for(int i=2; i<lim; ++i){
    assert(is_prime(i) == sieve[i]);
    if(sieve[i]){
      for(int j=2*i; j<lim; j+=i){
        sieve[j] = false;
      }
    }
  }
	vector<int> offset63 = {	25, 165, 259, 301, 375, 387, 391, 409, 457, 471 };
  for(int cur : offset63){
    assert(is_prime((1LL<<63)-cur));
  }
  for(ll i=1; i<=lim; ++i){
    map<ll, int> factorization;
    prime_factor(i, factorization);
    ll c = 1;
    for(auto cur : factorization){
      assert(is_prime(cur.first));
      for(int j=0; j < cur.second; ++j){
        c *= cur.first;
      }
    }
    assert(c==i);
  }
  */
  ll last = 1e9+7;
  for(int i=0; i<500; ++i){
    ll nxt = last+1;
    while(!is_prime(nxt)){
      ++nxt;
    }
    ll cor = last*nxt;
    map<ll, int> factorization;
    prime_factor(cor, factorization);
    assert(factorization.size() == 2);
    ll cand = 1;
    for(auto cur : factorization){
      assert(is_prime(cur.first));
      assert(cur.second == 1);
      cand *= cur.first;
    }
    assert(cor==cand);
    last = nxt;
  }
}
