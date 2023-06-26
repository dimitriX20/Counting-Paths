#pragma once

#include <algorithm> // Für sort(), lower_bound(), set_intersection()
#include <numeric> // Für accumulate()
#include <unordered_map> // Für unordered_map()
#include <vector> // Für vector()
#include <set> // Für set_intersection()
#include <iterator> // Für back_inserter()
#include "graph.hh"
#include "treedec.hh"

#include "omp.h"

template <class Value>
struct HomomorphismCounting {
  Graph G, F;
  NiceTreeDecomposition NTD;
  HomomorphismCounting(Graph F_, Graph G_) : F(F_), G(G_) {
    for (auto &nbh: F.adj) sort(nbh.begin(), nbh.end());  
    for (auto &nbh: G.adj) sort(nbh.begin(), nbh.end());  
    NTD = niceTreeDecomposition(F);
  }
  struct VectorHash {
    size_t operator()(const std::vector<int> &x) const {
      constexpr size_t p = 1e9+7;
      size_t hash = 0;
      for(int i = 0; i < x.size(); ++i) {
        hash += p * hash + x[i];
      }
      return hash;
    }
  };
  using DPTable = std::unordered_map<std::vector<int>, Value, VectorHash>;
  Value run() {
    auto [I, X] = run(NTD.root);
    return I[std::vector<int>()];
  }
  std::pair<DPTable, std::vector<int>> run(int x) {
    DPTable I, J, K;
    std::vector<int> X;
    if (NTD.isLeaf(x)) {
//std::cerr << "  NTD.isLeaf passed!\n";
      X = {NTD.vertex(x)};

//std::cerr << "  NTD.isVertex passed!\n";
      for (int a = 0; a < G.n; ++a) {
        std::vector<int> phi = {a};
        I[phi] = 1;

//std::cerr << "  I[phi] passed!\n";
      }
    } else if (NTD.isIntroduce(x)) {
//std::cerr << "  NTD.isIntroduce(x) passed!\n";
      std::tie(J, X) = run(NTD.child(x));
//std::cerr << "  std::tie(J, X) = run(NTD.child(x)); passed!\n";
      int p = std::distance(X.begin(), std::lower_bound(X.begin(), X.end(), NTD.vertex(x)));
//std::cerr << "  p = std::distance(X.begin()... passed!\n";
      std::vector<int> candidate_id;
      int i = 0;
      for (auto v: F.adj[NTD.vertex(x)]) {
        while (i < X.size() && X[i] < v) ++i;
        if (i == X.size()) break;
        if (X[i] == v) candidate_id.emplace_back(i);
      }
//std::cerr << "  for (auto v: F.adj[NTD.vertex(x)]) .. komplette Schleife passed!\n";
      if (candidate_id.empty()) {
        candidate_id.resize(G.n);
        std::iota(candidate_id.begin(), candidate_id.end(), 0);
      }

//std::cerr << "  if (candidate_id.empty() komplett passed!\n";
      for (auto [phi, val]: J) {
      //  std::cerr << "  Z.73!\n";
        std::vector<int> candidate_a;
        std::sort(candidate_id.begin(), candidate_id.end(), [&](int i, int j) {
          return G.adj[i].size() < G.adj[j].size();
        });
       // std::cerr << "  Z.78!\n";
        for (int i: candidate_id) {
          if (i < 0 or i >= phi.size())
            continue;
          int a = phi[i];
         // std::cerr << "  Z.81!\n";
          if (candidate_a.empty()) {
            candidate_a = G.adj[a];
            //std::cerr << "  Z.84!\n";
          } else {
            std::vector<int> temp; 
            std::set_intersection(candidate_a.begin(), candidate_a.end(),
                G.adj[a].begin(), G.adj[a].end(), std::back_inserter(temp));
        //  std::cerr << "  Z.89!\n";          
            candidate_a.swap(temp);
//            std::cerr << "  Z.91!\n";
          }
        }
//std::cerr << "  for (auto [phi, val]: J) { Schleife bis Z.94 komplett passed!\n";
        auto psi = phi;
        psi.insert(psi.begin()+p, 0);
        for (int a: candidate_a) {
          psi[p] = a;
          I[psi] = val;
        }
      }
      X.insert(X.begin()+p, NTD.vertex(x));
//std::cerr << "  Zeile 103, also for aut [phi, val]: J und insert passed \n";
    } else if (NTD.isForget(x)) {
      std::tie(J, X) = run(NTD.child(x));
      int p = std::distance(X.begin(), std::lower_bound(X.begin(), X.end(), NTD.vertex(x)));
      X.erase(X.begin() + p);

      for (auto [phi, val]: J) {
        auto psi = phi;
        psi.erase(psi.begin() + p);
        I[psi] += val;
      }

//std::cerr << "  Zeile 115 passed \n";
    } else if (NTD.isJoin(x)) {
      std::tie(J, X) = run(NTD.left(x));
      std::tie(K, X) = run(NTD.right(x));
      if (J.size() > K.size()) swap(J, K);
      for (auto& [phi, val]: J) {
        if (K.count(phi)) I[phi] = val * K[phi];
      }
    }
 //   std::cerr << "  Zeile 124 passed \n";
    return std::make_pair(I, X);
  }
};

// Computing hom(F, G) for tree F.
template <class Value>
struct HomomorphismCountingTree {
  Graph F, G;
  HomomorphismCountingTree(Graph F_, Graph G_) : F(F_), G(G_) { }
  Value run() {
    auto hom_r = run(0, -1);
    return accumulate(hom_r.begin(), hom_r.end(), Value(0));
  }
  std::vector<Value> run(int x, int p) {
    std::vector<Value> hom_x(G.n, 1);
    for (int y: F.adj[x]) {
      if (y == p) continue;
      auto hom_y = run(y, x);
      for (int a = 0; a < G.n; ++a) {
        Value sum = 0;
        for (int b: G.adj[a]) sum += hom_y[b];
        hom_x[a] *= sum;
      }
    }
    return hom_x;
  }
};

// both F and G are connected
template <class Value>
Value hom_(Graph F, Graph G) {
  if (isTree(F)) {
    return HomomorphismCountingTree<Value>(F, G).run();
  } else {
    return HomomorphismCounting<Value>(F, G).run();
  }
}
template <class Value>
Value hom(Graph F, Graph G) {
  Value value = 1;
  for (auto Fi: connectedComponents(F)) {
    Value value_i = 0;
    for (auto Gj: connectedComponents(G)) 
      value_i += hom_<Value>(Fi, Gj);
    value *= value_i;
  }
  return value;
}
