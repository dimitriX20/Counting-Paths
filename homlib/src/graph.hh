#pragma once

#include <iostream>
#include <set>  
#include <unordered_set>
#include <cmath>
#include <map>
#include <algorithm>
#include <numeric>  
#include <omp.h>
#include <queue>
#include <unordered_map> 
#include "hom.hh"

extern "C" {
	#include "nauty.h"
	#include "naututil.h"
} 

struct Graph {
	int n, m;
	std::vector<std::vector<int>> adj;
	std::vector<std::set<int>> s; 
	std::vector<int> oldName; 
	std::vector<int> nwName; 
	std::unordered_map <int, std::vector<int>> partClasses; // store for i \in {0, ... n - 1} nodes in their partition 
	std::vector<std::pair<std::pair<__int128, __int128>, Graph>> spasms; // format: {{coeff, homNumber}, GraphInSpasmOf(G)}

	Graph(int n) : n(n), adj(n), s(n), m(0), oldName(n), nwName(n) {
		std::iota(oldName.begin(), oldName.end(), 0); 
		std::iota(nwName.begin(), nwName.end(), 0); 

		for (int i = 0; i < n; i += 1) 
			partClasses[i].push_back(i); 
	}

	void addEdge(int u, int v) { 
		if (u < 0 or v < 0 or u >= n or v >= n or v == u) 
			return; 
		
		if (s[u].find(v) != s[u].end())  
			return; 

		adj[u].push_back(v);
		adj[v].push_back(u);
		s[u].insert(v); 
		s[v].insert(u); 
		m += 1; 
	}

	void addNextNodeAndEdge() {
		n += 1; 
		m += 1; 
		
		int N = nwName.size() + 1;
		oldName.resize(n); 
		nwName.resize(N); 
		
		oldName[n - 1] = N - 1;
		nwName[N - 1] = n - 1;  
		
		s.resize(n); 
		s[n - 1].insert(nwName[N - 2]); 
		s[nwName[N - 2]].insert(n - 1); 

		adj.resize(n); 
		adj[n - 1].push_back(nwName[N - 2]);
		adj[nwName[N - 2]].push_back(n - 1);

		partClasses[n - 1].push_back(N - 1); 
	}

