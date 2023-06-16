#include <bits/stdc++.h>
using namespace std;   
#include "hom.hh" // achte auf doppeltes include, wenn spasm wieder auskommentiert 
#include "testSpasmGenerator.cpp"

int64_t countAutomorphisms(const Graph &h, const Graph& g) { 
	int64_t ans = 0; 
	std::vector<int> p(h.n);
	std::iota(p.begin(), p.end(), 0);

	do {
			bool isIsomorphic = true;
			for (int i = 0; i < g.n; i++) {
					for (int j : g.s[i]) {
							if (h.s[p[i]].find(p[j]) == h.s[p[i]].end()) {
									isIsomorphic = false;
									break;
							}
					}
					if (!isIsomorphic)
							break;
			}
			if (isIsomorphic)
					ans += 1;
	} while (std::next_permutation(p.begin(), p.end()));

	return ans;
}

int64_t countSubgraphs(Graph H, Graph G) {
	std::vector<int64_t> factorials(21); 
	factorials[0] = 1LL; 

	for (int64_t i = 1; i < 21LL; i += 1LL) 
		factorials[i] = factorials[i - 1] * i; 
	
	auto getBlockFactors = [&](Graph& cur) -> int64_t {
		int64_t ans = 1; 
		std::vector<bool> vis(cur.n); 
		for (int i = 0; i < cur.n; i += 1) {
			int p = cur.dsu.get(i); 
			if (not vis[p]) {
				ans *= factorials[int64_t(cur.dsu.size(p) - 1)];
				vis[p] = true; 
			}
		}
		return ans; 
	}; 

	generateSpasm(H);
	std::vector<std::pair<int, Graph>> spasm = H.spasms; 
	int64_t subgraphs = 0; 

	for (auto& h: spasm) {
		HomomorphismCounting<int64_t> homCounter(h.second, G);
		int64_t coeff = h.first * 1LL * getBlockFactors(h.second); //(Block-Größen - 1)! von jeder Partition
		coeff *= (abs(H.n - h.second.n) & 1 ? -1LL : 1LL); // hier evtl. h.second.n modifizeren nachdem genSpasm final implementiert
		subgraphs += h.first * coeff * homCounter.run(); 
	}

	int64_t automorphisms = 0; 
	std::vector<Graph> connectedComponentsH = connectedComponents(H); 

	for (auto& h: connectedComponentsH)
   		automorphisms += countAutomorphisms(h, h); 

	return subgraphs;// / automorphisms; 
}

void test_tree() {
	int t = 5;
	Graph T(t);
	for (int i = 1; i < t; ++i) {
		T.addEdge(rand() % i, i);
	}
	int n = 20;
	Graph G(n);
	for (int i = 0; i < n; ++i) {
		for (int j = i+1; j < n; ++j) {
			if (rand() % 2 == 0) {
				G.addEdge(i, j);
			}
		}
	}
	HomomorphismCounting<int> hom(T, G);
	HomomorphismCountingTree<int> homTree(T, G);
	cout << hom.run() << "\n";
	cout << homTree.run() << "\n";
	assert(hom.run() == homTree.run());
	}
 

int main() {
	ios::sync_with_stdio(false); 
	cin.tie(0);  
	//Graph G(1);  
	//test_tree();
	runAllTests();
	return 0;
}