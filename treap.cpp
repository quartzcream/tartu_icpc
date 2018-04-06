#include<iostream>
#include<deque>
#include<random>
#include<tuple>
using namespace std;

//!escape \section{Treap $\mathcal{O}(\log n)$ per query}
//!begin_codebook
//!start
mt19937 randgen;

struct Treap {
    struct Node {
        int key;
        int value;
        unsigned int priority;
        long long total;
        
        Node* lch;
        Node* rch;
        
        Node(int new_key, int new_value) {
            key = new_key;
            value = new_value;
            priority = randgen();
            total = new_value;
            lch = 0;
            rch = 0;
        }
        void update() {
            total = value;
            if(lch) total += lch->total;
            if(rch) total += rch->total;
        }
    };
    
    deque<Node> nodes;
    Node* root = 0;
    
    pair<Node*, Node*> split(int key, Node* cur) {
        if(cur == 0) {
            return {0, 0};
        }
        
        pair<Node*, Node*> result;
        if(key <= cur->key) {
            auto ret = split(key, cur->lch);
            cur->lch = ret.second;
            result = {ret.first, cur};
        } else {
            auto ret = split(key, cur->rch);
            cur->rch = ret.first;
            result = {cur, ret.second};
        }
        cur->update();
        return result;
    }
    
    Node* merge(Node* left, Node* right) {
        if(left == 0) return right;
        if(right == 0) return left;
        
        Node* top;
        if(left->priority < right->priority) {
            left->rch = merge(left->rch, right);
            top = left;
        } else {
            right->lch = merge(left, right->lch);
            top = right;
        }
        top->update();
        return top;
    }
    
    void insert(int key, int value) {
        nodes.push_back(Node(key, value));
        Node* cur = &nodes.back();
        
        pair<Node*, Node*> ret = split(key, root);
        cur = merge(ret.first, cur);
        cur = merge(cur, ret.second);
        root = cur;
    }
    
    void erase(int key) {
        Node *left, *mid, *right;
        
        tie(left, mid) = split(key, root);
        tie(mid, right) = split(key+1, mid);
        root = merge(left, right);
    }
    
    long long sum_upto(int key, Node* cur) {
        if(cur == 0) {
            return 0;
        }
        
        if(key <= cur->key) {
            return sum_upto(key, cur->lch);
        } else {
            long long result = cur->value + sum_upto(key, cur->rch);
            if(cur->lch) result += cur->lch->total;
            return result;
        }
    }
    
    long long get(int l, int r) {
        return sum_upto(r+1, root) - sum_upto(l, root);
    }
};

//Solution for: http://codeforces.com/group/UO1GDa2Gwb/contest/219104/problem/TREAP
int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    
    int m;
    Treap treap;
    
    cin >> m;
    for(int i=0;i<m;i++) {
        int type;
        cin >> type;
        
        if(type == 1) {
            int x, y;
            cin >> x >> y;
            treap.insert(x, y);
        } else if(type == 2) {
            int x;
            cin >> x;
            treap.erase(x);
        } else {
            int l, r;
            cin >> l >> r;
            cout << treap.get(l, r) << endl;
        }
    }
    
    return 0;
}
//!finish
//!end_codebook

