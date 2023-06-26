#include "hom.hh" // achte auf doppeltes include, wenn spasm wieder auskommentiert 
#include <vector>
#include <iostream>
#include <future> // Für std::async und std::future

template <class Value>
struct SubgraphCounting {
    Graph H;
    Graph G;

    SubgraphCounting(Graph _H, Graph _G) : H(_H), G(_G) {}

    Value countSubgraphs() {
        if (H.n > G.n or H.m > G.m)
            return 0;

        std::vector<Value> factorials(21); 
        factorials[0] = 1LL; 

        for (Value i = 1; i < 21LL; i += 1LL) 
            factorials[i] = factorials[i - 1] * i; 

        auto getBlockFactors = [&](Graph& cur) -> Value {
            Value ans = 1; 
            std::vector<bool> vis(cur.n); 
            for (int i = 0; i < cur.n; i += 1) {
                int p = cur.dsu.get(i); 
                if (not vis[p]) {
                    ans *= factorials[Value(cur.dsu.size(p) - 1)];
                    vis[p] = true; 
                }
            }
            return ans; 
        }; 

        generateSpasm(H); 
        std::vector<std::pair<int, Graph>> spasm = H.spasms; 
        Value subgraphs = 0; 

        bool parallelisieren = H.spasms.size() >= 15; 
        if (not parallelisieren) {
            for (auto& h: spasm) {
                HomomorphismCounting<Value> homCounter(h.second, G);
                Value coeff = h.first * 1LL * getBlockFactors(h.second); 
                coeff *= (abs(H.n - h.second.n) & 1 ? -1LL : 1LL); 
                subgraphs += coeff * homCounter.run(); 
            }

            Value automorphisms = 0; 
            std::vector<Graph> connectedComponentsH = connectedComponents(H); 

            for (auto& h: connectedComponentsH)
                automorphisms += h.countAutomorphisms();

            return subgraphs / automorphisms; 
        } 

        std::vector<std::future<Value>> futures1;
        for (auto& h: spasm) {
            futures1.emplace_back(std::async(std::launch::async, [&] {
                HomomorphismCounting<Value> homCounter(h.second, G);
                Value coeff = h.first * 1LL * getBlockFactors(h.second); 
                coeff *= (abs(H.n - h.second.n) & 1 ? -1LL : 1LL);
                return coeff * homCounter.run();
            }));
        }

        for (auto &future : futures1) 
            subgraphs += future.get();

        Value automorphisms = 0; 
        std::vector<Graph> connectedComponentsH = connectedComponents(H); 

        std::vector<std::future<Value>> futures2;
        for (auto& h: connectedComponentsH) {
            futures2.emplace_back(std::async(std::launch::async, [&] {
                return h.countAutomorphisms();
            }));
        }

        for (auto &future : futures2) 
            automorphisms += future.get();
        
        return subgraphs / automorphisms; 
    }
};

struct countPathsOnePair {
    Graph g;
    int s, t;
    std::vector<int> dist_s, dist_t;

    countPathsOnePair(const Graph& g, int s, int t) : g(g), s(s), t(t), dist_s(g.n, -1), dist_t(g.n, -1) {}

    void dfs(int u, std::vector<bool>& visited) {
        visited[u] = true;
        for (int v : g.adj[u]) {
            if (!visited[v]) {
                dfs(v, visited);
            }
        }
    }

    void bfs(int start, std::vector<int>& dist) {
        std::queue<int> q;
        q.push(start);
        dist[start] = 0;
        while (not q.empty()) {
            int u = q.front(); q.pop();
            for (int v : g.adj[u]) {
                if (dist[v] == -1) {
                    dist[v] = dist[u] + 1;
                    q.push(v);
                }
            }
        }
    }

    int64_t countSTPaths(int k) {
        std::vector<bool> visited(g.n, false);
        dfs(s, visited);

        if (not visited[t]) 
            return 0;

        bfs(s, dist_s);
        bfs(t, dist_t);

        std::vector<bool> remove(g.n);
        int sz = g.n; 

        for (int v = 0; v < g.n; ++v) {
            if (not visited[v] or dist_s[v] + dist_t[v] > k) {
                remove[v] = true;
                sz -= 1;
            }
        }

        if (sz > 22) 
            return 0;   

        int64_t ans = 0; 
        for (int N = 3; N <= sz; N += 1) {
            std::vector<vector<int64_t>> dp(1 << N, std::vector<int64_t> (N, 0LL));
        }

        return ans; 
    }
}; 
