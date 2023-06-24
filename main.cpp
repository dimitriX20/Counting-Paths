#include "subgraphCounting.cpp"

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
	SubgraphCounting<int64_t> s(K13, peter); 
	assert(s.countSubgraphs() == 10); 
}

void testCountSubgraphs2() {
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
	Graph p = getPk(4); 
	SubgraphCounting<int64_t> s2(p, g);
	assert(s2.countSubgraphs() == 8);
}

void runAllTests() {
	testCountSubgraphs(); 
	testCountSubgraphs2(); 
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
 
	runAllTests();
	return 0;
}