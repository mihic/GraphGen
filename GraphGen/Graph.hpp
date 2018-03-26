#include "stdafx.h"

class Graph {
public:
  int num_nodes;
  int num_edges;
  std::vector<std::vector<std::pair<int, int>>> adj_list;

  Graph(int a) : adj_list(a, std::vector<std::pair<int, int>>()), num_nodes(a), num_edges(0) {}
  Graph() {}

  void add_edge(int source, int dest, int cost) {
    adj_list[source].emplace_back(std::make_pair(dest, cost));
    num_edges++;
  }

  void output_pajek() {
    std::cout << "*vertices " << num_nodes << std::endl;
    std::cout << "*arcs" << std::endl;
    for (int i = 0; i < num_nodes; ++i) {
      auto neighbours = adj_list[i];
      for (auto j : neighbours) {
        std::cout << i + 1 << ' ' << j.first + 1 << ' ' << j.second << std::endl;
      }
    }
  }
  void output_pmed(int num_centers) {
    std::cout << num_nodes << ' ' << num_edges << ' ' << num_centers << std::endl;
    for (int i = 0; i < num_nodes; ++i) {
      auto neighbours = adj_list[i];
      for (auto j : neighbours) {
        std::cout << i + 1 << ' ' << j.first + 1 << ' ' << j.second << std::endl;
      }
    }
  }
};

Graph RandomGraph(int n, std::default_random_engine random_engine, bool directed, double density, int cmin, int cmax) {
  Graph G(n);
  std::uniform_int_distribution<int> r_int(cmin, cmax);
  std::uniform_real_distribution<double> r_double(0, 1);

  for (int i = 0; i < n; ++i) {
    int j;
    if (directed) {
      j = 0;
    }
    else {
      j = i + 1;
    }
    for (; j < n; ++j) {
      if (r_double(random_engine) < density) {
        G.add_edge(i, j, r_int(random_engine));
      }
    }
  }
  return G;
}

Graph Random2DGridGraph(int n, std::default_random_engine random_engine, bool directed, double density, int cmin, int cmax) {
  std::uniform_int_distribution<int> r_int(cmin, cmax);
  std::uniform_real_distribution<double> r_double(0, 1);
  int old_n = n;
  n = int(std::sqrt(n));
  int n2 = n * n;
  if (n2 != old_n) {
    std::cerr << "num nodes was not a perfect square, new n is " << n2 << std::endl;;
  }
  Graph g(n2);
  for (int i = 0; i < n2; ++i) {

    int d = i + n; //down
    int l = i - 1; //left
    int u = i - n; //up
    int r = i + 1; //right

    //down
    if (d < n2) {
      if (r_double(random_engine) < density) {
        g.add_edge(i, d, r_int(random_engine));
      }
    }
    //left
    if (i%n != 0) {
      if (r_double(random_engine) < density) {
        g.add_edge(i, l, r_int(random_engine));

      }
    }
    if (directed) {
      //up
      if (u >= 0) {
        if (r_double(random_engine) < density) {
          g.add_edge(i, u, r_int(random_engine));
        }
      }
      //right
      if (r%n != 0) {
        if (r_double(random_engine) < density) {
          g.add_edge(i, r, r_int(random_engine));
        }
      }
    }

  }
  return g;
}