#include <bits/stdc++.h>
using namespace std;

//!escape Memorypool

//!begin_codebook
//!start
const int BLOCK = 8;
const int MEM_SIZE = 1 << 26;
char glob_buf[MEM_SIZE];
int glob_idx;
vector<bool> glob_used;

void init_mem() {
  glob_used.resize(MEM_SIZE / BLOCK);
  glob_used[0] = true;
}

template <typename T>
struct Ptr {
  unsigned idx;
  explicit Ptr(T *tar) { idx = (char *)tar - glob_buf; }
  Ptr() { idx = 0; }
  template <typename... Args>
  void construct(Args... args) {
    new (glob_buf + idx) T(args...);
  }
  T *operator->() {
    assert(idx);
    return (T *)(glob_buf + idx);
  }
  T &operator*() { return *operator->(); }
  bool operator==(const Ptr &oth) const {
    return idx == oth.idx;
  }
  operator unsigned() const { return idx; }
  Ptr &operator+=(int diff) {
    idx += diff * sizeof(T);
    return *this;
  }
  Ptr operator+(int diff) {
    Ptr res;
    res.idx = idx;
    return res += diff;
  }
  T &operator[](int diff) { return *operator+(diff); }
};

template <typename T, typename... Args>
Ptr<T> alloc(int n, Args... args) {
  unsigned len = 0; // TLE if running low on mem
  while (len < sizeof(T) * n) {
    if (!glob_idx) glob_idx = MEM_SIZE / BLOCK;
    if (glob_used[--glob_idx]) {
      len = 0;
    } else {
      len += BLOCK;
    }
  }
  ran(i, 0, ((int)sizeof(T) * n + BLOCK - 1) / BLOCK)
    glob_used[glob_idx + i] = true;
  Ptr<T> res;
  if (n) res.idx = BLOCK * glob_idx;
  ran(i, 0, n)(res + i).construct(args...);
  return res;
}

template <typename T>
void dealloc(Ptr<T> ptr, int n) {
  ran(i, 0, ((int)sizeof(T) * n + BLOCK - 1) / BLOCK)
    glob_used[ptr.idx / BLOCK + i] = false;
}

template <typename T>
struct hash<Ptr<T> > {
  std::size_t operator()(const Ptr<T> &cur) const {
    return cur.idx;
  }
};
//!finish
//!end_codebook
