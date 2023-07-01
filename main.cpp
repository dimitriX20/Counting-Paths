#include "subgraphCounting.cpp"
#include "testGraph.cpp"
#include <chrono> // Für Zeitmessung

Graph createGridGraph(int rows, int columns) {
    Graph grid(rows * columns);
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            int vertex = i * columns + j;
            
            // Add edge to the right (if possible)
            if (j < columns - 1) {
                int rightVertex = vertex + 1;
                grid.addEdge(vertex, rightVertex);
            }
            
            // Add edge to the bottom (if possible)
            if (i < rows - 1) {
                int bottomVertex = vertex + columns;
                grid.addEdge(vertex, bottomVertex);
            }
        }
    }
    
    return grid;
}

int main() {
	//ios::sync_with_stdio(false); 
	//cin.tie(0);  
	// Startzeitpunkt erfassen
	auto startTime = std::chrono::high_resolution_clock::now();

	// Graph p4 = getPk(4); 
	// p4.addEdge(0, 2); 

	Graph g(8); 
	for (int i = 1; i < 8; i += 1) {
		for (int j = i + 1; j < 8; j += 1) {
			g.addEdge(i, j); 
		}
	}
	countSubgraphs(g, 6);
	// g.addEdge(0, 5); 

	// HomomorphismCounting<int64_t> hh(p4, g); 
	// for (int i = 0; i < 5; i += 1) {
	// 	if (i == 3) 
	// 		continue; 
	// 	Graph nwGraph = p4;
	// 	nwGraph.addNode(); 
	// 	nwGraph.addEdge(nwGraph.dsu.get(3), 4); 
	// 	Graph nw = contract(nwGraph, i, 4); 
	// }

	//Graph p11 = getPk(11); 
	//generateSpasm(p11); 
	//std::cout << " " << p11.spasms.size() << "\n";

	// Graph p2 = getPk(10); 
	// Graph p12 = getPk(222);
	// p12.addEdge(199, 0);
	// p2.addEdge(1, 3);   
	// p12.addEdge(123, 125); 
	// p12.addEdge(1, 211);  
	//Graph gitterGraph = createGridGraph(6, 6); 

	// Graph g(10); 
	// for (int i = 0; i < 10; i += 1) {
	// 	for (int j = i + 1; j < 10; j += 1) {
	// 		g.addEdge(i,j); 
	// 	}
	// }

	// Graph p5 = getPk(10); 
	// SubgraphCounting<int64_t> s(p5, g); //1814400

	// std::cout << s.countSubgraphs() << "\n"; // 1 262 816

	runAllTests();
	// Endzeitpunkt erfassen
	auto endTime = std::chrono::high_resolution_clock::now();

	// Zeitdifferenz berechnen und in Millisekunden umwandeln
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	// Zeit in Millisekunden ausgeben
	std::cout << "Execution time: " << duration.count() << " milliseconds\n";

	return 0;
}
