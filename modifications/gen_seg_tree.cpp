#include <bits/stdc++.h>
using namespace std;
typedef long long               ll;
const int mod = 1e9+7;
#ifndef M_PI
const double M_PI = acos(-1.0);
#endif

//!begin_codebook
//!start
struct Segment{
	ll sum_val=0;
	ll min_val=0;
	void find_sum(int seg_len, ll &cur_sum){
		cur_sum = cur_sum + sum_val;
	}
	void find_min(int seg_len, ll &cur_min){
		cur_min = min(cur_min, min_val);
	}

	void recalc(int seg_len, const Segment &lhs_seg, const Segment &rhs_seg){
		sum_val = lhs_seg.sum_val + rhs_seg.sum_val;
		min_val = min(lhs_seg.min_val, rhs_seg.min_val);
	}
};

struct Lazy{
	ll add_val;
	ll assign_val; //LLONG\_MIN if no assign;

	void init(){
		add_val = 0;
		assign_val = LLONG_MIN;
	}
	Lazy(){ init(); }

  void apply_to_lazy(int seg_len, Lazy &child) const{
		if(assign_val != LLONG_MIN){
			child.add_val = 0;
      child.assign_val = assign_val;
    }
    child.add_val += add_val;
  }
	void apply_to_seg(int seg_len, Segment &cur) const{
		if(assign_val != LLONG_MIN){
			cur.min_val =	assign_val;
			cur.sum_val =	seg_len * assign_val;
		}
		cur.min_val += add_val;
		cur.sum_val += seg_len * add_val;
	} //Following code should not need to be modified
	void split(int seg_len, Lazy &lhs_lazy, Lazy &rhs_lazy){
    apply_to_lazy(seg_len, lhs_lazy); //Empty current and pass on to chidlren
    apply_to_lazy(seg_len, rhs_lazy);
    init();
	}
};

// Highly optimized generic segment tree with lazy propagation
class SegTree{ //indexes start from 0, ranges are [beg, end)
private:
	int offset;
	int height;
	Segment *segs;
	Lazy *lazys;
  vector<bool> is_lazy;
  void split(int len, int idx){
    is_lazy[idx] = false;
    lazys[idx].apply_to_seg(len/2, segs[2*idx]);
    lazys[idx].apply_to_seg(len/2, segs[2*idx+1]);
    lazys[idx].split(len/2, lazys[2*idx], lazys[2*idx+1]);
    is_lazy[2*idx] = true;
    is_lazy[2*idx+1] = true;
  }
	void push(int bot_idx){
		for(int s = height-1; s>0; --s){
			int idx = bot_idx>>s;
			if(is_lazy[idx]){ //Lazys can be below other lazys
        split(1<<s, idx);
			}
		}
	}
	void build(int len, int idx){
		for(; idx; len<<=1, idx>>=1){
			segs[idx].recalc(len, segs[2*idx], segs[2*idx+1]);
		}
	}
public:
	SegTree(int tree_size){
		offset = tree_size;
		height = 32 - __builtin_clz(tree_size);
		segs = new Segment[2*tree_size];
		lazys = new Lazy[2*tree_size];
		is_lazy.resize(2*tree_size, false);
	}
	~SegTree(){
		delete[] segs;
		delete[] lazys;
	}
	void modify(int l, int r, const Lazy &upd){
		l+=offset;
		r+=offset;
		push(l);
		push(r-1);
		int len = 1;
		for(int l_tmp = l, r_tmp = r; l_tmp<r_tmp; l_tmp >>= 1, r_tmp >>= 1, len <<= 1){
			if(l_tmp & 1){
        upd.apply_to_lazy(len, lazys[l_tmp]);
        upd.apply_to_seg(len, segs[l_tmp]);
				is_lazy[l_tmp] = true;
				++l_tmp;
			}
			if(r_tmp & 1){
				--r_tmp;
        upd.apply_to_lazy(len, lazys[r_tmp]);
        upd.apply_to_seg(len, segs[r_tmp]);
				is_lazy[r_tmp] = true;
			}
		}
		len = 1<<(__builtin_ctz(l)+1);
		l >>=  __builtin_ctz(l) + 1;
		build(len, l);
		len = 1<<(__builtin_ctz(r)+1);
		r >>= __builtin_ctz(r) + 1;
		build(len, r);
	}
	template< typename ...QueryArgs >
	void query(int l, int r, void (Segment::*query_func)(int, QueryArgs...), QueryArgs &&...query_args){
		l+=offset;
		r+=offset;
		push(l);
		push(r-1);
		int len = 1;
    int r_orig = r;
		for(; l< r; l>>=1, r>>=1, len <<= 1){ //Segments apllied in order to querry
			if(l & 1){
				(segs[l++].*query_func)(len, query_args...);
			}
		}
    for(;r < r_orig;){
      r<<=1;
      len>>=1;
      if(r_orig & len){
        (segs[r++].*query_func)(len, query_args...);
      }
    }
	}
};
//!finish
//!end_codebook
int val_rand(){ 
  return (rand()-RAND_MAX/2)%10;
}

//!begin_codebook
//!start
int main(){
//!finish
//!end_codebook
  int run_test;
  cin>>run_test;
  if(run_test){
    for(int n=1; n<30; ++n){
      for(int m=1; m<30; ++m){
        for(int t=0; t<1000; ++t){
          cout<<t<<'\n';
          cout<<n<<'\n'<<m<<'\n';
          SegTree seg_tree(n);
          vector<ll> brute(n);
          for(int i=0; i<n; ++i){
            Lazy tmp;
            int v = val_rand();
            tmp.assign_val = v;
            cout<<v<<' ';
            seg_tree.modify(i, i+1, tmp);
            brute[i] = v;
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
              tmp.add_val = d;
              cout<<' '<<d;
              seg_tree.modify(l, r, tmp);
              for(int j=l; j<r; ++j){
                brute[j] += d;
              }
            } else if(o==2){
              Lazy tmp;
              int x = val_rand();
              tmp.assign_val = x;
              cout<<' '<<x;
              seg_tree.modify(l, r, tmp);
              for(int j=l; j<r; ++j){
                brute[j] = x;
              }
            } else {
              ll res=0;
              seg_tree.query(l, r, &Segment::find_sum, res);
              ll cor_res = 0;
              for(int j=l; j<r; ++j){
                cor_res += brute[j];
              }
              if(res != cor_res){
                cout<<endl;
                cout<<"Is: "<<res<<", should be: "<<cor_res<<endl;
                assert(res == cor_res);
              }
            }
            cout<<'\n';
          }
          cout<<'\n'<<'\n';
        }
      }
    }
  }
//!begin_codebook
//!start
  int n, m; //solves Mechanics Practice LAZY
  cin>>n>>m;
	SegTree seg_tree(n);
  for(int i=0; i<n; ++i){
    Lazy tmp;
    scanf("%lld", &tmp.assign_val);
    seg_tree.modify(i, i+1, tmp);
  }
  for(int i=0; i<m; ++i){
    int o;
    int l, r;
    scanf("%d %d %d", &o, &l, &r);
    --l;
    if(o==1){
      Lazy tmp;
      scanf("%lld", &tmp.add_val);
      seg_tree.modify(l, r, tmp);
    } else if(o==2){
      Lazy tmp;
      scanf("%lld", &tmp.assign_val);
      seg_tree.modify(l, r, tmp);
    } else {
      ll res=0;
      seg_tree.query(l, r, &Segment::find_sum, res);
      printf("%lld\n",res);
    }
  }
}
//!finish
//!end_codebook
