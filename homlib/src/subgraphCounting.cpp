#include "hom.hh"  
#include <vector>
#include <iostream>
#include <future>  

std::ostream&
operator<<( std::ostream& dest, __int128_t value )
{
    std::ostream::sentry s( dest );
    if ( s ) {
        __uint128_t tmp = value < 0 ? -value : value;
        char buffer[ 128 ];
        char* d = std::end( buffer );
        do
        {
            -- d;
            *d = "0123456789"[ tmp % 10 ];
            tmp /= 10;
        } while ( tmp != 0 );
        if ( value < 0 ) {
            -- d;
            *d = '-';
        }
        int len = std::end( buffer ) - d;
        if ( dest.rdbuf()->sputn( d, len ) != len ) {
            dest.setstate( std::ios_base::badbit );
        }
    }
    return dest;
}

__int128 countSubgraphs(Graph g, size_t N) { // count sub(P_k,g) with k <= N + 1
	N = std::min(N, size_t(g.n));

	if (N == 1) 
		return g.m;

	std::vector<__int128> factorials(21); 
	factorials[0] = 1LL; 

	for (int64_t i = 1; i < 21LL; i += 1LL) 
		factorials[i] = factorials[i - 1] * i; 

	auto getBlockFactors = [&](Graph& cur) -> __int128 {
		__int128 ans = 1; 
		for (int i = 0; i < cur.n; i += 1) 
            ans *= factorials[cur.partClasses[i].size() - 1]; 
		
		return ans; 
	}; 
 
	size_t k = 2;  
	g.spasms.push_back({{__int128(1), __int128(2 * g.m)}, getPk(2)}); // hom(p_2, g) == 2 * nrEdgesOfG
	__int128 subgraphs = (N == 1 ? 0 : g.m); 
    
	while (k < N) { 
		k += 1; 
         
		if (g.n < k or g.m < k - 1) 
			break; 

        std::vector<std::pair<std::pair<__int128, __int128>, Graph>> nwSpasm; 
		for (auto& [p, h]: g.spasms) {
			Graph nw = h; 

            nw.addNextNodeAndEdge();
            auto neighbor = nw.nwName[k - 2]; 

			for (int contractNode = 0; contractNode < k; contractNode += 1) {
				bool done = false; 
				auto toContract = nw.nwName[contractNode];

				if (toContract == neighbor)  
					continue; 

				Graph res = contract(nw, toContract, nw.nwName[k - 1]);  

				bool same = false;  
                Graph saveGraph(0);
                std::pair<__int128, __int128> savePair;  

				for (auto& [p2, h2]: nwSpasm) { // search for isomorphism and strong isomorphism
					if (h2 == res){
						same = true; 
                        saveGraph = res; 
                        savePair = {getBlockFactors(res) * (abs(int(k) - res.n) & 1 ? -1LL : 1LL), p2.second};
                    }						
					
					if (not same) 
						continue; 
 
					if (strongIsomorph(h2, res)) { // now as we are isomorph, therefore test for strong isomorphism 
						done = true; 
						break; 
					}
				}
                
                if (done) 
					continue; 

                if (same) 
                    nwSpasm.emplace_back(savePair, saveGraph);

				if (not same) {  
                    HomomorphismCounting<__int128> hom(res, g); 
                    nwSpasm.push_back({{getBlockFactors(res) * (abs(int(k) - res.n) & 1 ? __int128(-1) : __int128(1)), hom.run()}, res});
                }             
			}
		}
        std::swap(nwSpasm, g.spasms);  
        //std::cerr << " Größe von g.spasms: " << g.spasms.size()  << "\n";
 
        __int128 sub = 0; 
        for (auto [p, gr]: g.spasms) 
            sub += p.first * p.second; 
        
       // std::cout << " subgraphen von P_" << k << " in G ist: " << __int128(sub / __int128(2)) << "\n"; //need to overload operator to print __int128
	}

	return subgraphs / __int128(2);
}

struct countPathsTree {
    Graph g;  

    int n, a, b; 
    int subtree[200001];

    int64_t ans = 0, bit[200001];
    int mx_depth;
    bool processed[200001];
    bool terminal;  

    countPathsTree(Graph g, int n, int N, bool terminal): g(g), n(n), b(N - 1), terminal(terminal) {}

    int get_subtree_sizes(int node, int parent = 0) {
        subtree[node] = 1;
        for (int i : g.adj[node])
            if (!processed[i] && i != parent)
                subtree[node] += get_subtree_sizes(i, node);
        return subtree[node];
    }
 
    int get_centroid(int desired, int node, int parent = 0) {
        for (int i : g.adj[node])
            if (!processed[i] && i != parent && subtree[i] >= desired)
                return get_centroid(desired, i, node);
        return node;
    }
    
