#include <bits/stdc++.h>
using namespace std;   
#include <cassert>
#include "graph.hh"

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

 
void testIsomorph() {
    Graph nw1(8); 
    Graph nw2(8); 
    Graph nw3(5); 
    Graph nw4(3); 
    for (int i = 0; i < 3; i += 1) {
        nw1.addEdge(i, i + 1); 
        nw2.addEdge(i, i + 1); 
        nw3.addEdge(i, i + 1); 
    } 
    assert(nw1 == nw2); 
    assert(nw1 != nw3); 
    assert(nw1 != nw4);  
}

void testContraction() {
    Graph g(6); 
    Graph h(6); 
    for (int i = 0; i < 3; i += 1) 
        g.addEdge(i, i + 1); 

    h = g;

    assert(contract(g, 0, 2) == contract(h, 1, 3));
    assert(contract(g, 0, 3) != contract(h, 1, 3));

    Graph cycle = getPk(4); 
    Graph nwH = getPk(5);

    cycle.addEdge(0, 3); 
    nwH.addEdge(3, 4); 

    assert(contract(nwH, 0, 4) == cycle); 
}

void testOldNameContract() { // newly added might check interaction
	Graph g(4); 
	g.addEdge(0,1); 
	g.addEdge(0,2); 
	g.addEdge(0,3); 

    generateSpasm(g); 
    auto Y = g.spasms;
    assert(Y.size() == 3); // funktioniert noch nicht

	Graph h1 = contract(g, 1, 2);
	auto X = h1.getNonNeighbors(); // funktioniert noch nicht
	assert(X.size() == 1);

	Graph h2 = contract(h1, 0, 2); 
	assert(h2.dsu.size(0) == 1); 
	assert(h2.dsu.size(1) == 3); 
	assert(h2.oldName[0] == 1); 
	assert(h2.oldName[1] == 0);
}

void testContractionDSU() {
    Graph g = getPk(5);
    g = contract(g, 0, 2); 
    g = contract(g, 0, 3); 
    g = contract(g, 1, 2); 
    assert(g.dsu.size(0) == 3); 
    assert(g.dsu.size(1) == 2);

    int prod = 1; 
    std::vector<bool> vis(5); 
    for (int i = 0; i < g.n; i += 1) {
        int par = g.dsu.get(i); 
        if (not vis[par]) {
            vis[par] = true; 
            prod *= g.dsu.size(par); 
        }
    }
    assert(prod == 6); 

    int p1 = g.dsu.get(0); 
    assert(p1 == g.dsu.get(2));
    assert(p1 == g.dsu.get(4));

    int p2 = g.dsu.get(1); 
    assert(p2 == g.dsu.get(3));

    bool okFirstBlock = g.oldName[0] == 0 or g.oldName[0] == 2 or g.oldName[0] == 4; 
    bool okSecondBlock = g.oldName[1] == 1 or g.oldName[1] == 3;
    assert(okFirstBlock); 
    assert(okSecondBlock); 
}


void testGenerateSpasm() {
    Graph p5 = getPk(5);
    generateSpasm(p5); 
    auto spasm = p5.spasms;

    vector<Graph> need = {p5, getPk(4), getPk(3), getPk(2), contract(p5, 0, 4),
                         contract(p5, 0, 3), contract(getPk(4), 0, 3), contract(getPk(5), 1, 3)};

    bool isFine = true;  
    for (auto g: spasm) { 
        bool ok = false; 
        for (auto h: need) 
            ok |= h == g.second; 

        isFine = isFine and ok; 
        if (not isFine) 
            break; 
    }

    assert(isFine);
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