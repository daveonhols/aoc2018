#include <fstream>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

std::pair<bool, bool> checkOne(std::string line) {
  std::map<char, int> counts{};
  for (auto c : line) {
    counts[c] += 1;
  }

  bool twice = false;
  bool thrice = false;

  for (auto entry : counts) {
    if (2 == entry.second) {
      twice = true;
    }
    if (3 == entry.second) {
      thrice = true;
    }
  }
  return std::pair<bool, bool>(twice, thrice);
}

void p1() {
  std::ifstream input("d2/input.txt");
  std::string line;

  int twos = 0;
  int threes = 0;

  while (std::getline(input, line)) {
    auto lineScore = checkOne(line);
    twos += lineScore.first ? 1 : 0;
    threes += lineScore.second ? 1 : 0;
  }
  std::cout << "checksum: " << twos * threes << std::endl;
}

bool offByOne(std::string a, std::string b) {
  int off = 0;
  for (int i = 0; i < a.length(); ++i) {
    off += a[i] == b[i] ? 0 : 1;
    if (off > 1) {
      return false;
    }
  }
  return 1 == off;
}

void p2() {

  std::ifstream input("d2/input.txt");
  std::string line;
  std::vector<std::string> lines{};
  while (std::getline(input, line)) {
    lines.push_back(line);
  }

  for (auto cmp = lines.begin(); cmp != lines.end(); ++cmp) {
    for (auto other = cmp; other != lines.end(); ++other) {
      if (offByOne(*cmp, *other)) {
        std::cout << "Found off by one:: " << std::endl;
        std::cout << *cmp << "\n" << *other << "\n";
        return;
      }
    }
  }
}

int main() {
  std::cout << "d2" << std::endl;
  p1();
  p2();
  return 0;
}
