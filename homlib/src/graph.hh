#pragma once

#include<set>  
#include <unordered_set>
#include <cmath>
#include <map>
#include <algorithm>
#include <numeric>   
#include "dsu.cpp"  
//#include <mutex>

//std::mutex graphMutex;

struct Graph {
	int n, m;
	DSU dsu;  
	std::vector<std::vector<int>> adj;
	std::vector<std::set<int>> s; 
	std::vector<int> oldName; 
	std::map<std::pair<int, int>, std::vector<std::pair<int, Graph>>> mp;

	Graph(int n) : n(n), adj(n), s(n), m(0), dsu(n), oldName(n) {
		std::iota(oldName.begin(), oldName.end(), 0); 
	}

	void addEdge(int u, int v) {
		adj[u].push_back(v);
		adj[v].push_back(u);
		s[u].insert(v); 
		s[v].insert(u); 
		m += 1;
	}

	std::vector<std::pair<int, int>> getNonNeighbors() {
		auto notNeighbors = [&](int u, int v) -> bool {
			return u != v and s[u].find(v) == s[u].end(); 
		}; 

		std::vector<std::pair<int, int>> ans; 
		std::vector<bool> vis(n); 
		std::map<pair<int, int>, bool> done; 

		for (int i = 0; i < n; i += 1) {
			int p1 = dsu.get(i); 
			if (not vis[p1]) {
				vis[p1] = true; 
				for (int j = i + 1; j < n; j += 1) {
					int p2 = dsu.get(j); 
					if (not done[{p1, p2}] and notNeighbors(p1, p2)) {
						done[{p1, p2}] = done[{p2, p1}] = true; 
						ans.emplace_back(p1, p2); 
					}
				}
			}
		}
		return ans; 
	}

		// Check whether two graphs are isomorphic.
	bool isomorphic(const Graph &h) { 
			if (n != h.n)
					return false;

			// Create vectors of degrees
			std::vector<int> deg1(n);
			std::vector<int> deg2(h.n);

			for (int i = 0; i < n; i++)
					deg1[i] = adj[i].size();

			for (int i = 0; i < h.n; i++)
					deg2[i] = h.adj[i].size();

			std::sort(deg1.begin(), deg1.end());
			std::sort(deg2.begin(), deg2.end());
			if (deg1 != deg2)
					return false;
	
			std::vector<int> p(h.n);
			std::iota(p.begin(), p.end(), 0);

			do {
					bool isIsomorphic = true;
					for (int i = 0; i < n; i++) {
							for (int j : s[i]) {
									if (h.s[p[i]].find(p[j]) == h.s[p[i]].end()) {
											isIsomorphic = false;
											break;
									}
							}
							if (!isIsomorphic)
									break;
					}
					if (isIsomorphic)
							return true;
			} while (std::next_permutation(p.begin(), p.end()));

			return false;
	}

	// Copy constructor
	Graph(const Graph& other) {
			// Perform a deep copy of the graph
			n = other.n;
			m = other.m;
			adj = other.adj;
			s = other.s;
			dsu = other.dsu;
			oldName = other.oldName; 
	}

	// Assignment operator
	Graph& operator=(const Graph& other) {
			if (this != &other) {
					// Perform a deep copy of the graph
					n = other.n;
					m = other.m;
					adj = other.adj;
					s = other.s;
					dsu = DSU(other.n);
					oldName = other.oldName;
			}
			return *this;
	}

	// equal operator
	bool operator==(const Graph other) {
			return isomorphic(other);
	}

	//not equal operator
	bool operator!=(const Graph other) {
			return not isomorphic(other);
	}
};


Graph getPk(int k) {
	if (k < 1) 
		return Graph(0); 

	Graph g(k); 
	for (int i = 0; i < k - 1; i += 1)
		g.addEdge(i, i + 1); 

	return g; 
}

void print(const Graph& g) {
	for (int i = 0; i < g.n; ++i) {
		std::cout << "Vertex " << i << ": ";
		for (int v : g.adj[i]) {
			std::cout << v << " ";
		}
		std::cout << "\n";
	}
}

Graph contract(const Graph& h, int v, int u){
	if (v >= h.n or u >= h.n) 
		return Graph(0); 

	Graph res(h.n - 1);
	std::set<int> allNeighbors; 
	res.dsu = h.dsu; 

	res.oldName = h.oldName; 
	int p1 = res.dsu.get(res.oldName[v]); 
	int p2 = res.dsu.get(res.oldName[u]); 
//assert(not p1 == p2); 
	res.dsu.unite(p1, p2);
	
	int idx = 0; 
	std::map<int, int> nwName; 
	nwName[v] = nwName[u] = idx; 
	res.oldName[idx] = res.oldName[v]; 

	for (int i = 0; i < h.n; i += 1) {
		if (i == v or i == u) 
			continue; 

		idx += 1; 
		nwName[i] = idx; 
		res.oldName[idx] = res.oldName[i]; 
	}

	for (int i: h.adj[v]) {
		if (i != v and i != u)
			allNeighbors.insert(nwName[i]); 
	}

	for (int i: h.adj[u]) {
		if (i != v and i != u)
			allNeighbors.insert(nwName[i]); 
	}

	for (int i: allNeighbors) {
		if (i != 0)
			res.addEdge(0, i); 
	}

	for (int i = 0; i < h.n; i += 1) {
		if (i == v or i == u) 
			continue; 

		for (int j: h.adj[i]) {
			if (j != v and j != u and (nwName[i] != nwName[j]) and res.s[nwName[i]].find(nwName[j]) == res.s[nwName[i]].end())  
				res.addEdge(nwName[i], nwName[j]); 
		}
	}

	return res; 
}

