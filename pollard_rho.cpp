#define DEBUG(...) cerr << __VA_ARGS__ << endl;

#ifndef CDEBUG
#undef DEBUG
#define DEBUG(...) ((void)0);
#define NDEBUG
#endif

#define ran(i, a, b) for (auto i = (a); i < (b); i++)

#include <bits/stdc++.h>
typedef long long ll;
typedef unsigned long long ull;
typedef long double ld;
using namespace std;
const int mod = 1e9 + 7;
#ifndef M_PI
const double M_PI = acos(-1.0);
#endif

//!escape Fast mod mult, Rabbin Miller prime check, Pollard rho factorization O(p^0.5)

//!begin_codebook
//!start

struct ModArithm {
  //!pause
  ull q = 0; // (1<<(shift+64))/n
  char shift = -1;
  //!unpause
  ull n;
  ld rec;
  ModArithm(ull _n) : n(_n) { // n in [2, 1<<63)
    rec = 1.0L / n;
    //!pause
    ull rem = 1;
    while (rem < n) {
      ++shift;
      rem <<= 1;
    }
    for (int i = 63; i >= 0; --i) {
      if (rem >= n) {
        q |= 1LL << i;
        rem -= n;
      }
      rem *= 2;
    }
    //!unpause
  }
  // a, b in [0, min(2*n, 1<<63))
  ull multf(ull a, ull b) {
    ull mult = (ld)a * b * rec + 0.5L;
    ll res = a * b - mult * n;
    if (res < 0) res += n;
    return res; // in [0, n-1)
  }
  //!pause
#ifndef ONLINE_JUDGE
  ull multa(ll a, ll b) { // a, b in [0, min(2*n, 1<<63))
    ull res;
    ull tmp;
    __asm__(
      "mulq %[b];\n\t"          // a*b
      "movq %%rax, %[res];\n\t" // ab_l = (a*b) & ((1 <<
                                // 64)-1)
      "movq %%rdx, %[tmp];\n\t" // ab_h = (a*b) >> 64
      "mulq %[q];\n\t"          // ab_l * q
      "movq %[tmp], %%rax;\n\t"
      "movq %%rdx, %[tmp];\n\t" // abq_m = (ab_l * q) >>
                                // 64
      "mulq %[q];\n\t"          // ab_h * q
      "addq %[tmp], %%rax;\n\t" // abq_m += (ab_h * q) &
                                // ((1 << 64)-1)
      "adcq $0, %%rdx;\n\t" // abq_h = ((ab_h * q) >> 64)
                            // + carry
      "shrdq %%cl, %%rdx, %%rax;\n\t" // d = (a*b)/n =
                                      // (abq_m >> shift)
                                      // | (abq_h << (64
                                      // -shift))
      "mulq %[n];\n\t"                // d*n
      "subq %%rax, %[res];\n\t"       // ab_l - = dn_l
      : [res] "=&r"(res), [tmp] "=&r"(tmp)
      : [a] "%a"(a), [b] "d"(b), [q] "r"(q),
      [shift] "c"(shift), [n] "rm"(n)
      : "cc");
    return res; // in [0, 2n-1)
  }
  ull multi(ull a, ull b) {
    unsigned __int128 mult = a;
    mult *= b;
    ull prod =
      (((mult >> 64) * q) +
        ((unsigned __int128)(ull)mult * q >> 64)) >>
      shift;
    return (ull)mult - prod * n;
  }
  ull fix(ull a) {
    if (a >= n) a -= n;
    return a;
  }
  ull mult(ull a, ull b) { return fix(multi(a, b)); }
#endif
  //!unpause
  ull sqp1(ull a) { return multf(a, a) + 1; }
};
//!finish
//!start

ull pow_mod(ull a, ull n, ModArithm &arithm) {
  ull res = 1;
  for (ull i = 1; i <= n; i <<= 1) {
    if (n & i) res = arithm.multf(res, a);
    a = arithm.multf(a, a);
  }
  return res;
}
//!finish
//!start

