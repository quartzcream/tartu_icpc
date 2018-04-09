#include<iostream>
#include<vector>
using namespace std;

//!escape \section{Lazy Segment Tree $\mathcal{O}(\log n)$ per query}
//!begin_codebook
//!start
struct SegmentTree {
    struct Node {
        long long value = 0;
        int size = 1;
        
        int lazy_add = 0;
        bool lazy_set = false;
        int lazy_to_set = 0;
        
        void set(int to_set) {
            lazy_set = true;
            lazy_to_set = to_set;
            lazy_add = 0;
        }
    };
    int n;
    vector<Node> nodes;
    
    void propagate(int pos) {
        Node& cur = nodes[pos];
        if(cur.lazy_set) {
            if(pos < n) {
                nodes[pos*2].set(cur.lazy_to_set);
                nodes[pos*2+1].set(cur.lazy_to_set);
            }
            cur.value = 1LL * cur.size * cur.lazy_to_set;
            cur.lazy_set = false;
        }
        if(cur.lazy_add != 0) {
            if(pos < n) {
                nodes[pos*2].lazy_add += cur.lazy_add;
                nodes[pos*2+1].lazy_add += cur.lazy_add;
            }
            cur.value += 1LL * cur.size * cur.lazy_add;
            cur.lazy_add = 0;
        }
    }
    
    long long get_value(int pos) {
        propagate(pos);
        return nodes[pos].value;
    }
    
    SegmentTree(int nsize) {
        n = 1;
        while(n < nsize) n*=2;
        nodes.resize(2*n);
        for(int i=n-1; i>0; i--)
            nodes[i].size = nodes[2*i].size * 2;
    }
    
    void set(int l, int r, int to_set, int pos = 1, int lb = 0, int rb = -1) {
        propagate(pos);
        if(rb == -1) rb = n;
        if(l <= lb && rb <= r) {
            nodes[pos].set(to_set);
            return;
        }
        int mid = (lb + rb) / 2;
        if(l < mid)
            set(l, r, to_set, pos*2, lb, mid);
        if(mid < r)
            set(l, r, to_set, pos*2+1, mid, rb);
        nodes[pos].value = get_value(pos*2) + get_value(pos*2+1);
    }
    
    void add(int l, int r, int to_add, int pos = 1, int lb = 0, int rb = -1) {
        propagate(pos);
        if(rb == -1) rb = n;
        if(l <= lb && rb <= r) {
            nodes[pos].lazy_add += to_add;
            return;
        }
        int mid = (lb + rb) / 2;
        if(l < mid)
            add(l, r, to_add, pos*2, lb, mid);
        if(mid < r)
            add(l, r, to_add, pos*2+1, mid, rb);
        nodes[pos].value = get_value(pos*2) + get_value(pos*2+1);
    }
    
    long long get(int l, int r, int pos = 1, int lb = 0, int rb = -1) {
        propagate(pos);
        if(rb == -1) rb = n;
        if(l <= lb && rb <= r) return get_value(pos);
        int mid = (lb + rb) / 2;
        long long result = 0;
        if(l < mid)
            result += get(l, r, pos*2, lb, mid);
        if(mid < r)
            result += get(l, r, pos*2+1, mid, rb);
        return result;
    }
};
//!finish

//Solution for: http://codeforces.com/group/UO1GDa2Gwb/contest/219104/problem/LAZY
int main() {
//!end_codebook
    ios_base::sync_with_stdio(false);
    cin.tie(0);
//!begin_codebook
    
    int n, m;
    cin >> n >> m;
    
    SegmentTree stree(n);
    for(int i=0;i<n;i++) {
        int a;
        cin >> a;
        stree.set(i, i+1, a);
    }
    
    for(int i=0;i<m;i++) {
        int type;
        cin >> type;
        
        if(type == 1) {
            int l, r, d;
            cin >> l >> r >> d;
            stree.add(l-1, r, d);
        } else if(type == 2) {
            int l, r, x;
            cin >> l >> r >> x;
            stree.set(l-1, r, x);
        } else {
            int l, r;
            cin >> l >> r;
            cout << stree.get(l-1, r) << '\n';
        }
    }
    
}
//!end_codebook