std::vector<std::pair<int, Graph>> generateSpasm(Graph &g) {  
		// we assume that g is the initial P_k graph
		// Get all pairs of non-neighboring vertices.
		std::vector<std::pair<int, int>> pairs = g.getNonNeighbors();

		for (auto &pair : pairs) {
				// Contract the graph at the given vertices.
				Graph newGraph = contract(g, pair.first, pair.second);

				// Create a key for the map based on the number of nodes and edges.
				std::pair<int, int> key = {newGraph.n, newGraph.m};

				// Check if a graph isomorphic to the new graph is already in mp[key].
				bool isomorphicExists = false;
				for (auto &pairInMap : g.mp[key]) {
						if (newGraph == pairInMap.second) {
								isomorphicExists = true;  
								pairInMap.first += 1;
								break; 
						}
				}

				// If no isomorphic graph is in the map, add the new graph.
				if (!isomorphicExists) {
						g.mp[key].push_back({1, newGraph});
						generateSpasm(newGraph);
				}
		}

		// Concatenate all vectors in mp to get the final result.
		std::vector<std::pair<int, Graph>> spasms = {{1, g}};
		for (auto &pair : g.mp) {
				for (auto &graphPair : pair.second) {
						spasms.emplace_back(graphPair);
				}
		}
 
		return spasms;
}

bool isTree(Graph G) {
	std::vector<int> parent(G.n, -1);
	std::vector<int> stack = {0};
	for (int i = 0; i < stack.size(); ++i) {
		int u = stack[i];
		for (int v: G.adj[u]) {
			if (v == parent[u]) continue;
			if (parent[v] >= 0) return false;
			parent[v] = u;
			stack.push_back(v);
		}
	}
	return stack.size() == G.n;
}

std::vector<Graph> connectedComponents(Graph G) {
	std::vector<int> index(G.n, -1);
	std::vector<Graph> components;
	for (int s = 0; s < G.n; ++s) {
		if (index[s] >= 0) continue;
		index[s] = 0;
		std::vector<int> stack = {s};
		for (int i = 0; i < stack.size(); ++i) {
			int u = stack[i];
			for (int v: G.adj[u]) {
				if (index[v] >= 0) continue;
				index[v] = stack.size();
				stack.emplace_back(v);
			}
		}
		Graph H(stack.size());
		for (int u: stack) {
			for (int v: G.adj[u]) {
				if (u >= v) continue;
				H.addEdge(index[u], index[v]);
			}
		}
		components.emplace_back(H);
	}
	return components;
}

Graph createPetersonGraph() {
    Graph peterson(10);
    peterson.addEdge(0, 1);
    peterson.addEdge(1, 2);
    peterson.addEdge(2, 3);
    peterson.addEdge(3, 4);
    peterson.addEdge(4, 0);
    peterson.addEdge(0, 5);
    peterson.addEdge(1, 6);
    peterson.addEdge(2, 7);
    peterson.addEdge(3, 8);
    peterson.addEdge(4, 9);
    peterson.addEdge(5, 7);
    peterson.addEdge(7, 9);
    peterson.addEdge(9, 6);
    peterson.addEdge(6, 8);
    peterson.addEdge(8, 5);
    return peterson;
}

// int64_t countSubgraphs(Graph H, Graph G) {
// 	std::vector<int64_t> factorials(21); 
// 	factorials[0] = 1LL; 

// 	for (int64_t i = 1; i < 21LL; i += 1LL) 
// 		factorials[i] = factorials[i - 1] * i; 
	
// 	auto getBlockFactors = [&](Graph& cur) -> int64_t {
// 		int64_t ans = 1; 
// 		std::vector<bool> vis(cur.n); 
// 		for (int i = 0; i < cur.n; i += 1) {
// 			int p = cur.dsu.get(i); 
// 			if (not vis[p]) {
// 				ans *= int64_t(cur.dsu.size(p) - 1); 
// 				vis[p] = true; 
// 			}
// 		}
// 		return ans; 
// 	}; 

// 	std::vector<std::pair<int, Graph>> spasm = generateSpasm(H); 
// 	int64_t subgraphs = 0; 

// 	for (auto& h: spasm) {
// 		HomomorphismCounting<int64_t> homCounter(h.second, G);
// 		int64_t coeff = h.first * 1LL * getBlockFactors(h.second); //(Block-Größen - 1)! von jeder Partition
// 		coeff *= (abs(H.n - h.second.n) & 1 ? -1LL : 1LL); // hier evtl. h.second.n modifizeren nachdem genSpasm final implementiert
// 		subgraphs += h.first * homCounter.run(); 
// 		/* 
// 			außerdem sollten wir die Anzahl der Vorkommnisse eines jeden Spasm Graphen
// 			beim generieren des Spasms speichern als Multiplikator 
// 		*/  
// 	}

// 	int64_t automorphisms = 0; 
// 	std::vector<Graph> connectedComponentsH = connectedComponents(H); 

// 	//for (auto& h: connectedComponentsH)
//  //   automorphisms += countAutomorphisms(h); 

// 	return subgraphs;// / automorphisms; 
// }