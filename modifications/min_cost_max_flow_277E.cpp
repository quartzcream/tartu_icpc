#include <bits/stdc++.h>

using namespace std;

typedef long long               ll;

//!escape \section{Min Cost Max Flow with succesive dijkstra $\mathcal{O}(\text{flow} \cdot n^2)$}

//!begin_codebook
typedef double CostType;
const int nmax=1055;
const ll inf=1e14;
int t, n, v; //0 is source, v-1 sink
ll rem_flow[nmax][nmax]; //set [x][y] for directed capacity from x to y.
CostType cost[nmax][nmax]; //set [x][y] for directed cost from x to y. SET TO inf IF NOT USED
CostType min_dist[nmax];
int prev_node[nmax];
ll node_flow[nmax];
bool visited[nmax];
CostType tot_cost //output;
ll tot_flow; 
void min_cost_max_flow(){ //incase of negative edges have to add Bellman-Ford that is run once. 
    tot_cost=0;							//Does not work with negative cycles.
    tot_flow=0;
    CostType sink_pot=0;
    while(true){
        for(int i=0; i<=v; ++i){
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
//!end_codebook
//for http://www.spoj.com/problems/GREED/
int main(){
    for(int j=0; j<nmax; ++j){
        for(int k=0; k<nmax; ++k){
            cost[j][k]=inf;
            rem_flow[j][k]=0;
        }
    }
    int n;
    cin >> n;
    v = 2*n+2;
    vector<pair<int,int> > points(n);
    for(int i = 0; i < n; ++i)
        scanf("%d %d",&points[i].second,&points[i].first);
    sort(points.rbegin(),points.rend());
    for(int i = 1; i <= n; ++i){
        cost[0][i] = 0;
        rem_flow[0][i] = 2;
    }
    for(int i = n+2; i <= 2*n; ++i){
        cost[i][v-1] = 0;
        rem_flow[i][v-1] = 1;
    }
    for(int i = 0; i < n; ++i)
        for(int j = i+1; j < n; ++j)
            if(points[i].first > points[j].first){
                CostType d = hypot(points[i].first-points[j].first,points[i].second-points[j].second);
                cost[i+1][j+n+1] = d;
                rem_flow[i+1][j+n+1] = 1;
            }
    min_cost_max_flow();
    if(tot_flow != n-1){
        puts("-1");
        return 0;
    }
    //cout << tot_flow << '\n';
    cout << fixed << setprecision(10) << tot_cost <<'\n';
}
