//----------------------------------------------------------------
//OMLBEGIN: Flow Network
#include<vector>
#include<deque>
#include<functional>
#include<tuple>
#include<climits>
using namespace std;


struct Network {
	struct Node;
	
	struct Edge {
		int index;
		Node *u, *v;
		int cap, flow;
		
		Node* from(Node* pos) {
			if(pos == u) return v;
			return u;
		}
		void addFlow(Node* pos, int toAdd) {
			if(pos == u) flow += toAdd;
			else flow -= toAdd;
		}
		int getCap(Node* pos) {
			if(pos == u) return cap-flow;
			return flow;
		}
	};
	struct Node {
		int index;
		vector<Edge*> conn;
	};
	int n = 0, m = 0;
	deque<Node> nodes;
	deque<Edge> edges;
	
	Node* addNode() {
		nodes.push_back({n++});
		return &nodes.back();
	}
	Edge* addEdge(Node* u, Node* v, int cap = 1, int flow = 0) {
		edges.push_back({m++, u, v, cap, flow});
		u->conn.push_back(&edges.back());
		v->conn.push_back(&edges.back());
		return &edges.back();
	}
	
	int dinic(Node* source, Node* sink) {
		int result = 0;
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
					
					for(auto ep : cur->conn) if(ep->getCap(cur) > 0) {
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
					if(edge->getCap(cur) > 0 && level[cur->index] < level[next->index]) {
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
};
//OMLEND: Flow Network
//----------------------------------------------------------------