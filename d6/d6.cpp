#include "d6.hpp"

int main() {

  std::cout << "d6" << std::endl;

  std::ifstream input("d6/input.txt");
  std::string line;
  std::set<node_t> nodes{};
  int i = 0;
  while (std::getline(input, line)) {
    nodes.insert(make_node(line, i++));
  }

  std::map<node_t, short> areas{};
  std::set<node_t> edges{};

  int p2_score = 0;

  const short size = 450;
  for (short i = 0; i < size; ++i) {
    for (short j = 0; j < size; ++j) {
      distances_t ds = measure(point_t{i, j}, nodes);
      nearest_t n = find_nearest(ds);
      if (total_distance(ds) < 10000) {
        p2_score++;
      }
      if (n) {
        areas[*n] += 1;
        if ((i == 0) || (j == 0) || (i == 449) || (j == 449)) {
          edges.insert(*n);
        }
      }
    }
  }

  short best = std::accumulate(
      areas.begin(), areas.end(), 0,
      [&edges](int best, auto next) { return nbest(best, next, edges); });

  std::cout << "largest area: " << best << std::endl;
  std::cout << "p2 score: " << p2_score << std::endl;
  return 0;
}
