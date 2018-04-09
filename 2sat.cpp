#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;


//!escape \section{2-Sat $\mathcal{O}(n)$ and SCC $\mathcal{O}(n)$}
//!begin_codebook
//!start
struct Graph {
    int n;
    vector<vector<int> > conn;
    
    Graph(int nsize) {
        n = nsize;
        conn.resize(n);
    }
    
    void add_edge(int u, int v) {
        conn[u].push_back(v);
    }
    
    void _topsort_dfs(int pos, vector<int> &result, vector<bool> &explr, vector<vector<int> > &revconn) {
        if(explr[pos])
            return;
        explr[pos] = true;
        for(auto next : revconn[pos])
            _topsort_dfs(next, result, explr, revconn);
        result.push_back(pos);
    }
    
    vector<int> topsort() {
        vector<vector<int> > revconn(n);
        for(int u = 0; u < n; u++) {
            for(auto v : conn[u])
                revconn[v].push_back(u);
        }
        
        vector<int> result;
        vector<bool> explr(n, false);
        for(int i=0; i < n; i++)
            _topsort_dfs(i, result, explr, revconn);
        reverse(result.begin(), result.end());
        return result;
    }
    
    void dfs(int pos, vector<int> &result, vector<bool> &explr) {
        if(explr[pos])
            return;
        explr[pos] = true;
        for(auto next : conn[pos])
            dfs(next, result, explr);
        result.push_back(pos);
    }
//!finish
//!start
	vector<vector<int> > scc(){ // tested on https://www.hackerearth.com/practice/algorithms/graphs/strongly-connected-components/practice-problems/algorithm/a-walk-to-remember-qualifier2/
        vector<int> order = topsort();
		reverse(order.begin(),order.end());
        vector<bool> explr(n, false);
		vector<vector<int> > results;
		for(auto it = order.rbegin(); it != order.rend(); ++it){
			vector<int> component;
			_topsort_dfs(*it,component,explr,conn);
			sort(component.begin(),component.end());
			results.push_back(component);
		}
		sort(results.begin(),results.end());
		return results;
	}
};
//!finish

//Solution for: http://codeforces.com/group/PjzGiggT71/contest/221700/problem/C
int main() {
//!end_codebook
    ios_base::sync_with_stdio(false);
    cin.tie(0);
//!begin_codebook
    
    int n, m;
    cin >> n >> m;
    
    Graph g(2*m);
    
    for(int i=0; i<n; i++) {
        int a, sa, b, sb;
        cin >> a >> sa >> b >> sb;
        a--, b--;
        
        g.add_edge(2*a + 1 - sa, 2*b + sb);
        g.add_edge(2*b + 1 - sb, 2*a + sa);
    }
    
    vector<int> state(2*m, 0);
    {
        vector<int> order = g.topsort();
        vector<bool> explr(2*m, false);
        
        for(auto u : order) {
            vector<int> traversed;
            g.dfs(u, traversed, explr);
            
            if(traversed.size() > 0 && !state[traversed[0]^1]) {
                for(auto c : traversed)
                    state[c] = 1;
            }
        }
    }
    
    for(int i=0; i < m; i++) {
        if(state[2*i] == state[2*i+1]) {
            cout << "IMPOSSIBLE\n";
            return 0;
        }
    }
    
    for(int i=0; i < m; i++) {
        cout << state[2*i+1] << '\n';
    }
    return 0;
}
//!end_codebook


