//----------------------------------------------------------------
//OMLBEGIN: Successive Shortest Paths Min-Cost Max-Flow approach, uses Dinic
#include<vector>
#include<deque>
#include<algorithm>
#include<functional>
#include<tuple>
#include<climits>
#include<queue>
using namespace std;


struct Network {
	struct Node;
	
	struct Edge {
		Node *u, *v;
		int f, c, cost;
		
		Node* from(Node* pos) {
			if(pos == u)
				return v;
			return u;
		}
		int getCap(Node* pos) {
			if(pos == u)
				return c-f;
			return f;
		}
		int getCost(Node* pos) {
			if(pos == u)
				return cost;
			return -cost;
		}
		int addFlow(Node* pos, int toAdd) {
			if(pos == u) {
				f += toAdd;
				return toAdd * getCost(pos);
			}
			else {
				f -= toAdd;
				return -toAdd * getCost(pos);
			}
		}
		
	};
	struct Node {
		vector<Edge*> conn;
		int index;
	};
	
	deque<Node> nodes;
	deque<Edge> edges;
	
	Node* addNode() {
		nodes.push_back(Node());
		nodes.back().index = nodes.size()-1;
		return &nodes.back();
	}
	Edge* addEdge(Node* u, Node* v, int f, int c, int cost) {
		edges.push_back({u, v, f, c, cost});
		u->conn.push_back(&edges.back());
		v->conn.push_back(&edges.back());
		return &edges.back();
	}
	
	
	//Dinic that fills the zero-cost path
	int dinic_minmax(Node* source, Node* sink, vector<int> &pot) {
		int n = nodes.size();
		int result = 0;
		
		auto getPotCost = [&](Node* node, Edge* edge) {
			if(node == edge->u)
				return pot[edge->u->index] - pot[edge->v->index] + edge->getCost(node);
			return pot[edge->v->index] - pot[edge->u->index] + edge->getCost(node);
		};
		
		
		while(1) {
			//First divide into layers
			vector<int> level(n, -1);
			{
				deque<tuple<int, Node*> > que(1, make_tuple(0, source) );
				level[source->index] = 0;
				
				while(que.size() > 0) {
					int lev; Node* cur;
					tie(lev, cur) = que.front();
					que.pop_front();
					
					for(auto ep : cur->conn) if(ep->getCap(cur) > 0 && getPotCost(cur, ep) == 0) {
						Node* next = ep->from(cur);
						if(level[next->index] == -1) {
							level[next->index] = lev+1;
							que.push_back(make_tuple(lev+1, next));
						}
					}
				}
				
				if(level[sink->index] == -1)
					return result;
			}
			
			//Now perform dfs to saturate the edges
			vector<int> connIndex(n, 0);
			
			function<int (Node*, int)> dfs = [&](Node* cur, int cap) {
				if(cur == sink)
					return cap;
				
				for(int& i = connIndex[cur->index]; i < (int)cur->conn.size(); i++) {
					Edge* edge = cur->conn[i]; Node* next = edge->from(cur);
					if(edge->getCap(cur) > 0 && getPotCost(cur, edge) == 0 && level[cur->index] < level[next->index]) {
						int ret = dfs(next, min(cap, edge->getCap(cur) ) );
						if(ret > 0) {
							edge->addFlow(cur, ret);
							return ret;
						}
					}
				}
				return 0;
			};
			int ret;
			do {
				ret = dfs(source, INT_MAX);
				result += ret;
			}while(ret > 0);
		}
	}
	
	//If there is an negative cost cycle initially, then it goes into infinite loop
	int minCostMaxFlow(Node* source, Node* sink) {
		//Augment in a loop
		int result = 0, n = nodes.size();
		
		while(1) {
			//First calculate the potentials with Belleman-Ford derivative
			vector<int> pot(n, INT_MAX);
			{
					
				deque<pair<int, Node*> > front;
				front.push_back({0, source});
				
				while(front.size() > 0) {
					int p;
					Node* cur;
					tie(p, cur) = front.front();
					front.pop_front();
					
					if(p >= pot[cur->index])
						continue;
					pot[cur->index] = p;
					
					for(Edge* edge : cur->conn) if(edge->getCap(cur) > 0) {
						front.push_back({p + edge->getCost(cur), edge->from(cur)});
					}
				}
			}
			
			//Next find the zero cost flow
			{
				int ret = dinic_minmax(source, sink, pot);
				result += ret * (pot[sink->index] - pot[source->index]);
				if(ret == 0)
					return result;
			}
		}
	}
	
};

//OMLBEGIN: Successive Shortest Paths Min-Cost Max-Flow approach, uses Dinic
//----------------------------------------------------------------