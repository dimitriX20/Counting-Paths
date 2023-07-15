#include "subgraphCounting.cpp"
#include "testGraph.cpp"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

int main() {
    int n, m, start, end, k;
    Graph gg(0); 
    std::string line;
    bool terminals = false;

    while (std::getline(std::cin, line)) {
        if (line.empty()) 
			continue;  

        char line_type = line[0];
        std::istringstream iss(line.substr(1)); // remove line_type and parse the rest

        if (line_type == 'c') {
            continue;
        } else if (line_type == 'p') {
            std::string problem_type;
            iss >> problem_type >> n >> m;
            gg = Graph(n); 
        } else if (line_type == 'e') {
            int v1, v2;
            iss >> v1 >> v2;
            v1--, v2--; 
            gg.addEdge(v1, v2); 
        } else if (line_type == 'l') {
            iss >> k; 
            k += 1;
        } else if (line_type == 't') {
            iss >> start >> end;
            start--, end--; 
            terminals = true;
            break;
        }
    } 

	if (terminals) { 
		countPathsOnePair counter(gg, start, end);
		std::cout << counter.countSTPaths(k) << "\n";
	} 

	else 
		std::cout << countSubgraphs(gg, k) << "\n";
    
    return 0;
}