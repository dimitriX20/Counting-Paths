#include <bits/stdc++.h>
using namespace std;   
#include "hom.hh"
#include <cassert>
#include "spasmGenerator.cpp"
 
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
    for (auto g: spasm) {
        bool ok = false; 
        for (auto h: need) 
            ok |= h == g; 

        isFine = isFine and ok; 
        if (not isFine) 
            break; 
    }

    assert(isFine);
}

void runAllTests() {
    testIsomorph(); 
    testContraction(); 
    testGenerateSpasm();
}