#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <set>
#include <sstream>

// x,y
using point_t = std::pair<short, short>;
// id, point
using node_t = std::pair<short, point_t>;
// id of nearest if there is one
using nearest_t = std::optional<node_t>;
// distance, node @ that distance
using distances_t = std::multimap<short, node_t>;

short distance(point_t p, node_t n, bool debug = false) {
  return abs(p.first - n.second.first) + abs(p.second - n.second.second);
}

node_t make_node(std::string line, int id) {
  int comma = line.find(",");
  std::stringstream parse;
  short x;
  short y;
  parse = std::stringstream(line.substr(0, comma));
  parse >> x;
  parse = std::stringstream(line.substr(comma + 2));
  parse >> y;
  return node_t{id, point_t{x, y}};
}

distances_t measure(point_t p, std::set<node_t> &nodes) {
  distances_t ds{};
  for (node_t n : nodes) {
    short d = distance(p, n);
    ds.insert({d, n});
  }
  return ds;
}

int total_distance(distances_t ds) {
  return std::accumulate(ds.begin(), ds.end(), 0, [](int total, auto next) {
    return total + next.first;
  });
}

nearest_t find_nearest(distances_t ds) {
  auto first = ds.begin();
  auto second = ++ds.begin();
  return (first->first == second->first) ? nearest_t{}
                                         : nearest_t{first->second};
}

int nbest(int pbest, std::pair<node_t, short> nnode,
          const std::set<node_t> &edges) {
  if (edges.count(nnode.first) == 0) {
    return nnode.second > pbest ? nnode.second : pbest;
  }
  return pbest;
}