	bool isIsomorphic(const Graph& h) {
		if (h.n != this->n || h.m != this->m)
			return false;

		auto convertToNautyGraph = [&](const Graph &g, graph *&g1, size_t &g1_sz, int m_value) {
			int n_value = g.n;
			g1_sz = 0; 
			DYNALLOC2(graph, g1, g1_sz, n_value, m_value, "malloc");
			g1_sz = n_value * m_value; // Update the size of g1 after the allocation
			EMPTYGRAPH(g1, m_value, n_value);
			
			for (int i = 0; i < n_value; ++i) {
				for (int j: g.adj[i]) {
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
		size_t k, g1_sz = 0, g2_sz = 0;  

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
		convertToNautyGraph(*this, g1, g1_sz, m_value);   
		convertToNautyGraph(h, g2, g2_sz, m_value);   

		DYNALLOC2(graph, cg1, cg1_sz, n_value, m_value, "malloc");
		DYNALLOC2(graph, cg2, cg2_sz, n_value, m_value, "malloc");

		densenauty(g1, lab1, ptn, orbits, &options, &stats, m_value, n_value, cg1);
		densenauty(g2, lab2, ptn, orbits, &options, &stats, m_value, n_value, cg2);

		for (k = 0; k < m_value*(size_t)n_value; ++k)
			if (cg1[k] != cg2[k]) break;

		bool result = (k == m_value*(size_t)n_value);

		// Deallocate memory
		DYNFREE(g1, g1_sz);
		DYNFREE(g2, g2_sz);
		DYNFREE(cg1, cg1_sz);
		DYNFREE(cg2, cg2_sz);
		DYNFREE(lab1, lab1_sz);
		DYNFREE(lab2, lab2_sz);
		DYNFREE(ptn, ptn_sz);
		DYNFREE(orbits, orbits_sz);
		DYNFREE(map, map_sz);

		return result;
	}


	// Copy constructor
	Graph(const Graph& other) {
			// Perform a deep copy of the graph
			n = other.n;
			m = other.m;
			adj = other.adj;
			s = other.s;
			oldName = other.oldName; 
			nwName = other.nwName; 
			partClasses = other.partClasses; 
	}

	// Assignment operator
	Graph& operator=(const Graph& other) {
			if (this != &other) {
					// Perform a deep copy of the graph
					n = other.n;
					m = other.m;
					adj = other.adj;
					s = other.s;
					oldName = other.oldName;
					nwName = other.nwName; 
					partClasses = other.partClasses; 
			}
			return *this;
	}

	bool operator==(const Graph other) {
			return isIsomorphic(other);
	}
 
	bool operator!=(const Graph other) {
			return not isIsomorphic(other);
	}
};

struct hashV {
	size_t operator()(const std::vector<int> &x) const {
		constexpr size_t p = 1e9+7;
		size_t hash = 0;
		for(int i = 0; i < x.size(); ++i) {
		hash += p * hash + x[i];
		}
		return hash;
	}
};

using partitionTable = std::unordered_map<std::vector<int>, int, hashV>;

bool strongIsomorph(Graph& a, Graph& b) { // here we assume that graphs are already isomorphic, that is a == b 	
	partitionTable t; 
	for (auto& [i, v]: a.partClasses) {
		std::sort(v.begin(), v.end()); 
		t[v] += 1;
	}

	for (auto& [i, v]: b.partClasses) {
		std::sort(v.begin(), v.end()); 
		t[v] -= 1; 
	}

	bool isOk = true; 
	for (auto& [v, i]: t) 
		isOk &= i == 0; 

	return isOk;
}

Graph getPk(int k) {
	if (k < 1) 
		return Graph(0); 

	Graph g(k); 
	for (int i = 0; i < k - 1; i += 1)
		g.addEdge(i, i + 1); 

	return g; 
}

void print(const Graph& g) {
	std::cerr << "n: " << g.n << " m: " << g.m << "\n";
	for (int i = 0; i < g.n; ++i) {
		std::cout << "Vertex " << i << ": ";
		for (int v : g.adj[i]) {
			std::cerr << v << " ";
		}
		std::cerr << "\n";
	}
}

Graph contract(Graph h, int v, int u) {  
	if (v >= h.n or u >= h.n or u < 0 or v < 0) // we assume that v and u are the new names 
		return Graph(0); 

	//bool nichtBenachbart = h.s[u].find(u) == h.s[u].end(); 
	//#include <cassert>
	//assert(nichtBenachbart); 

	if (v == u) 
		return h; 

	Graph res(h.n - 1);
    res.oldName = h.oldName; 
	std::vector<int> updOldName = h.oldName; 
	updOldName[0] = h.oldName[v]; 

	std::set<int> allNeighbors; 
	std::map<int, int> nwName;
	nwName[v] = nwName[u] = 0; 

	if (h.partClasses[v].size() < h.partClasses[u].size()) // we assume that keys v and u exist!
		std::swap(h.partClasses[v], h.partClasses[u]);

	res.partClasses[0] = h.partClasses[v];
	res.partClasses[0].insert(res.partClasses[0].end(), h.partClasses[u].begin(), h.partClasses[u].end());

	for (int i = 0, idx = 0; i < h.nwName.size(); i += 1) {
		if (i == v or i == u) 
			continue; 

		idx += 1; 
		nwName[i] = idx; 
        if (i < h.n) {
		    updOldName[idx] = res.oldName[i]; 
			res.partClasses[idx] = h.partClasses[i]; 
		}
	}

	std::vector<int> upNwName = h.nwName; 
    for (int i = 0; i < h.nwName.size(); i += 1)
        upNwName[i] = nwName[h.nwName[i]];

	res.oldName = updOldName; 
	res.nwName = upNwName; 

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

bool isTree(Graph G) {
	std::vector<int> parent(G.n, -1);
	std::vector<int> stack = {0};
	
	for (int i = 0; i < stack.size(); ++i) {
		int u = stack[i];
		for (int v: G.adj[u]) {
			if (v == parent[u]) 
				continue;
				
			if (parent[v] >= 0) 
				return false;
				
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
		if (index[s] >= 0) 
			continue;

		index[s] = 0;
		std::vector<int> stack = {s};

		for (int i = 0; i < stack.size(); ++i) {
			int u = stack[i];
			for (int v: G.adj[u]) {
				if (index[v] >= 0) 
					continue;

				index[v] = stack.size();
				stack.emplace_back(v);
			}
		}

		Graph H(stack.size());
		for (int u: stack) {
			for (int v: G.adj[u]) {
				if (u >= v) 
					continue;
				
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

void printPartitionClassesOfGraph(Graph res) {
		std::cerr << res.n << " " << res.m << "\n";
		for (int i = 0; i < res.n; i += 1) {
			std::cerr << "Ecke: " << i + 1 << ": ";
			for (int j: res.partClasses[i]) 
				std::cerr << " " << j + 1 << " "; 
		std::cerr << "\n";
		}
		std::cerr << "\n";
}