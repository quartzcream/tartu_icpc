#include <bits/stdc++.h>
using namespace std;
typedef long long               ll;
const int mod = 1e9+7;
#ifndef M_PI
const double M_PI = acos(-1.0);
#endif

//!escape \section{Generic persistent compressed lazy segment tree}

//!begin_codebook
//!start

struct Seg{
	ll sum=0;
	void recalc(const Seg &lhs_seg, int lhs_len, const Seg &rhs_seg, int rhs_len){
		sum = lhs_seg.sum + rhs_seg.sum;
	}
} __attribute__((packed));

struct Lazy{
	ll add;
	ll assign_val; //LLONG_MIN if no assign;
	void init(){
		add = 0;
		assign_val = LLONG_MIN;
	}
	Lazy(){ init(); }
	void split(Lazy &lhs_lazy, Lazy &rhs_lazy, int len){
    lhs_lazy = *this;
    rhs_lazy = *this;
    init();
  }
  void merge(Lazy &oth, int len){
		if(oth.assign_val != LLONG_MIN){
			add = 0;
      assign_val = oth.assign_val;
    }
    add += oth.add;
  }
	void apply_to_seg(Seg &cur, int len) const{
		if(assign_val != LLONG_MIN){
			cur.sum =	len * assign_val;
		}
		cur.sum += len * add;
	} 
} __attribute__((packed));

//!finish
//!start

struct Node{ //Following code should not need to be modified
  int ver;
  bool is_lazy = false;
  Seg seg;
  Lazy lazy;
	Node *lc=NULL, *rc=NULL;
	void init(){
		if(!lc){
			lc = new Node {ver};
			rc = new Node {ver};
		}
	}
  Node *upd(int L, int R, int l, int r, Lazy &val, int tar_ver){
    if(ver != tar_ver){
      Node *rep = new Node(*this);
      rep->ver = tar_ver;
      return rep->upd(L, R, l, r, val, tar_ver);
    }
    if(L >= l && R <= r){
      val.apply_to_seg(seg, R-L);
      lazy.merge(val, R-L);
      is_lazy = true;
    } else {
      init();
      int M = (L+R)/2;
      if(is_lazy){
        Lazy l_val , r_val;
        lazy.split(l_val, r_val, R-L);
        lc = lc->upd(L, M, L, M, l_val, ver);
        rc = rc->upd(M, R, M, R, r_val, ver);
        is_lazy = false;
      }
      Lazy l_val , r_val;
      val.split(l_val, r_val, R-L);
      if(l < M)
        lc = lc->upd(L, M, l, r, l_val, ver);
      if(M < r)
        rc = rc->upd(M, R, l, r, r_val, ver);
      seg.recalc(lc->seg, M-L, rc->seg, R-M);
    }
    return this;
  }
  void get(int L, int R, int l, int r, Seg *&lft_res, Seg *&tmp, bool last_ver){
    if(L >= l && R <= r){
      tmp->recalc(*lft_res, L-l, seg, R-L);
      swap(lft_res, tmp);
    } else {
      init();
      int M = (L+R)/2;
      if(is_lazy){
        Lazy l_val , r_val;
        lazy.split(l_val, r_val, R-L);
        lc = lc->upd(L, M, L, M, l_val, ver+last_ver);
        lc->ver = ver;
        rc = rc->upd(M, R, M, R, r_val, ver+last_ver);
        rc->ver = ver;
        is_lazy = false;
      }
      if(l < M)
        lc->get(L, M, l, r, lft_res, tmp, last_ver);
      if(M < r)
        rc->get(M, R, l, r, lft_res, tmp, last_ver);
    }
  }
} __attribute__((packed));

struct SegTree{ //indexes start from 0, ranges are [beg, end)
  vector<Node *> roots; //versions start from 0
  int len;
  SegTree(int _len) : len(_len){
    roots.push_back(new Node {0});
  }
  int upd(int l, int r, Lazy &val, bool new_ver = false){
    Node *cur_root = roots.back()->upd(0, len, l, r, val, roots.size()-!new_ver);
    if(cur_root != roots.back())
      roots.push_back(cur_root);
    return roots.size()-1;
  }
  Seg get(int l, int r, int ver = -1){
    if(ver == -1)
      ver = roots.size()-1;
    Seg seg1, seg2;
    Seg *pres = &seg1, *ptmp = &seg2;
    roots[ver]->get(0, len, l, r, pres, ptmp, roots.size()-1);
    return *pres;
  }
};

