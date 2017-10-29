//----------------------------------------------------------------
//OMLBEGIN: 2SAT Graph
#include<vector>
#include<functional>
#include<algorithm>
using namespace std;


struct Graph {
	int n;
	vector<vector<int> > conn;
	
	Graph(int nsize) {
		n = nsize;
		conn.resize(n);
	}
	
	void addEdge(int a, int b) {
		conn[a].push_back(b);
	}
	
	vector<int> topsort() {
		vector<vector<int> > revconn(n);
		
		for(int i=0;i<n;i++)
			for(int j : conn[i])
				revconn[j].push_back(i);
		
		vector<int> result;
		vector<bool> explr(n, false);
		
		function<void (int)> dfs = [&](int pos) {
			if(explr[pos])
				return;
			explr[pos] = true;
			
			for(auto next : revconn[pos])
				dfs(next);
			result.push_back(pos);
		};
		
		for(int i=0;i<n;i++)
			dfs(i);
		
		reverse(result.begin(), result.end() );
		return result;
	}
	
	//Assumes (2*i, 2*i+1) is the single variable pair
	vector<int> satSolve() {
		vector<int> topsorted = topsort();
		
		vector<int> result;
		vector<bool> curexplr(n, false), explr(n, false);
		
		function<void (int)> dfs = [&](int pos) {
			if(curexplr[pos^1])
				throw 16;
			if(explr[pos] || explr[pos^1])
				return;
				
			explr[pos] = true;
			curexplr[pos] = true;
			result.push_back(pos);
			
			for(auto next : conn[pos])
				dfs(next);
			
			curexplr[pos] = false;
		};
		
		for(auto c : topsorted)
			dfs(c);
		
		return result;
	}
};
//OMLEND: 2SAT Graph
//----------------------------------------------------------------