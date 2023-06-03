//#include <execution>
#include "graph.hh"
#include <unordered_set>
//#include <mutex>
#include <cmath>
#include <map>
#include <algorithm>
#include <numeric>  

//std::mutex graphMutex;

// Generate a hash value for a graph.
// size_t hash(const Graph &g) {
//     // TODO: Implement a proper hash function for graph
//     // This is a placeholder, replace with your own implementation.
//     size_t result = 0;

//     // For example, you could combine the hashes of the adjacency lists of each node.
//     for (const auto &adjList : g.getAdjacencyLists()) {
//         for (int node : adjList) {
//             result ^= std::hash<int>{}(node);
//         }
//     }

//     return result;
// }

size_t graphHash(const Graph &g) {
    std::string adjacencyMatrixString;

    // Generate the adjacency matrix string.
    for (int i = 0; i < g.n; i++) {
        for (int j = 0; j < g.n; j++) {
            if (std::find(g.adj[i].begin(), g.adj[i].end(), j) != g.adj[i].end()) {
                // If there is an edge between vertices i and j, append '1'.
                adjacencyMatrixString += '1';
            } else {
                // If there is no edge, append '0'.
                adjacencyMatrixString += '0';
            }
        }
    }

    // Use the std::hash function on the string.
    return std::hash<std::string>{}(adjacencyMatrixString);
}


std::map<std::pair<int, int>, std::vector<std::pair<int, Graph>>> mp;

std::vector<Graph> generateSpasm(Graph &g) { 
    cout << " in der generateSpasm : " << g.n << " " << g.m << "\n";
    // we assume that g is the initial P_k graph
    // Get all pairs of non-neighboring vertices.
    std::vector<std::pair<int, int>> pairs = g.getNonNeighbors();

    for (auto &pair : pairs) {
        // Contract the graph at the given vertices.
        Graph newGraph = contract(g, pair.first, pair.second);

        // Create a key for the map based on the number of nodes and edges.
        std::pair<int, int> key = {newGraph.n, newGraph.m};

        // Calculate the hash of the new graph.
        size_t newGraphHash = graphHash(newGraph);

        // Check if a graph isomorphic to the new graph is already in mp[key].
        bool isomorphicExists = false;
        for (auto &pairInMap : mp[key]) {
            if (newGraph == pairInMap.second) {
                isomorphicExists = true; 
                break; 
            }
            // Only check isomorphism if the hashes match.
            //if (pairInMap.first == newGraphHash && isomorphic(pairInMap.second, newGraph)) {
            //    isomorphicExists = true;
            //    break;
            //}
        }

        // If no isomorphic graph is in the map, add the new graph.
        if (!isomorphicExists) {
            mp[key].push_back({newGraphHash, newGraph});
            generateSpasm(newGraph);
        }
    }

    // Concatenate all vectors in mp to get the final result.
    std::vector<Graph> spasms = {g};
    for (auto &pair : mp) {
        for(auto &graphPair : pair.second) {
            spasms.push_back(graphPair.second);
        }
    }

    return spasms;
}