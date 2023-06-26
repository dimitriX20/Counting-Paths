#include "subgraphCounting.cpp"
#include "testGraph.cpp"
#include <chrono> // Für Zeitmessung

int main() {
	//ios::sync_with_stdio(false); 
	//cin.tie(0);  
	// Startzeitpunkt erfassen
	auto startTime = std::chrono::high_resolution_clock::now();

	//Graph p11 = getPk(11); 
	//generateSpasm(p11); 
	//std::cout << " " << p11.spasms.size() << "\n";
	// Graph p2 = getPk(8); 
	// Graph p12 = getPk(555);
	// p12.addEdge(199, 0);
	// p2.addEdge(1, 3);  
	// p12.addEdge(444, 333); 
	// p12.addEdge(123, 125); 
	// p12.addEdge(1, 211);  

	// SubgraphCounting<int64_t> s(p2, p12); 
	// std::cout << s.countSubgraphs() << "\n";
	// std::cout << p2.spasms.size() << "\n";

	runAllTests();
	// Endzeitpunkt erfassen
	auto endTime = std::chrono::high_resolution_clock::now();

	// Zeitdifferenz berechnen und in Millisekunden umwandeln
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	// Zeit in Millisekunden ausgeben
	std::cout << "Execution time: " << duration.count() << " milliseconds\n";

	return 0;
}
