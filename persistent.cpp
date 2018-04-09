#include <iostream>
#include <vector>

using namespace std;

const int MOD = 1000000007;

struct sum {
  int identity;

  sum () {
    identity = 0;
  }
  
  int operator() (int a, int b) {
    return (a + b) % MOD;
  }
};

//!escape \section{Templated Persitent Segment Tree $\mathcal{O}(\log n)$ per query}
//!begin_codebook
//!start
template<typename T, typename comp>
class PersistentST {
  struct Node {
    Node *left, *right;
    int lend, rend;
    T value;

    Node (int position, T _value) {
      left = NULL;
      right = NULL;
      lend = position;
      rend = position;
      value = _value;
    }

    Node (Node *_left, Node *_right) {
      left = _left;
      right = _right;
      lend = left->lend;
      rend = right->rend;
      value = comp()(left->value, right->value);
    }

    T query (int qleft, int qright) {
      qleft = max(qleft, lend);
      qright = min(qright, rend);

      if (qleft == lend && qright == rend) {
        return value;
      } else if (qleft > qright) {
        return comp().identity;
      } else {
        return comp()(left->query(qleft, qright), right->query(qleft, qright));
      }
    }
  };

  int size;
  Node **tree;
  vector<Node*> roots;
public:
  PersistentST () {}

  PersistentST (int _size, T initial) {
    for (int i = 0; i < 32; i++) {
      if ((1 << i) > _size) {
        size = 1 << i;
        break;
      }
    }
    
    tree = new Node* [2 * size + 5];

    for (int i = size; i < 2 * size; i++)
      tree[i] = new Node (i - size, initial);

    for (int i = size - 1; i > 0; i--)
      tree[i] = new Node (tree[2 * i], tree[2 * i + 1]);

    roots = vector<Node*> (1, tree[1]);
  }

  void set (int position, T _value) {
    tree[size + position] = new Node (position, _value);
    for (int i = (size + position) / 2; i >= 1; i /= 2)
      tree[i] = new Node (tree[2 * i], tree[2 * i + 1]);
    roots.push_back(tree[1]);
  }

  int last_revision () {
    return (int) roots.size() - 1;
  }

  T query (int qleft, int qright, int revision) {
    return roots[revision]->query(qleft, qright);
  }

  T query (int qleft, int qright) {
    return roots[last_revision()]->query(qleft, qright);
  }
};
//!finish
//!end_codebook
