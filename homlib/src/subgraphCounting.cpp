#include "hom.hh" // achte auf doppeltes include, wenn spasm wieder auskommentiert 
#include <vector>
#include <iostream>
#include <future> // Für std::async und std::future

int64_t countSubgraphs(Graph g, size_t N) { // count sub(P_k,g) with k <= N + 1
    std::cerr << " starting method countSubgraphs  \n";
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
	std::cerr << " now we get while (k < N): \n";
    
	while (k < N) { // efficiently transform k to k + 1 
		k += 1; 
         
		if (g.n < k or g.m < k - 1) // test if P_k+1 has more nodes or edges than g 
			break; 

        std::vector<std::pair<std::pair<int64_t, int64_t>, Graph>> nwSpasm; 

	    std::cerr << " now start with iterating over g.spasms \n";
		for (auto& [p, h]: g.spasms) {
			Graph nw = h; // copy old spasm graph
            std::cerr << " nw.dsu.get(" << k - 2 << ") : " << nw.dsu.get(k - 2) << "\n";
            std::cerr << " größe von nwName: " << nw.nwName.size() << "\n";
			
            auto partClassNeighbor = nw.nwName[k - 2]; // oder evtl. nwName[nw.dsu.get(k - 2)]
            std::cerr << " partClassNeighbor: " << partClassNeighbor << "\n";
            
            nw.addNextNodeAndEdge();
            std::cerr << " added next node and edge" << "\n"; 
            std::cerr << " so sieht nw nun aus nach neuer Kante und Ecke: \n";
            print(nw); 
 
 

			HomomorphismCounting<int64_t> oldHom(nw, g); 
			int64_t dg1HeuristicHom = oldHom.run(); // precompute to apply change if nwNode has dg == 1 
            std::cerr << " computed heuristicForHom \n";

			for (int contractNode = 0; contractNode < k - 1; contractNode += 1) {
				bool done = false; 
				auto toContract = nw.nwName[contractNode];//nw.nwName[nw.dsu.get(contractNode)]; // betrachte aktuellen Namen der repräsentativen Ecke der DSU klasse 

                std::cerr << " wenn hier 1 rauskommt: " << (toContract != partClassNeighbor) << " , dann kontrahiere: " << toContract << " und " << partClassNeighbor << "\n";
                // kontrahieren aktuell was falsches
                //if (toContract == partClassNeighbor or nw.s[toContract].find(partClassNeighbor) != nw.s[toContract].end())
				if (toContract != partClassNeighbor) // or nw.s[toContract].find(partClassNeighbor) != nw.s.end()) // gleiche Klasse wie Nachbar, daher keine Kontraktion erlaubt  
					continue; 

				Graph res = contract(nw, nw.nwName[k - 1], toContract); // toContract ist bereits neuer Name 
				bool same = false; 

                std::cerr << " contracted:" << toContract << " and " << nw.nwName[k - 1] << " now searching for isomorphisms \n";
                std::cerr << " so sieht res nun aus nach Kontraktion von nw: \n";
                print(res); 

				for (auto& [p2, h2]: nwSpasm) { // search for isomorphism and strong isomorphism
					if (h2 == res)							
						same = true; 
					
					if (not same) 
						continue; 

                    // hier aufpassen aufgrund von nwName und olName!!!
					if (strongIsomorph(h2, res)) { // now as we are isomorph, therefore test for strong isomorphism 
                        std::cerr << " we are strongly isomorphic! \n";
						done = true; 
						break; 
					}

					if (same) { // we assume that hom(res) doesn't change if graphs are isomorph; so we reuse the hom nr.
                        nwSpasm.push_back({{getBlockFactors(res) * ((k - res.n) & 1 ? -1LL : 1LL), p2.second}, res});
						break; 
					}
				}
                
                std::cerr << " passed isomorphism checking \n";
				
                if (done) 
					continue; 

				if (not same) { // found unique spasm graph. therefore, compute hom number or use precomputed value
                    //if (res.adj[res.dsu.get(k - 1)].size() == 1) 
                //		nwSpasm.push_back({{getBlockFactors(res) * ((k - res.n) & 1 ? -1LL : 1LL), dg1HeuristicHom * int64_t(g.n - 1)}, res}); 
                    //else {

                    std::cerr << " countingHom res, g \n";  
                    std::cerr << " printing res graph: "; 
                    print(res); 
                    HomomorphismCounting<int64_t> hom(res, g); 

                    std::cerr << " blockfactors(res): " << getBlockFactors(res) << "\n";

                    std::cerr << " running hom.run(): " << hom.run() << "\n";
                    nwSpasm.push_back({{getBlockFactors(res) * ((k - res.n) & 1 ? -1LL : 1LL), hom.run()}, res});
                }
//}
			}
		}

        //std::cerr << " passed an iteration round of getting from k to k + 1 \n";
        std::swap(nwSpasm, g.spasms); // swappe erst hier, da nun nwSpasm aktuell 

        std::cerr << " Größe von nwSpasm: " << g.spasms.size()  << "\n";
		// hier nun mit neuem g.spasms die Subgraphen zählen 
        int64_t sub = 0; 
        for (auto [p, gr]: g.spasms) {
            sub += p.first * p.second; 
        }

        std::cerr << " subgraphen von P_" << k << " in G ist: " << sub / int64_t(2) << "\n";
	}
	return subgraphs / int64_t(2);// da P_k stets 2 Automorphismen hat f.countAutomorphisms(); 
}

