#include <bits/stdc++.h>
using namespace std;
//!escape \section{general flow}
//!begin_codebook
struct MaxFlow{
    typedef long long ll;
    typedef double COST_TYPE;
    const ll INF = 1e18;
    struct Edge{
        int u,v;
        ll c,rc;
        COST_TYPE cost;
        shared_ptr<ll> flow;
        Edge(int _u, int _v, ll _c, COST_TYPE _cost = 0):u(_u),v(_v),c(_c),cost(_cost){
            rc = 0;
        }
    };
    struct FlowTracker{
        shared_ptr<ll> flow;
        ll cap, rcap;
        bool dir;
        FlowTracker(ll _cap, ll _rcap, shared_ptr<ll> _flow, int _dir):cap(_cap),rcap(_rcap),flow(_flow),dir(_dir){ }
        ll rem() const {
            if(dir == 0){
                return cap-*flow;
            }
            else{
                return rcap+*flow;
            }
        }
        void add_flow(ll f){
            if(dir == 0)
                *flow += f;
            else
                *flow -= f;
            assert(*flow <= cap);
            assert(-*flow <= rcap);
        }
        operator ll() const { return rem(); }
        void operator-=(ll x){ add_flow(x); }
        void operator+=(ll x){ add_flow(-x); }
    };
    int source,sink;
    vector<vector<int> > adj;
    vector<vector<COST_TYPE> > cost;
    vector<vector<FlowTracker> > cap;
    vector<Edge> edges;
    MaxFlow(int _source, int _sink):source(_source),sink(_sink){
        assert(source != sink);
    }
    int add_edge(int u, int v, ll c, COST_TYPE cost = 0){
        edges.push_back(Edge(u,v,c,cost));
        return edges.size()-1;
    }
    vector<int> now,lvl;
    vector<COST_TYPE> pot;
    void prep(){
        vector<int> indices = {source,sink};
        for(auto &edge : edges){
            indices.push_back(edge.u);
            indices.push_back(edge.v);
        }
        sort(indices.begin(),indices.end());
        indices.erase(unique(indices.begin(),indices.end()),indices.end());
        source = lower_bound(indices.begin(),indices.end(),source)-indices.begin();
        sink = lower_bound(indices.begin(),indices.end(),sink)-indices.begin();
        int max_id = indices.size()-1;
        adj.resize(max_id+1);
        cost.resize(max_id+1);
        cap.resize(max_id+1);
        now.resize(max_id+1);
        lvl.resize(max_id+1);
        pot.resize(max_id+1);
        for(auto &edge : edges){
            edge.u = lower_bound(indices.begin(),indices.end(),edge.u)-indices.begin();
            edge.v = lower_bound(indices.begin(),indices.end(),edge.v)-indices.begin();
            auto flow = make_shared<ll>(0);
            adj[edge.u].push_back(edge.v);
            cost[edge.u].push_back(edge.cost);
            cap[edge.u].push_back(FlowTracker(edge.c,edge.rc,flow,0));
            if(edge.u != edge.v){
                adj[edge.v].push_back(edge.u);
                cost[edge.v].push_back(-edge.cost);
                cap[edge.v].push_back(FlowTracker(edge.c,edge.rc,flow,1));
            }
            assert(cap[edge.u].back() == edge.c);
            edge.flow = flow;
        }
    }
    bool dinic_bfs(){
        fill(now.begin(),now.end(),0);
        fill(lvl.begin(),lvl.end(),0);
        lvl[source] = 1;
        vector<int> bfs(1,source);
        for(int i = 0; i < bfs.size(); ++i){
            int u = bfs[i];
            for(int j = 0; j < adj[u].size(); ++j){
                int v = adj[u][j];
                if(cap[u][j] > 0 && lvl[v] == 0){
                    lvl[v] = lvl[u]+1;
                    bfs.push_back(v);
                }
            }
        }
        return lvl[sink] > 0;
    }
    ll dinic_dfs(int u, ll flow){
        if(u == sink)
            return flow;
        while(now[u] < adj[u].size()){
            int v = adj[u][now[u]];
            if(lvl[v] == lvl[u] + 1 && cap[u][now[u]] != 0){
                ll res = dinic_dfs(v,min(flow,(ll)cap[u][now[u]]));
                if(res > 0){
                    cap[u][now[u]] -= res;
                    return res;
                }
            }
            ++now[u];
        }
        return 0;
    }
    ll max_flow(){
        prep();
        ll ans = 0;
        while(dinic_bfs()){
            ll cur = 0;
            do{
                cur = dinic_dfs(source,INF);
                ans += cur;
            }while(cur > 0);
        }
        return ans;
    }
    void bellman_ford(){
        vector<COST_TYPE> dist(adj.size(),INF);
        dist[source] = 0;
        for(int times = 0; times < dist.size(); ++times)
            for(auto &edge : edges)
                dist[edge.v] = min(dist[edge.v], dist[edge.u]+edge.cost);
        pot = dist;
    }
    ll dijkstra(vector<COST_TYPE> &dist, vector<int> &last_node, vector<int> &last_index){
        dist[source] = 0;
        vector<ll> flow(dist.size());
        flow[source] = INF;
        vector<bool> visited(dist.size());
        priority_queue<pair<COST_TYPE,int> > pq;
        pq.push({0,source});
        while(!pq.empty()){
            int u = pq.top().second;
            pq.pop();
            if(visited[u])
                continue;
            visited[u] = true;
            for(int i = 0; i < adj[u].size(); ++i){
                int v = adj[u][i];
                if(!visited[v] && cap[u][i])
                    if(dist[u]+cost[u][i]+pot[u]-pot[v] < dist[v]){
                        dist[v] = dist[u]+cost[u][i]+pot[u]-pot[v];
                        last_node[v] = u;
                        last_index[v] = i;
                        flow[v] = min(flow[u],(ll)cap[u][i]);
                        pq.push({-dist[v],v});
                    }
            }
        }
        return flow[sink];
    }
    pair<ll,COST_TYPE> min_cost_flow(){
        prep();
        bool has_negative = false;
        for(auto &edge : edges)
            if(edge.cost < 0)
                has_negative = true;
        if(has_negative)
            bellman_ford();
        ll total_flow = 0; 
        COST_TYPE total_cost = 0;
        while(true){
            vector<COST_TYPE> dist(adj.size(),INF);
            vector<int> last_node(dist.size()),last_index(dist.size());
            ll flow = dijkstra(dist,last_node,last_index);
            if(flow == 0)
                break;
            for(int u = sink; u != source; u = last_node[u])
                cap[last_node[u]][last_index[u]] -= flow;
            for(int i = 0; i < adj.size(); ++i)
                pot[i] += dist[i];
            total_flow += flow;
            total_cost += pot[sink]*flow;
        }
        return {total_flow,total_cost};
    }
    ll flow_on_edge(int edge_index){
        assert(edge_index < edges.size());
        return *edges[edge_index].flow;
    }
};
int main(){
}
