#include<iostream>
#include<map>
#include "graph.hh"
#include<vector>
#include "hom.hh"

Graph getPk(int k) {
  if (k < 1) 
    return Graph(0); 

  Graph g(k); 
  for (int i = 0; i < k - 1; i += 1)
    g.addEdge(i, i + 1); 

  return g; 
}

void print(const Graph& g) {
  for (int i = 0; i < g.n; ++i) {
    std::cout << "Vertex " << i << ": ";
    for (int v : g.adj[i]) {
      std::cout << v << " ";
    }
    std::cout << "\n";
  }
}

Graph contract(const Graph& h, int v, int u){
  if (v >= h.n or u >= h.n) 
    return Graph(0); 

  Graph res(h.n - 1);
  std::set<int> allNeighbors; 
  
  int idx = 0; 
  std::map<int, int> nwName; 
  nwName[v] = nwName[u] = idx; 

  for (int i = 0; i < h.n; i += 1) {
    if (i == v or i == u) 
      continue; 

    idx += 1; 
    nwName[i] = idx; 
  }

  for (int i: h.adj[v]) {
    if (i != v and i != u)
      allNeighbors.insert(nwName[i]); 
  }

  for (int i: h.adj[u]) {
    if (i != v and i != u)
      allNeighbors.insert(nwName[i]); 
  }

  for (int i: allNeighbors) {
    if (i != 0)
      res.addEdge(0, i); 
  }

  for (int i = 0; i < h.n; i += 1) {
    if (i == v or i == u) 
      continue; 

    for (int j: h.adj[i]) {
      if (j != v and j != u and (nwName[i] != nwName[j]) and res.s[nwName[i]].find(nwName[j]) == res.s[nwName[i]].end())  
        res.addEdge(nwName[i], nwName[j]); 
    }
  }

  return res; 
}

std::vector<std::tuple<DSU, int, Graph>> generateSpasm(Graph &g) {  
    // we assume that g is the initial P_k graph
    // Get all pairs of non-neighboring vertices.
    std::vector<std::pair<int, int>> pairs = g.getNonNeighbors();

    for (auto &pair : pairs) {
        // Contract the graph at the given vertices.
        Graph newGraph = contract(g, pair.first, pair.second);

        // Create a key for the map based on the number of nodes and edges.
        std::pair<int, int> key = {newGraph.n, newGraph.m};

        // Check if a graph isomorphic to the new graph is already in mp[key].
        bool isomorphicExists = false;
        for (auto &pairInMap : g.mp[key]) {
            if (newGraph == pairInMap) {
                isomorphicExists = true;  
                break; 
            }
        }

        // If no isomorphic graph is in the map, add the new graph.
        if (!isomorphicExists) {
            g.mp[key].push_back({1, newGraph});
            generateSpasm(newGraph);
        }
    }

    // Concatenate all vectors in mp to get the final result.
    std::vector<std::tuple<DSU, int, Graph>> spasms = {g};
    for (auto &pair : g.mp) {
        for(auto &graphPair : pair.second) {
            spasms.emplace_back(graphPair);
        }
    }

  //  mp.clear();
    return spasms;
}

bool isTree(Graph G) {
  std::vector<int> parent(G.n, -1);
  std::vector<int> stack = {0};
  for (int i = 0; i < stack.size(); ++i) {
    int u = stack[i];
    for (int v: G.adj[u]) {
      if (v == parent[u]) continue;
      if (parent[v] >= 0) return false;
      parent[v] = u;
      stack.push_back(v);
    }
  }
  return stack.size() == G.n;
}

std::vector<Graph> connectedComponents(Graph G) {
  std::vector<int> index(G.n, -1);
  std::vector<Graph> components;
  for (int s = 0; s < G.n; ++s) {
    if (index[s] >= 0) continue;
    index[s] = 0;
    std::vector<int> stack = {s};
    for (int i = 0; i < stack.size(); ++i) {
      int u = stack[i];
      for (int v: G.adj[u]) {
        if (index[v] >= 0) continue;
        index[v] = stack.size();
        stack.emplace_back(v);
      }
    }
    Graph H(stack.size());
    for (int u: stack) {
      for (int v: G.adj[u]) {
        if (u >= v) continue;
        H.addEdge(index[u], index[v]);
      }
    }
    components.emplace_back(H);
  }
  return components;
}
 

int64_t countSubgraphs(Graph H, Graph G) {
  auto sp = generateSpasm(H); 
  std::vector<pair<int64_t, Graph>> spasm(sp.begin(), sp.end()); 

  int64_t subgraphs = 0; 
  for (auto& h: spasm) {
    int vorzeichen = (abs(H.n - h.second.n) & 1 ? -1 : 1); // hier evtl. h.second.n modifizeren nachdem genSpasm final implementiert
    HomomorphismCounting<int64_t> homCounter(h.second, G);
    subgraphs += h.first * homCounter.run(); 

    /* 
      müssen das Produkt der (Block-Größen - 1)! von jeder Partition ermitteln 
      dazu speichern wir für jeden Spasm Graphen eine eigene DSU Datenstruktur  
    */ 


    /* 
      außerdem sollten wir die Anzahl der Vorkommnisse eines jeden Spasm Graphen
      beim generieren des Spasms speichern als Multiplikator 
    */  
  }

  int64_t automorphisms = 0; 
  std::vector<Graph> connectedComponentsH = connectedComponents(H); 

  //for (auto& h: connectedComponentsH)
 //   automorphisms += countAutomorphisms(h); 

  return subgraphs;// / automorphisms; 
}