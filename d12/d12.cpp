#include <fstream>
#include <iostream>
#include <deque>
#include <array>
#include <unordered_map>


//using rule_key = std::tuple<bool, bool, bool, bool, bool>;
using rule_key = std::string;
using rule_map = std::unordered_map<rule_key, bool>;

using pots = std::deque<char>;

int main() {
  std::cout << "hello d12" << std::endl;
  pots p;
  rule_map rules;
  //  std::unordered_map<std::tuple<int, int>, bool> b{};
  std::string start = "#...##.#...#..#.#####.##.#..###.#.#.###....#...#...####.#....##..##..#..#..#..#.#..##.####.#.#.###";
  for ( auto& c : start) {
    p.push_back(c);
  }

  std::string line;
  std::ifstream file("d12/input.txt");
  while (std::getline(file, line)) {
    //    std::cout << line;
    auto k = line.substr(0, 5);
    std::cout << k << "!!" << std::endl;
    rules.insert({k, line[9] == '#'});
  }

}
