#pragma once

#include <vector>
#include<set> 

struct Graph {
  int n, m;
  std::vector<std::vector<int>> adj;
  std::vector<std::set<int>> s; 

  Graph(int n) : n(n), adj(n), s(n) {
    m = 0;
  }

  void addEdge(int u, int v) {
    adj[u].push_back(v);
    adj[v].push_back(u);
    s[u].insert(v); 
    s[v].insert(u); 
    m += 1;
  }

  std::vector<std::pair<int, int>> getNonNeighbors() {
    std::vector<std::pair<int, int>> ans; 
    for (int i = 0; i < n; i += 1) {
        for (int j = i + 1; j < n; j += 1) {
            if (s[i].find(j) == s[i].end()) 
                ans.emplace_back(i, j); 
        }
    }
    return ans; 
  }

  // Check whether two graphs are isomorphic.
bool isomorphic(const Graph &h) { 
    if (n != h.n)
        return false;

    // Create vectors of degrees
    std::vector<int> deg1(n);
    std::vector<int> deg2(h.n);

    for (int i = 0; i < n; i++)
        deg1[i] = adj[i].size();

    for (int i = 0; i < h.n; i++)
        deg2[i] = h.adj[i].size();

    std::sort(deg1.begin(), deg1.end());
    std::sort(deg2.begin(), deg2.end());
    if (deg1 != deg2)
        return false;
 
    std::vector<int> p(h.n);
    std::iota(p.begin(), p.end(), 0);

    do {
        bool isIsomorphic = true;
        for (int i = 0; i < n; i++) {
            for (int j : s[i]) {
                if (h.s[p[i]].find(p[j]) == h.s[p[i]].end()) {
                    isIsomorphic = false;
                    break;
                }
            }
            if (!isIsomorphic)
                break;
        }
        if (isIsomorphic)
            return true;
    } while (std::next_permutation(p.begin(), p.end()));

    return false;
}

  // Copy constructor
  Graph(const Graph& other) {
      // Perform a deep copy of the graph
      n = other.n;
      m = other.m;
      adj = other.adj;
      s = other.s;
  }

  // Assignment operator
  Graph& operator=(const Graph& other) {
      if (this != &other) {
          // Perform a deep copy of the graph
          n = other.n;
          m = other.m;
          adj = other.adj;
          s = other.s;
      }
      return *this;
  }

  // equal operator
  bool operator==(const Graph other) {
      return isomorphic(other);
  }

  //not equal operator
  bool operator!=(const Graph other) {
      return not isomorphic(other);
  }
};

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
  map<int, int> nwName; 
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
