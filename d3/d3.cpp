#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <vector>

// <number_of_claims, first claiming id>
using n_claim = std::pair<int, std::string>;
using fabric_t = std::vector<std::vector<n_claim>>;

struct claim {
  int x, y, w, h;
  std::string id;
};

claim parseOne(std::string line) {
  size_t at = line.find("@");
  size_t comma = line.find(",");
  size_t colon = line.find(":");
  size_t x = line.find("x");
  claim c{};
  std::stringstream sx(line.substr(at + 2, comma - at - 2));
  sx >> c.x;
  std::stringstream sy(line.substr(comma + 1, colon - comma - 1));
  sy >> c.y;
  std::stringstream sw(line.substr(colon + 2, x - colon - 2));
  sw >> c.w;
  std::stringstream sh(line.substr(x + 1));
  sh >> c.h;
  c.id = line.substr(0, at);
  return c;
}

int applyClaim(fabric_t &fabric, std::set<std::string> &clean, const claim &c,
               int overlaps) {
  bool trampled = false;
  for (int i = c.x; i < c.w + c.x; ++i) {
    for (int j = c.y; j < c.h + c.y; ++j) {
      if (fabric[i][j].first == 1) {
        ++overlaps;
      }
      if (fabric[i][j].first > 0) {
        trampled = true;
        clean.erase(fabric[i][j].second);
      }
      if (fabric[i][j].first == 0) {
        fabric[i][j].second = c.id;
      }
      fabric[i][j].first += 1;
    }
  }
  if (!trampled) {
    clean.insert(c.id);
  }
  return overlaps;
}

void run() {
  std::ifstream input("d3/input.txt");
  fabric_t fabric{};
  fabric.resize(3000);
  for (auto &v : fabric) {
    v.resize(3000);
  }

  std::set<std::string> clear{};
  std::string line;
  int overlaps = 0;
  while (std::getline(input, line)) {
    claim c = parseOne(line);
    overlaps = applyClaim(fabric, clear, c, overlaps);
  }
  std::cout << overlaps << std::endl;
  std::cout << "clear:" << clear.size();
  for (auto c : clear) {
    std::cout << c << std::endl;
  }
}

int main() {
  std::cout << "d3" << std::endl;
  run();
  std::cout << "done" << std::endl;
}
