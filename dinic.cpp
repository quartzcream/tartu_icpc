#include <bits/stdc++.h>
using namespace std;
//!escape \section{Dinic}
//!begin_codebook
struct MaxFlow{
    typedef long long ll;
    const ll INF = 1e18;
    struct Edge{
        int u,v;
        ll c,rc;
        shared_ptr<ll> flow;
        pair<int,int> id() const {
            return make_pair(min(u,v),max(u,v));
        }
        Edge(int _u, int _v, ll _c, ll _rc = 0):u(_u),v(_v),c(_c),rc(_rc){
        }
        void join(const Edge &t){
            if(u == t.u){
                c += t.c;
                rc += t.rc;
            }
            else{
                c += t.rc;
                rc += t.c;
            }
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
    vector<vector<FlowTracker> > cap;
    vector<Edge> edges;
    MaxFlow(int _source, int _sink):source(_source),sink(_sink){
        assert(source != sink);
    }
    int add_edge(Edge e){
        edges.push_back(e);
        return edges.size()-1;

    }
    int add_edge(int u, int v, ll c, ll rc = 0){
        return add_edge(Edge(u,v,c,rc));
    }
    void group_edges(){
        map<pair<int,int>,vector<Edge> > edge_groups;
        for(auto edge: edges)
            if(edge.u != edge.v)
                edge_groups[edge.id()].push_back(edge);
        vector<Edge> grouped_edges;
        for(auto group: edge_groups){
            Edge main_edge = group.second[0];
            for(int i = 1; i < group.second.size(); ++i)
                main_edge.join(group.second[i]);
            grouped_edges.push_back(main_edge);
        }
        edges = grouped_edges;
    }
    vector<int> now,lvl;
    void prep(){
        int max_id = max(source,sink);
        for(auto edge : edges)
            max_id = max(max_id,max(edge.u,edge.v));
        adj.resize(max_id+1);
        cap.resize(max_id+1);
        now.resize(max_id+1);
        lvl.resize(max_id+1);
        for(auto &edge : edges){
            auto flow = make_shared<ll>(0);
            adj[edge.u].push_back(edge.v);
            cap[edge.u].push_back(FlowTracker(edge.c,edge.rc,flow,0));
            adj[edge.v].push_back(edge.u);
            cap[edge.v].push_back(FlowTracker(edge.c,edge.rc,flow,1));
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
    ll calc(){
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
};
int main(){
    int n,m;
    cin >> n >> m;
    auto mf = MaxFlow(1,n);
    int edge_index;
    for(int i = 0; i < m; ++i){
        int a,b,c;
        cin >> a >> b >> c;
        edge_index = mf.add_edge(a,b,c); //store edge index if care about flow value
        mf.add_edge(b,a,c);
    }
    mf.group_edges(); // call if we have multiple directed edges or self loops
    cout << mf.calc() << '\n';
    //cout << *mf.edges[edge_index].flow << '\n'; // can call only if group_edges() was not called
}
//!end_codebook
    // solves http://www.spoj.com/problems/FASTFLOW/
