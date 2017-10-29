//----------------------------------------------------------------
//OMLBEGIN: Segment Trees with Lazy Propagation
#include<vector>
using namespace std;

//T needs functions add, multiply, combine and operator()
template<class T>
struct LazySegmentTree {
	struct Node {
		Node *lch, *rch;
		T value;
		T *lazyAdd, *lazySet;
		int size;
		
		void setMark(T val) {
			if(lazyAdd != 0) delete lazyAdd;
			lazyAdd = 0;
			if(lazySet != 0) delete lazySet;
			lazySet = new T(val);
		}
		void addMark(T val) {
			if(lazyAdd != 0) *lazyAdd = lazyAdd->add(val);
			else lazyAdd = new T(val);
		}
		
		void propagate() {
			if(lazySet) {
				T toSet = *lazySet;
				value = toSet.multiply(size);
				if(lch != 0 && rch != 0) {
					lch->setMark(toSet);
					rch->setMark(toSet);
				}
				delete lazySet;
				lazySet = 0;
			}
			if(lazyAdd) {
				T toAdd = *lazyAdd;
				value = value.add(toAdd.multiply(size));
				if(lch != 0 && rch != 0) {
					lch->addMark(toAdd);
					rch->addMark(toAdd);
				}
				delete lazyAdd;
				lazyAdd = 0;
			}
		}
		
		T getValue() {
			propagate();
			return value;
		}
		
		void update() {
			value = lch->getValue().combine(rch->getValue());
		}
	};
	vector<Node> nodes;
	int n;
	
	LazySegmentTree(int nsize, vector<T> init = vector<T>()) {
		n = 1;
		while(n < nsize)
			n *= 2;
		
		nodes.resize(2*n-1, {0, 0, T(), 0, 0, 1});
		
		for(int i=0;i<n;i++) {
			if(i < (int)init.size())
				nodes[n-1+i].value = init[i];
		}
		
		for(int i=n-2;i>=0;i--) {
			nodes[i].value = nodes[2*i+1].value.combine(nodes[2*i+2].value);
			nodes[i].lch = &nodes[2*i+1];
			nodes[i].rch = &nodes[2*i+2];
			nodes[i].size = nodes[i].lch->size + nodes[i].rch->size;
		}
	}
	
	void add(int l, int r, T toAdd, Node* cur = 0, int lb = 0, int rb = -1) {
		if(cur == 0) {
			cur = &nodes[0];
			rb = n;
		}
		cur->propagate();
		
		if(l <= lb && rb <= r) {
			cur->addMark(toAdd);
			return;
		}
		int mid = (lb + rb) / 2;
		if(l < mid)
			add(l, r, toAdd, cur->lch, lb, mid);
		if(mid < r)
			add(l, r, toAdd, cur->rch, mid, rb);
		cur->update();
	}
	
	void set(int l, int r, T toSet, Node* cur = 0, int lb = 0, int rb = -1) {
		if(cur == 0) {
			cur = &nodes[0];
			rb = n;
		}
		cur->propagate();
		
		if(l <= lb && rb <= r) {
			cur->setMark(toSet);
			return;
		}
		int mid = (lb + rb) / 2;
		if(l < mid)
			set(l, r, toSet, cur->lch, lb, mid);
		if(mid < r)
			set(l, r, toSet, cur->rch, mid, rb);
		cur->update();
	}
	
	T get(int l, int r, Node* cur = 0, int lb = 0, int rb = -1) {
		if(cur == 0) {
			cur = &nodes[0];
			rb = n;
		}
		cur->propagate();
		
		if(l <= lb && rb <= r)
			return cur->getValue();
		
		vector<T> ret;
		int mid = (lb + rb) / 2;
		if(l < mid)
			ret.push_back(get(l, r, cur->lch, lb, mid) );
		if(mid < r)
			ret.push_back(get(l, r, cur->rch, mid, rb) );
		
		if(ret.size() == 2) return ret[0].combine(ret[1]);
		return ret[0];
	}
};

//OMLEND: Segment Trees with Lazy Propagation
//----------------------------------------------------------------