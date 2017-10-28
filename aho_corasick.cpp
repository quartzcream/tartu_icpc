#include <bits/stdc++.h>
using namespace std;
typedef long long               ll;

//!escape \section{Aho Corasick O($|\text{alpha}|\sum{\text{len}}$)}

//!begin_codebook
const int alpha_size=26;
struct node{
	node *nxt[alpha_size]; //May use other structures to move in trie
	node *suffix;
	node(){
		memset(nxt, 0, alpha_size*sizeof(node *));
	}
	int cnt=0;
};
node *aho_corasick(vector<vector<char> > &dict){
	node *root= new node;
	root->suffix = 0;
	vector<pair<vector<char> *, node *> > cur_state;
	for(vector<char> &s : dict)
		cur_state.emplace_back(&s, root);
	for(int i=0; !cur_state.empty(); ++i){
		vector<pair<vector<char> *, node *> > nxt_state;
		for(auto &cur : cur_state){
			node *nxt=cur.second->nxt[(*cur.first)[i]];
			if(nxt){
				cur.second=nxt;
			}else{
				nxt = new node;
				cur.second->nxt[(*cur.first)[i]] = nxt;
				node *suf = cur.second->suffix;
				cur.second = nxt;
				nxt->suffix = root; //set correct suffix link
				while(suf){
					if(suf->nxt[(*cur.first)[i]]){
						nxt->suffix = suf->nxt[(*cur.first)[i]];
						break;
					}
					suf=suf->suffix;
				}
			}
			if(cur.first->size() > i+1)
				nxt_state.push_back(cur);
		}
		cur_state=nxt_state;
	}
	return root;
}
node *walk(node *cur, char c){
	while(true){
		if(cur->nxt[c])
			return cur->nxt[c];
		if(!cur->suffix){
			return cur;
		}
		cur = cur->suffix;
	}
}
void cnt_matches(node *root, vector<char> &match_in){
	node *cur = root;
	for(char c : match_in){
		cur = walk(cur, c);
		++cur->cnt;
	}
}
void add_cnt(node *root){
	vector<node *> to_visit = {root};
	for(int i=0; i<to_visit.size(); ++i){
		node *cur = to_visit[i];
		for(int j=0; j<alpha_size; ++j){
			if(cur->nxt[j]){
				to_visit.push_back(cur->nxt[j]);
			}
		}
	}
	for(int i=to_visit.size()-1; i>0; --i){
		to_visit[i]->suffix->cnt += to_visit[i]->cnt;
	}
}
//!end_codebook
//http://codeforces.com/group/s3etJR5zZK/contest/212916/problem/4
int main(){
	int n, len;
	scanf("%d %d", &len, &n);
	vector<char> a(len+1);
	scanf("%s", a.data());
	a.pop_back();
	for(char &c : a){
		c -= 'a';
	}
	vector<vector<char> > dict(n);
	for(int i=0; i<n; ++i){
		scanf("%d", &len);
		dict[i].resize(len+1);
		scanf("%s", dict[i].data());
		dict[i].pop_back();
		for(char &c : dict[i]){
			c -= 'a';
		}
	}
	node *root = aho_corasick(dict);
	cnt_matches(root, a);
	add_cnt(root);
	for(int i=0; i<n; ++i){
		node *cur = root;
		for(char c : dict[i]){
			cur = walk(cur, c);
		}
		printf("%d\n", cur->cnt);
	}
	return 0;
}