    void update(int pos, int64_t val) {
        for (pos++; pos <= n; pos += pos & -pos) bit[pos] += val;
    }
    
    int64_t query(int l, int r) {
        int64_t ans = 0;
        for (r++; r; r -= r & -r) ans += bit[r];
        for (; l; l -= l & -l) ans -= bit[l];
        return ans;
    }
    
    void get_cnt(int node, int parent, bool filling, int depth = 1) {
        if (depth > b) return;
        mx_depth = std::max(mx_depth, depth);
        if (filling) update(depth, 1);
        else ans += query(std::max(0, a - depth), b - depth);
        for (int i : g.adj[node])
            if (!processed[i] && i != parent) get_cnt(i, node, filling, depth + 1);
    }
    
    void centroid_decomp(int node = 1) {
        int centroid = get_centroid(get_subtree_sizes(node) >> 1, node);
        processed[centroid] = true;
        mx_depth = 0;
        for (int i : g.adj[centroid])
            if (!processed[i]) {
                get_cnt(i, centroid, false);
                get_cnt(i, centroid, true);
            }
        for (int i = 1; i <= mx_depth; i++) update(i, -query(i, i));
        for (int i : g.adj[centroid])
            if (!processed[i]) centroid_decomp(i);
    }

    void bfs(int start, std::vector<int>& dist) {
        std::queue<int> q;
        q.push(start);
        dist[start] = 0;
        while (not q.empty()) {
            int u = q.front(); q.pop();
            for (int v : g.adj[u]) {
                if (dist[v] == -1) {
                    dist[v] = dist[u] + 1;
                    q.push(v);
                }
            }
        }
    }

    int64_t countAll() { 
        update(0, 1);
	    centroid_decomp();
	    return ans;
    }

    int countTerminal(int start, int end) {
        std::vector<int> d(g.n, -1); 
        bfs(start, d); 
        return int(d[end] != -1 and d[end] <= b);
    }
};


struct countPathsOnePair {
    Graph g;
    int s, t;
    std::vector<int> dist_s, dist_t;

    countPathsOnePair(const Graph g, int s, int t) : g(g), s(s), t(t), dist_s(g.n, -1), dist_t(g.n, -1) {}

    void dfs(int u, std::vector<bool>& visited) {
        visited[u] = true;
        for (int v : g.adj[u]) {
            if (!visited[v]) {
                dfs(v, visited);
            }
        }
    }

    void bfs(int start, std::vector<int>& dist) {
        std::queue<int> q;
        q.push(start);
        dist[start] = 0;
        while (not q.empty()) {
            int u = q.front(); q.pop();
            for (int v : g.adj[u]) {
                if (dist[v] == -1) {
                    dist[v] = dist[u] + 1;
                    q.push(v);
                }
            }
        }
    }

    int64_t countSTPaths(int k) {
        std::vector<bool> visited(g.n, false);
        dfs(s, visited);

        if (not visited[t]) 
            return 0;

        bfs(s, dist_s);
        bfs(t, dist_t);

        std::vector<bool> remove(g.n);
        int sz = g.n; 

        for (int v = 0; v < g.n; ++v) {
            if (not visited[v] or dist_s[v] + dist_t[v] >= k) { // evtl. > k 
                remove[v] = true;
                sz -= 1;
            }
        }

        if (sz > 22) 
            return 0;   
 
        Graph res(sz);  
        std::map<int, int> nwName;
        nwName[s] = 0, nwName[t] = 1; 

        for (int i = 0, idx = 1; i < g.n; i += 1) {
            if (i == s or i == t or remove[i]) 
                continue; 

            idx += 1; 
            nwName[i] = idx;  
        }
    
        for (int i = 0; i < g.n; i += 1) {
            if (remove[i]) 
                continue; 

            for (int j: g.adj[i]) {
                if (not remove[j]) 
                    res.addEdge(nwName[i], nwName[j]); 
            }
        } 

        auto getRes = [](Graph gg, int n, int k, int start, int end) {
            int64_t ans = 0; 
            std::vector<std::vector<int64_t>> dp(1 << n, std::vector<int64_t>(n, 0LL));
            dp[1 << start][start] = 1;

            for (int s = (1 << start); s < (1 << n); s++) {
                if (__builtin_popcount(s) >= k) 
                    continue;

                for (int i = 0; i < n; i++) { 
                    if (dp[s][i]) {
                        for (int j: gg.adj[i]) {
                            if (~s >> j & 1) {
                                dp[s | 1 << j][j] += dp[s][i];
                                ans += (j == end) * dp[s][i]; 
                            }
                        }
                    }
                }
            }
            
            return ans; 
        }; 

        return getRes(res, res.n, k, 0, 1); 
    }
}; 
