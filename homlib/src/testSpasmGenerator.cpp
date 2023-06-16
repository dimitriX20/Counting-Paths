#include <bits/stdc++.h>
using namespace std;   
#include <cassert>
#include "graph.hh"
 
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

	Graph h1 = contract(g, 1, 2);
	auto X = h1.getNonNeighbors(); // funktioniert noch nicht
	//assert(X.size() == 1);

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

// void testGenerateSpasmSize() {
//     Graph p5 = getPk(5);
//     Graph p4 = getPk(4);
//     Graph p3 = getPk(3); 

//     //assert(generateSpasm(p5).size() == 8); 
//     auto X = generateSpasm(p3); 
//     cout << " " << p3.n << " " << p3.m << "\n";
//     cout << X.size() << "\n";
//     for (auto i: X)
//         print(i);
// // assert(generateSpasm(p4).size() == 4); 
//   //  assert(generateSpasm(p3).size() == 2);
// }

void testGenerateSpasm() {
    Graph p5 = getPk(5);
    auto spasm = generateSpasm(p5); 

    vector<Graph> need = {p5, getPk(4), getPk(3), getPk(2), contract(p5, 0, 4),
                         contract(p5, 0, 3), contract(getPk(4), 0, 3), contract(getPk(5), 1, 3)};

    bool isFine = true; 
    cout << " " << spasm.size() << "\n";
    for (auto g: spasm) {
        print(g.second);
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
    testContractionDSU;
    testIsomorph(); 
    testContraction(); 
    testGenerateSpasm();
    testOldNameContract();
}