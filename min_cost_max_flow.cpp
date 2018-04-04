#include <bits/stdc++.h>

using namespace std;

typedef long long               ll;

//!escape \section{Min Cost Max Flow with succesive dijkstra $\mathcal{O}(\text{flow} \cdot n^2)$}

//!begin_codebook
//!start
const int nmax=1055;
const ll inf=1e14;
int t, n, v; //0 is source, v-1 sink
ll rem_flow[nmax][nmax]; //set [x][y] for directed capacity from x to y.
ll cost[nmax][nmax]; //set [x][y] for directed cost from x to y. SET TO inf IF NOT USED
ll min_dist[nmax];
int prev_node[nmax];
ll node_flow[nmax];
bool visited[nmax];
ll tot_cost, tot_flow; //output
void min_cost_max_flow(){ 
	tot_cost=0;							//Does not work with negative cycles.
	tot_flow=0;
	ll sink_pot=0;
  min_dist[0] = 0;
//!finish
//!start
  for(int i=1; i<=v; ++i){ //incase of no negative edges Bellman-Ford can be removed.
    min_dist[i]=inf;
  }
  for(int i=0; i<v-1; ++i){
    for(int j=0; j<v; ++j){
      for(int k=0; k<v; ++k){
        if(rem_flow[j][k] > 0 && min_dist[j]+cost[j][k] < min_dist[k]){
          min_dist[k] = min_dist[j]+cost[j][k];
        }
      }
    }
  }
  for(int i=0; i<v; ++i){ 	//Apply potentials to edge costs.
    for(int j=0; j<v; ++j){
      if(cost[i][j]!=inf){
        cost[i][j]+=min_dist[i];
        cost[i][j]-=min_dist[j];
      }
    }
  }
  sink_pot+=min_dist[v-1]; //Bellman-Ford end.
//!finish
//!start
	while(true){
		for(int i=0; i<=v; ++i){ //node after sink is used as start value for Dijkstra.
			min_dist[i]=inf;
			visited[i]=false;
		}
		min_dist[0]=0;
		node_flow[0]=inf;
		int min_node;
		while(true){ //Use Dijkstra to calculate potentials
			int min_node=v;
			for(int i=0; i<v; ++i){
				if((!visited[i]) && min_dist[i]<min_dist[min_node]){
					min_node=i;
				}
			}
			if(min_node==v){
				break;
			}
			visited[min_node]=true;
			for(int i=0; i<v; ++i){
				if((!visited[i]) && min_dist[min_node]+cost[min_node][i] < min_dist[i]){
					min_dist[i]=min_dist[min_node]+cost[min_node][i];
					prev_node[i]=min_node;
					node_flow[i]=min(node_flow[min_node], rem_flow[min_node][i]);
				}
			}
		}
		if(min_dist[v-1]==inf){
			break;
		}
		for(int i=0; i<v; ++i){ 	//Apply potentials to edge costs.
			for(int j=0; j<v; ++j){ //Found path from source to sink becomes 0 cost.
				if(cost[i][j]!=inf){
					cost[i][j]+=min_dist[i];
					cost[i][j]-=min_dist[j];
				}
			}
		}
		sink_pot+=min_dist[v-1];
		tot_flow+=node_flow[v-1];
		tot_cost+=sink_pot*node_flow[v-1];
		int cur=v-1;
		while(cur!=0){ //Backtrack along found path that now has 0 cost.
			rem_flow[prev_node[cur]][cur]-=node_flow[v-1];
			rem_flow[cur][prev_node[cur]]+=node_flow[v-1];
			cost[cur][prev_node[cur]]=0;
			if(rem_flow[prev_node[cur]][cur]==0){
				cost[prev_node[cur]][cur]=inf;
			}
			cur=prev_node[cur];
		}
	}
}
//!finish
//!end_codebook
//  http://www.spoj.com/problems/GREED/
int main(){
	cin>>t;
	for(int i=0; i<t; ++i){
		cin>>n;
		for(int j=0; j<nmax; ++j){
			for(int k=0; k<nmax; ++k){
				cost[j][k]=inf;
				rem_flow[j][k]=0;
			}
		}
		for(int j=1; j<=n; ++j){
			cost[j][2*n+1]=0;
			rem_flow[j][2*n+1]=1;
		}
		for(int j=1; j<=n; ++j){
			int card;
			cin>>card;
			++rem_flow[0][card];
			cost[0][card]=0;
		}
		int ex_c;
		cin>>ex_c;
		for(int j=0; j<ex_c; ++j){
			int a, b;
			cin>>a>>b;
			if(b<a){
				swap(a,b);
			}
			cost[a][b]=1;
			rem_flow[a][b]=nmax;
			cost[b][n+b]=0;
			rem_flow[b][n+b]=nmax;
			cost[n+b][a]=1;
			rem_flow[n+b][a]=nmax;
		}
		v=2*n+2;
		min_cost_max_flow();
		cout<<tot_cost<<'\n';
		//cout<<tot_flow<<'\n';
	}
	return 0;
}
