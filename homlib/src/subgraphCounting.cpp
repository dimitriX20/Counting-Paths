#include "hom.hh" // achte auf doppeltes include, wenn spasm wieder auskommentiert 
#include <vector>
#include <iostream>
#include <future> // Für std::async und std::future

int64_t countSubgraphs(Graph g, size_t N) { // count sub(P_k,g) with k <= N + 1
	N = std::min(N, size_t(g.n));

	if (N == 1) 
		return g.m;

	std::vector<int64_t> factorials(21); 
	factorials[0] = 1LL; 

	for (int64_t i = 1; i < 21LL; i += 1LL) 
		factorials[i] = factorials[i - 1] * i; 

	auto getBlockFactors = [&](Graph& cur) -> int64_t {
		int64_t ans = 1; 
		for (int i = 0; i < cur.n; i += 1) 
            ans *= factorials[cur.partClasses[i].size() - 1]; 
		
		return ans; 
	}; 
 
	size_t k = 2;  
	g.spasms.push_back({{1LL, int64_t(2 * g.m)}, getPk(2)}); // hom(p_2, g) == 2 * nrEdgesOfG
	int64_t subgraphs = (N == 1 ? 0 : g.m); 
    
	while (k < N) { // efficiently transform k to k + 1 
		k += 1; 
         
		if (g.n < k or g.m < k - 1) // test if P_k+1 has more nodes or edges than g 
			break; 

        std::vector<std::pair<std::pair<int64_t, int64_t>, Graph>> nwSpasm; 
		for (auto& [p, h]: g.spasms) {
			Graph nw = h; 
            HomomorphismCounting<int64_t> oldHom(nw, g); 
			int64_t dg1HeuristicHom = oldHom.run();

            nw.addNextNodeAndEdge();
            auto neighbor = nw.nwName[k - 2]; 

			for (int contractNode = 0; contractNode < k; contractNode += 1) {
				bool done = false; 
				auto toContract = nw.nwName[contractNode];

				if (toContract == neighbor)  
					continue; 

				Graph res = contract(nw, nw.nwName[k - 1], toContract); // toContract ist bereits neuer Name 
				bool same = false; 
                std::pair<std::pair<int64_t, int64_t>, Graph> save = {{1, 2}, nw}; // placeholder values

				for (auto& [p2, h2]: nwSpasm) { // search for isomorphism and strong isomorphism
					if (h2 == res)							
						same = true; 
					
					if (not same) 
						continue; 
 
					if (strongIsomorph(h2, res)) { // now as we are isomorph, therefore test for strong isomorphism 
						done = true; 
						break; 
					}

					if (same) { // we assume that hom(res) doesn't change if graphs are isomorph; so we reuse the hom nr.
                        save.first = {getBlockFactors(res) * ((k - res.n) & 1 ? -1LL : 1LL), p2.second};
                        save.second = res;
						break; 
					}
				}
                
                if (done) 
					continue; 

                if (same) 
                    nwSpasm.push_back(save); 

				if (not same) { // found unique spasm graph. therefore, compute hom number or use precomputed value
                    //if (res.adj[res.dsu.get(k - 1)].size() == 1) 
                //		nwSpasm.push_back({{getBlockFactors(res) * ((k - res.n) & 1 ? -1LL : 1LL), dg1HeuristicHom * int64_t(g.n - 1)}, res}); 
                    //else {

                    HomomorphismCounting<int64_t> hom(res, g); 
                    nwSpasm.push_back({{getBlockFactors(res) * ((k - res.n) & 1 ? -1LL : 1LL), hom.run()}, res});
                }
//}
			}
		}

        std::swap(nwSpasm, g.spasms);  
        std::cerr << " Größe von g.spasms: " << g.spasms.size()  << "\n";

        int64_t sub = 0; 
        for (auto [p, gr]: g.spasms) 
            sub += p.first * p.second; 
        
        std::cerr << " subgraphen von P_" << k << " in G ist: " << sub / int64_t(2) << "\n";
	}

	return subgraphs / int64_t(2);
}


struct countPathsOnePair {
    Graph g;
    int s, t;
    std::vector<int> dist_s, dist_t;

    countPathsOnePair(const Graph& g, int s, int t) : g(g), s(s), t(t), dist_s(g.n, -1), dist_t(g.n, -1) {}

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
            if (not visited[v] or dist_s[v] + dist_t[v] > k) {
                remove[v] = true;
                sz -= 1;
            }
        }

        if (sz > 22) 
            return 0;   

        // müssen zunächst den Graphen neu erstellen und umbennen s und t sind zB. 0 und 1 
        // führe dies analog zu contract() durch!
        Graph nw(sz); 

        if (isTree(nw)) {
            // Tree dp 
            // oder Centroid Decomposition durchführen
        }

        // ans = summe über alle: dp[mask][1] mit pop_count(mask) <= k + 1
            // starte ab k = dist[s][t] + 1
        // initialisiere nur dp[1][1] = 1 (also Startecke 0 ist 1 in der mask)

        int64_t ans = 0; 
        for (int N = 3; N <= sz; N += 1) {
            std::vector<std::vector<int64_t>> dp(1 << N, std::vector<int64_t> (N, 0LL));
        }

        return ans; 
    }
}; 
