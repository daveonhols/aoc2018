#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

int processOne(int freq, std::string line) {
  char sign = line[0];
  std::string parse = line.substr(1);
  std::stringstream ss{parse};
  int delta;
  ss >> delta;
  return sign == '+' ? freq + delta : freq - delta;
}

void p1() {
  std::ifstream input("d1/input.txt");
  std::string line;

  int freq = 0;
  while (std::getline(input, line)) {
    freq = processOne(freq, line);
  }
  std::cout << "done = " << freq << std::endl;
}

void p2() {
  int freq = 0;
  std::set<int> seen{};
  bool done = false;
  while (!done) {
    std::ifstream input("d1/input.txt");
    std::string line;
    while (std::getline(input, line)) {
      freq = processOne(freq, line);
      if (seen.count(freq) == 1) {
        std::cout << "second time @ " << freq << std::endl;
        done = true;
        break;
      }
      seen.insert(freq);
    }
  }
}

int main() {
  std::cout << "hello aoc" << std::endl;
  p1();
  p2();
  return 0;
}
