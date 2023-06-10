// //#include <execution>
// #include "graph.hh"
// #include <unordered_set>
// //#include <mutex>
// #include <cmath>
// #include <map>
// #include <algorithm>
// #include <numeric>  

// //std::mutex graphMutex;

// std::map<std::pair<int, int>, std::vector<Graph>> mp;

// std::vector<Graph> generateSpasm(Graph &g) {  
//     // we assume that g is the initial P_k graph
//     // Get all pairs of non-neighboring vertices.
//     std::vector<std::pair<int, int>> pairs = g.getNonNeighbors();

//     for (auto &pair : pairs) {
//         // Contract the graph at the given vertices.
//         Graph newGraph = contract(g, pair.first, pair.second);

//         // Create a key for the map based on the number of nodes and edges.
//         std::pair<int, int> key = {newGraph.n, newGraph.m};

//         // Check if a graph isomorphic to the new graph is already in mp[key].
//         bool isomorphicExists = false;
//         for (auto &pairInMap : mp[key]) {
//             if (newGraph == pairInMap) {
//                 isomorphicExists = true; 
//                 break; 
//             }
//         }

//         // If no isomorphic graph is in the map, add the new graph.
//         if (!isomorphicExists) {
//             mp[key].push_back(newGraph);
//             generateSpasm(newGraph);
//         }
//     }

//     // Concatenate all vectors in mp to get the final result.
//     std::vector<Graph> spasms = {g};
//     for (auto &pair : mp) {
//         for(auto &graphPair : pair.second) {
//             spasms.push_back(graphPair);
//         }
//     }

//   //  mp.clear();
//     return spasms;
// }