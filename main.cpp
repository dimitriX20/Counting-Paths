#include "hom.hh" // achte auf doppeltes include, wenn spasm wieder auskommentiert 
#include "testSpasmGenerator.cpp"
#include <vector>
#include <iostream>

int64_t countSubgraphs(Graph H, Graph G) {
	if (H.n > G.n or H.m > G.m)
		return 0;

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
		//print(h.second); 
		//cout << " h.first: " << h.first << " coeff:" << coeff << "\n";
		subgraphs += coeff * homCounter.run(); 
	}

	int64_t automorphisms = 0; 
	std::vector<Graph> connectedComponentsH = connectedComponents(H); 

	for (auto& h: connectedComponentsH)
   		automorphisms += h.countAutomorphisms();

	//cout << " " << automorphisms << "\n"; 
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

void testCountSubgraphs() {
	Graph peter = createPetersonGraph(); 
	Graph K13(4); 
	K13.addEdge(0,1); 
	K13.addEdge(0,2); 
	K13.addEdge(0,3);
	assert(countSubgraphs(K13, peter) == 10); 
}

void testCountSubgraphs2() {
	//assert(countSubgraphs(p3, g) == 12);
}

void runAllTests() {
	testCountSubgraphs2(); 
	testCountSubgraphs(); 
	testOldNameContract();
    testContractionDSU;
    testIsomorph(); 
    testContraction(); 
    testGenerateSpasm();
	test_tree(); 
}

int main() {
	ios::sync_with_stdio(false); 
	cin.tie(0);  

	Graph g(10); 
	g.addEdge(1, 0);
	g.addEdge(2, 0);
	g.addEdge(3, 2);
	g.addEdge(4, 3);
	g.addEdge(5, 4);
	g.addEdge(6, 0);
	g.addEdge(7, 5);
	g.addEdge(8, 1);
	g.addEdge(9, 5);
	Graph p = getPk(3); 
	std::cout << countSubgraphs(p, g) << "\n";
	
	//testCountSubgraphs2();
	// runAllTests();
	return 0;
}