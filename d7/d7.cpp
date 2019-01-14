
#include "d7.hpp"
#include <fstream>

template <class P> void solve_one(P p) {
  std::ifstream input("d7/input.txt");
  std::string line;
  depends d{};
  while (std::getline(input, line)) {
    d.parse_add(line);
  }
  auto a = d.solve(p);
  std::cout << "Order:: ";
  for (auto c : a.order) {
    std::cout << c;
  }
  std::cout << ".  Duration:: " << a.duration << std::endl;
}

int main() {
  std::cout << "d7" << std::endl;
  solve_one(w_policy_p1{});
  solve_one(w_policy_p2{});
  return 0;
}
