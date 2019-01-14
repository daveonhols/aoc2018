#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>

bool match(char a, char b) { return 32 == abs(b - a); }

std::string reduce(std::string input) {

  for (int i = 0; i < input.length() - 1; ++i) {
    if (match(input[i], input[i + 1])) {
      return reduce(input.substr(0, i) + input.substr(i + 2));
    }
  }
  return input;
}

void p1(std::string line) {
  std::string reduced = reduce(line);
  std::cout << reduced.length() << std::endl;
}

void p2(std::string line) {
  std::string letters = "abcdefghijklmnopqrstuvwxyz";
  std::map<char, std::string> candidates{};

  for (auto letter : letters) {
    candidates[letter] = "";
    std::copy_if(
        line.begin(), line.end(), std::back_inserter(candidates[letter]),
        [letter](char c) { return !((c == letter) or c == (letter - 32)); });
  }

  int least = 9901;

  for (auto candidate : candidates) {
    std::string reduced = reduce(candidate.second);
    int reduced_len = reduced.length();
    std::cout << candidate.first << ": reduced to" << reduced_len << std::endl;
    if (reduced_len < least) {
      least = reduced_len;
    }
  }
  std::cout << "Most reduced length: " << least << std::endl;
}

int main() {
  std::cout << "d5 ... " << std::endl;
  std::ifstream input("d5/input.txt");
  std::string line;
  std::getline(input, line);
  p1(line);
  p2(line);
}