// template <class Value>
// struct SubgraphCounting {
//     Graph H;
//     Graph G;

//     SubgraphCounting(Graph _H, Graph _G) : H(_H), G(_G) {}

//     Value countSubgraphs() {
//         if (H.n > G.n or H.m > G.m)
//             return 0;

//         std::vector<Value> factorials(21); 
//         factorials[0] = 1LL; 

//         for (Value i = 1; i < 21LL; i += 1LL) 
//             factorials[i] = factorials[i - 1] * i; 

//         auto getBlockFactors = [&](Graph& cur) -> Value {
//             Value ans = 1; 
//             std::vector<bool> vis(cur.n); 
//             for (int i = 0; i < cur.n; i += 1) {
//                 int p = cur.dsu.get(i); 
//                 if (not vis[p]) {
//                     ans *= factorials[Value(cur.dsu.size(p) - 1)];
//                     vis[p] = true; 
//                 }
//             }
//             return ans; 
//         }; 

//         generateSpasm(H); 
//         auto spasm = H.spasms; 
//         Value subgraphs = 0; 

//         bool parallelisieren = H.spasms.size() >= 15; 
//        // long long summe = 0; 
//         if (not parallelisieren) {
//             for (auto& h: spasm) {
//                 HomomorphismCounting<Value> homCounter(h.second, G);
//                 Value coeff = h.first * 1LL * getBlockFactors(h.second); 
//                 coeff *= (abs(H.n - h.second.n) & 1 ? -1LL : 1LL); 
//                 subgraphs += coeff * homCounter.run(); 
//                 //summe += h.first;
//             }
//             //std::cerr << " summe für k = 10: " << summe << "\n";
//             Value automorphisms = 0; 
//             std::vector<Graph> connectedComponentsH = connectedComponents(H); 

//             for (auto& h: connectedComponentsH)
//                 automorphisms += h.countAutomorphisms();

//             return subgraphs / automorphisms; 
//         } 

//         std::vector<std::future<Value>> futures1;
//         for (auto& h: spasm) {
//             futures1.emplace_back(std::async(std::launch::async, [&] {
//                 HomomorphismCounting<Value> homCounter(h.second, G);
//                 Value coeff = h.first * 1LL * getBlockFactors(h.second); 
//                 coeff *= (abs(H.n - h.second.n) & 1 ? -1LL : 1LL);
//                 return coeff * homCounter.run();
//             }));
//         }

//         for (auto &future : futures1) 
//             subgraphs += future.get();

//         Value automorphisms = 0; 
//         std::vector<Graph> connectedComponentsH = connectedComponents(H); 

//         std::vector<std::future<Value>> futures2;
//         for (auto& h: connectedComponentsH) {
//             futures2.emplace_back(std::async(std::launch::async, [&] {
//                 return h.countAutomorphisms();
//             }));
//         }

//         for (auto &future : futures2) 
//             automorphisms += future.get();
        
//         return subgraphs / automorphisms; 
//     }
// };

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