//!finish
//!end_codebook
int val_rand(){ 
  return (rand()-RAND_MAX/2)%100000;
}

//!begin_codebook
int main(){
//!end_codebook
#if 0
  for(int n=1; n<64; ++n){
    for(int m=1; m<100; ++m){
      for(int t=0; t<99; ++t){
        cout<<t<<'\n';
        cout<<n<<'\n'<<m<<'\n';
        SegTree seg_tree(7);
        vector< vector<ll> > brute(1, vector<ll>(n));
        for(int i=0; i<n; ++i){
          Lazy tmp;
          int v = val_rand();
          tmp.assign_val = v;
          cout<<v<<' ';
          assert(seg_tree.upd(i, i+1, tmp) == 0);
          brute.back()[i] = v;
        }
        cout<<'\n';
        for(int i=0; i<m; ++i){
          int o = rand()%3 + 1;
          int l = rand()%n;
          int r;
          while((r = rand()%n + 1) <= l) {}
          cout<<o<<' '<<l+1<<' '<<r;
          if(o==1){
            Lazy tmp;
            int d = val_rand();
            bool new_ver = rand()&1;
            tmp.add = d;
            cout<<' '<<d<<' '<<new_ver;
            if(new_ver){
              brute.push_back(brute.back());
            }
            for(int j=l; j<r; ++j){
              brute.back()[j] += d;
            }
            assert(seg_tree.upd(l, r, tmp, new_ver) == brute.size()-1);
          } else if(o==2){
            Lazy tmp;
            int x = val_rand();
            tmp.assign_val = x;
            bool new_ver = rand()&1;
            cout<<' '<<x<<' '<<new_ver;
            if(new_ver){
              brute.push_back(brute.back());
            }
            for(int j=l; j<r; ++j){
              brute.back()[j] = x;
            }
            assert(seg_tree.upd(l, r, tmp, new_ver) == brute.size()-1);
          } else {
            int v = rand()%brute.size();
            cout<<' '<<v;
            Seg whole_res = seg_tree.get(l, r, v);
            ll res = whole_res.sum;
            ll cor_res = 0;
            for(int j=l; j<r; ++j){
              cor_res += brute[v][j];
            }
            if(res != cor_res){
              cout<<endl;
              cerr<<"Is: "<<res<<", should be: "<<cor_res<<endl;
              assert(res == cor_res);
            }
          }
          cout<<'\n';
        }
        cout<<'\n'<<'\n';
      }
    }
  }
#endif
#if 0
  SegTree seg_tree(20);
  ll res = 0;
  int n = 1e5;
  int m= 1e6;
  for(int i=0; i<n; ++i){
    Lazy tmp;
    int v = val_rand();
    tmp.assign_val = v;
    seg_tree.upd(i, i+1, tmp);
  }
  for(int i=0; i<m; ++i){
    int o = rand()%3 + 1;
    int l = rand()%n;
    int r = rand()%(n-l)+l+1;
    if(o==1){
      Lazy tmp;
      int d = val_rand();
      tmp.add = d;
      seg_tree.upd(l, r, tmp);
    } else if(o==2){
      Lazy tmp;
      int x = val_rand();
      tmp.assign_val = x;
      seg_tree.upd(l, r, tmp);
    } else {
      Seg whole_res = seg_tree.get(l, r);
      res += whole_res.sum;
    }
  }
  cout<<res<<'\n';
#endif
#if 1
//!begin_codebook
  int n, m; //solves Mechanics Practice LAZY
  cin>>n>>m;
	SegTree seg_tree(1<<17);
  for(int i=0; i<n; ++i){
    Lazy tmp;
    scanf("%lld", &tmp.assign_val);
    seg_tree.upd(i, i+1, tmp);
  }
  for(int i=0; i<m; ++i){
    int o;
    int l, r;
    scanf("%d %d %d", &o, &l, &r);
    --l;
    if(o==1){
      Lazy tmp;
      scanf("%lld", &tmp.add);
      seg_tree.upd(l, r, tmp);
    } else if(o==2){
      Lazy tmp;
      scanf("%lld", &tmp.assign_val);
      seg_tree.upd(l, r, tmp);
    } else {
      Seg res = seg_tree.get(l, r);
      printf("%lld\n",res.sum);
    }
  }
//!end_codebook
#endif
//!begin_codebook
}
//!end_codebook