vector<char> small_primes = {
  2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

bool is_prime(ull n) { // n <= 1<<63, 1M rand/s
  ModArithm arithm(n);
  if (n == 2 || n == 3) return true;
  if (!(n & 1) || n == 1) return false;
  int s = __builtin_ctzll(n - 1);
  ull d = (n - 1) >> s;
  for (ull a : small_primes) {
    if (a >= n) break;
    a = pow_mod(a, d, arithm);
    if (a == 1 || a == n - 1) continue;
    ran(r, 1, s) {
      a = arithm.multf(a, a);
      if (a == 1) return false;
      if (a == n - 1) break;
    }
    if (a != n - 1) return false;
  }
  return true;
}
//!finish
//!pause

#ifdef ONLINE_JUDGE
ll __gcd(ll a, ll b) {
  if (a) return __gcd(b % a, a);
  return b;
}
#endif

//!unpause
//!start

ll pollard_rho(ll n) {
  ModArithm arithm(n);
  int cum_cnt = 64 - __builtin_clzll(n);
  cum_cnt *= cum_cnt / 5 + 1;
  while (true) {
    ll lv = rand() % n;
    ll v = arithm.sqp1(lv);
    int idx = 1;
    int tar = 1;
    while (true) {
      ll cur = 1;
      ll v_cur = v;
      int j_stop = min(cum_cnt, tar - idx);
      for (int j = 0; j < j_stop; ++j) {
        cur = arithm.multf(cur, abs(v_cur - lv));
        v_cur = arithm.sqp1(v_cur);
        ++idx;
      }
      //!pause
      // cur = arithm.fix(cur);
      //!unpause
      if (!cur) {
        for (int j = 0; j < cum_cnt; ++j) {
          ll g = __gcd(abs(v - lv), n);
          if (g == 1) {
            v = arithm.sqp1(v);
          } else if (g == n) {
            break;
          } else {
            return g;
          }
        }
        break;
      } else {
        ll g = __gcd(cur, n);
        if (g != 1) return g;
      }
      v = v_cur;
      idx += j_stop;
      if (idx == tar) {
        lv = v;
        tar *= 2;
        v = arithm.sqp1(v);
        ++idx;
      }
    }
  }
}
//!finish
//!start


map<ll, int> prime_factor(
      ll n, map<ll, int> *res = NULL) {
  // n <= 1<<62, ~1000/s (<500/s on CF)
  if (!res) {
    map<ll, int> res_act;
    for (int p : small_primes) {
      while (!(n % p)) {
        ++res_act[p];
        n /= p;
      }
    }
    if (n != 1) prime_factor(n, &res_act);
    return res_act;
  }
  if (is_prime(n)) {
    ++(*res)[n];
  } else {
    ll factor = pollard_rho(n);
    prime_factor(factor, res);
    prime_factor(n / factor, res);
  }
  return map<ll, int>();
} // Usage: fact = prime_factor(n);
//!finish
//!end_codebook

int main() {
  std::default_random_engine gen;
  const ull mul_lim = ((ull)1 << 63);
#if 0
#if 1
  cout<<"test 1.1\n";
  {
    ull n = mul_lim-1;
    ModArithm arithm(n);
    for(int a = 2; a<100; ++a){
      for(int b = 2; b<100; ++b){
        ull ra = mul_lim-a;
        ull rb = mul_lim-b;
        ull c = arithm.mult(ra, rb);
        if(c != (ull)((__int128)(ra)*(rb)%n)){
          cout<<c<<' '<<a<<' '<<b<<'\n';
          return 1;
        }
      }
    }
  }
#endif
#if 1
  cout<<"test 1.2\n";
  {
    ull n = mul_lim/4-1;
    ModArithm arithm(n);
    for(int a = 1; a<100; ++a){
      for(int b = 1; b<100; ++b){
        ull ra = 2*n-a;
        ull rb = 2*n-b;
        ull c = arithm.mult(ra, rb);
        if(c != (ull)((__int128)(ra)*(rb)%n)){
          cout<<c<<' '<<a<<' '<<b<<'\n';
          return 1;
        }
      }
    }
  }
#endif
#if 1
  cout<<"test 1.3\n";
  for(int n=1; n<=200; ++n){
    ModArithm arithm(n);
    for(int a = 0; a<2*n; ++a){
      for(int b = 0; b<2*n; ++b){
        ull c = arithm.mult(a, b);
        if(c != (ull)((__int128)a*b%n)){
          cout<<n<<' '<<a<<' '<<b<<'\n';
          cout<<arithm.q<<' '<<(int)arithm.shift<<'\n';
          cout<<c<<' '<<(ull)((__int128)a*b/n)<<' '<<(ull)((__int128)a*b%n)<<'\n';
          return 1;
        }
      }
    }
  }
#endif
#if 1
  cout<<"test 1.4\n";
  ull n = mul_lim;
  ModArithm arithm(n);
  for(int j=0; j<1e6; ++j){
    std::uniform_int_distribution<ull> distr2(1, mul_lim-1);
    ull a = distr2(gen);
    ull b = distr2(gen);
    ull c = arithm.mult(a, b);
    if(c != (ull)((__int128)a*b%n)){
      cout<<' '<<j<<'\n';
      cout<<n<<' '<<a<<' '<<b<<'\n';
      cout<<arithm.q<<' '<<(int)arithm.shift<<'\n';
      cout<<c<<' '<<(ull)((__int128)a*b/n)<<' '<<(ull)((__int128)a*b%n)<<'\n';
      return 1;
    }
  }
#endif
#if 1
  cout<<"test 1.5\n";
  {
    std::uniform_int_distribution<ull> distr(1, 1LL<<61);
    for(int i=0; i<1e5; ++i){
      ull n = distr(gen);
      ModArithm arithm(n);
      for(int j=0; j<1e2; ++j){
        std::uniform_int_distribution<ull> distr2(1, min(mul_lim, n-1));
        ull a = distr2(gen);
        ull b = distr2(gen);
        ull c = arithm.mult(a, b);
        if(c != (ull)((__int128)a*b%n)){
          cout<<i<<' '<<j<<'\n';
          cout<<n<<' '<<a<<' '<<b<<'\n';
          cout<<arithm.q<<' '<<(int)arithm.shift<<'\n';
          cout<<c<<' '<<(ull)((__int128)a*b/n)<<' '<<(ull)((__int128)a*b%n)<<'\n';
          return 1;
        }
      }
    }
  }
#endif
#if 1
  cout<<"test 1.6\n";
  std::uniform_int_distribution<ull> distrc(1, 1LL<<35);
  for(int i=0; i<1e5; ++i){
    ull n = distrc(gen);
    ModArithm arithm(n);
    for(int j=0; j<1e2; ++j){
      std::uniform_int_distribution<ull> distr2(1, min(mul_lim, 2*n-1));
      ull a = distr2(gen);
      ull b = distr2(gen);
      ull c = arithm.mult(a, b);
      if(c != (ull)((__int128)a*b%n)){
        cout<<i<<' '<<j<<'\n';
        cout<<n<<' '<<a<<' '<<b<<'\n';
        cout<<arithm.q<<' '<<(int)arithm.shift<<'\n';
        cout<<c<<' '<<(ull)((__int128)a*b/n)<<' '<<(ull)((__int128)a*b%n)<<'\n';
        return 1;
      }
    }
  }
#endif
#endif
#if 0
  cout<<"test 2.1\n";
  {
    int lim = 3e5;
    vector<bool> sieve(lim, true);
    for(int i=2; i<lim; ++i){
      assert(is_prime(i) == sieve[i]);
      if(sieve[i]){
        for(int j=2*i; j<lim; j+=i){
          sieve[j] = false;
        }
      }
    }
    set<int> offset63 = {	25, 165, 259, 301, 375, 387, 391, 409, 457, 471 };
    for(int cur = 0; cur<=471; ++cur){
      assert(is_prime(((ull)1<<63)-cur) == offset63.count(cur));
    }
  }
#endif
#if 0
  cout<<"test 2.2\n";
  {
    int lim = 1.8e6;
    int cnt = 0;
    for(int i=0; i<lim; ++i){
      cnt += is_prime(((ull)1<<63)-i);
    }
    cout<<(double)lim/cnt<<'\n';
  }
#endif
#if 0
  cout<<"test 3.1\n";
  {
    int lim = 3e5;
    for(ll i=1; i<=lim; ++i){
      map<ll, int> factorization = prime_factor(i);
      ll c = 1;
      for(auto cur : factorization){
        assert(is_prime(cur.first));
        for(int j=0; j < cur.second; ++j){
          c *= cur.first;
        }
      }
      assert(c==i);
    }
  }
#endif
#if 0
  cout << "test 3.2\n";
  ull last = 1.5e9;
  while (!is_prime(last)) {
    ++last;
  }
  for (int i = 0; i < 1500; ++i) {
    ull nxt = last + 1;
    while (!is_prime(nxt)) {
      ++nxt;
    }
    ull cor = last * nxt;
    map<ll, int> factorization = prime_factor(cor);
    assert(factorization.size() == 2);
    ull cand = 1;
    for (auto cur : factorization) {
      assert(is_prime(cur.first));
      assert(cur.second == 1);
      cand *= cur.first;
    }
    assert(cor == cand);
    last = nxt;
  }
#endif
#if 0
  cout<<"test 3.3\n";
  for(int i=0; i<2e4; ++i){
    ull cor = rand()|((ull)rand()<<32);
    cor &= (1LL<<61)-1;
    map<ll, int> factorization = prime_factor(cor);
    ull cand = 1;
    for(auto cur : factorization){
      assert(is_prime(cur.first));
      for(int j=0; j<cur.second; ++j){
        cand *= cur.first;
      }
    }
    assert(cor==cand);
  }
#endif
#if 0
  cout<<"test 3.4\n";
  for(int i=0; i<2e4; ++i){
    ull cor = (1LL<<61)-i;
    map<ll, int> factorization = prime_factor(cor);
    ull cand = 1;
    for(auto cur : factorization){
      assert(is_prime(cur.first));
      for(int j=0; j<cur.second; ++j){
        cand *= cur.first;
      }
    }
    assert(cor==cand);
  }
#endif
#if 1
  cout<<"test 3.5\n";
  {
    int cnt = 100;
    vector<ll> primes;
    for(ll i = 2e9; primes.size()<cnt; --i){
      if(is_prime(i)){
        primes.push_back(i);
      }
    }
    ran(i, 0, primes.size()){
      ran(j, i, primes.size()){
        map<ll, int> factorization = prime_factor(primes[i]*primes[j]);
        assert(factorization.size() == 2 || factorization[primes[i]] == 2);
      }
    }
  }
#endif
}
