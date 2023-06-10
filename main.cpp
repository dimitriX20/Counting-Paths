#include <bits/stdc++.h>
using namespace std;   
#include "hom.hh" // achte auf doppeltes include, wenn spasm wieder auskommentiert 
#include "testSpasmGenerator.cpp"

int64_t countSubgraphs(Graph H, Graph G) {
  auto sp = generateSpasm(H); 
  std:vector<pair<int64_t, Graph>> spasm(sp.begin(), sp.end()); 

  int64_t subgraphs = 0; 
  for (auto& h: spasm) {
    HomomorphismCounting<int64_t> homCounter(h.second, G);
    subgraphs += h.first * homCounter.run();
  }

  int64_t automorphisms = 0; 
  std::vector<Graph> connectedComponentsH = connectedComponents(H); 

  //for (auto& h: connectedComponentsH)
 //   automorphisms += countAutomorphisms(h); 

  return subgraphs / automorphisms; 
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
	Graph G(1);  
	test_tree();
	runAllTests();
	return 0;
}