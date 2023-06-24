#include "subgraphCounting.cpp"
#include "testGraph.cpp"
#include <chrono> // Für Zeitmessung

int main() {
	ios::sync_with_stdio(false); 
	cin.tie(0);  
	// Startzeitpunkt erfassen
	auto startTime = std::chrono::high_resolution_clock::now();

	Graph p11 = getPk(11); 
	generateSpasm(p11); 
	std::cout << " " << p11.spasms.size() << "\n";
	//Graph p12 = getPk(12); 
	//SubgraphCounting<int64_t> s(p10, p12); 
	//std::cout << s.countSubgraphs() << "\n";

	// Endzeitpunkt erfassen
	auto endTime = std::chrono::high_resolution_clock::now();

	// Zeitdifferenz berechnen und in Millisekunden umwandeln
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	// Zeit in Millisekunden ausgeben
	std::cout << "Execution time: " << duration.count() << " milliseconds\n";

	//runAllTests();
	return 0;
}
