#pragma once

#include<set>  
#include <unordered_set>
#include <cmath>
#include <map>
#include <algorithm>
#include <numeric>   
#include "dsu.cpp"

extern "C" {
	#include "nauty.h"
	#include "naututil.h"
}
//#include <mutex>

//std::mutex graphMutex;

struct Graph {
	int n, m;
	DSU dsu;  
	std::vector<std::vector<int>> adj;
	std::vector<std::set<int>> s; 
	std::vector<int> oldName; 
	std::vector<std::pair<int, Graph>> spasms;

	Graph(int n) : n(n), adj(n), s(n), m(0), dsu(n), oldName(n) {
		std::iota(oldName.begin(), oldName.end(), 0); 
	}

	void addEdge(int u, int v) {
		adj[u].push_back(v);
		adj[v].push_back(u);
		s[u].insert(v); 
		s[v].insert(u); 
		m += 1;
		spasms.clear();
	}

	  std::vector<std::pair<int, int>> getNonNeighbors() {
		std::vector<std::pair<int, int>> ans; 
		for (int i = 0; i < n; i += 1) {
			for (int j = i + 1; j < n; j += 1) {
				if (s[i].find(j) == s[i].end()) 
					ans.emplace_back(i, j); 
			}
		}
		return ans; 
	}


	bool isIsomorphic(const Graph& h) {
		if (h.n != this->n || h.m != this->m)
			return false;

		auto convertToNautyGraph = [&](const Graph &g, graph *&g1, size_t &g1_sz, int m_value) {
			int n_value = g.n;
			g1_sz = 0; // Initialize the size of g1 to be 0
			DYNALLOC2(graph, g1, g1_sz, n_value, m_value, "malloc");
			g1_sz = n_value * m_value; // Update the size of g1 after the allocation
			EMPTYGRAPH(g1, m_value, n_value);
			
			for (int i = 0; i < n_value; ++i) {
				for (int j : g.adj[i]) {
					ADDONEEDGE(g1, i, j, m_value);
				}
			}
		}; 

		DYNALLSTAT(int, lab1, lab1_sz);
		DYNALLSTAT(int, lab2, lab2_sz);
		DYNALLSTAT(int, ptn, ptn_sz);
		DYNALLSTAT(int, orbits, orbits_sz);
		DYNALLSTAT(int, map, map_sz);
		DYNALLSTAT(graph, cg1, cg1_sz);
		DYNALLSTAT(graph, cg2, cg2_sz);
		static DEFAULTOPTIONS_GRAPH(options);
		statsblk stats;

		int m_value, n_value;
		size_t k, g1_sz = 0, g2_sz = 0; // Declare g1_sz and g2_sz

		options.getcanon = TRUE;

		n_value = this->n;
		m_value = SETWORDSNEEDED(n_value);
		nauty_check(WORDSIZE, m_value, n_value, NAUTYVERSIONID);

		DYNALLOC1(int, lab1, lab1_sz, n_value, "malloc");
		DYNALLOC1(int, lab2, lab2_sz, n_value, "malloc");
		DYNALLOC1(int, ptn, ptn_sz, n_value, "malloc");
		DYNALLOC1(int, orbits, orbits_sz, n_value, "malloc");
		DYNALLOC1(int, map, map_sz, n_value, "malloc");

		graph *g1 = NULL, *g2 = NULL;
		convertToNautyGraph(*this, g1, g1_sz, m_value);  // Converting this graph to nauty graph
		convertToNautyGraph(h, g2, g2_sz, m_value);  // Converting graph h to nauty graph

		DYNALLOC2(graph, cg1, cg1_sz, n_value, m_value, "malloc");
		DYNALLOC2(graph, cg2, cg2_sz, n_value, m_value, "malloc");

		densenauty(g1, lab1, ptn, orbits, &options, &stats, m_value, n_value, cg1);
		densenauty(g2, lab2, ptn, orbits, &options, &stats, m_value, n_value, cg2);

		for (k = 0; k < m_value*(size_t)n_value; ++k)
			if (cg1[k] != cg2[k]) break;

		return k == m_value*(size_t)n_value;
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
			return isIsomorphic(other);
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
	std::vector<int> updOldName = res.oldName; 
	updOldName[idx] = res.oldName[v]; 

	for (int i = 0; i < h.n; i += 1) {
		if (i == v or i == u) 
			continue; 

		idx += 1; 
		nwName[i] = idx; 
		updOldName[idx] = res.oldName[i]; 
	}

	res.oldName = updOldName; 

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


std::map<std::pair<int, int>, std::vector<std::pair<int, Graph>>> mp; // global map to store all graphs and multiplicators 
void generateSpasm(Graph &g) {  
		if (g.spasms.size() == 0) 
			g.spasms.emplace_back(1, g);

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
				for (auto &pairInMap : mp[key]) {
						if (newGraph == pairInMap.second) {
								isomorphicExists = true;  
								pairInMap.first += 1;
								break; 
						}
				}

				// If no isomorphic graph is in the map, add the new graph.
				if (!isomorphicExists) { 
						mp[key].push_back({1, newGraph});
						generateSpasm(newGraph);
				}
		}

		// Concatenate all vectors in mp to get the final result.
		for (auto &pair: mp) {
				for (auto &graphPair: pair.second) {
						g.spasms.emplace_back(graphPair);
				}
		}
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