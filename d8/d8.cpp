#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

std::string getInput();
std::string getTestInput();

// I thought visitor pattern would be useful to abstract over part 1 and part 2,
// it was not.
struct visitor {
  int num;
  void visit(int meta) { num += meta; }
};

void do_node(std::vector<int>::iterator &walker, visitor &v, int &score) {
  int num_children = *walker++;
  int num_meta = *walker++;
  std::vector<int> child_scores{};
  std::vector<int> meta_refs{};
  for (int i = 0; i < num_children; ++i) {
    int node_score = 0;
    do_node(walker, v, node_score);
    child_scores.push_back(node_score);
  }
  for (int j = 0; j < num_meta; ++j) {
    meta_refs.push_back(*walker);
    v.visit(*walker++);
  }
  if (num_children > 0) {
    for (int meta_ref : meta_refs) {
      if (meta_ref > 0 && meta_ref <= child_scores.size())
        score += child_scores[meta_ref - 1];
    }
  } else {
    for (int meta_val : meta_refs) {
      score += meta_val;
    }
  }
  return;
}

void walk(std::vector<int> input) {
  visitor v{};
  int score = 0;
  auto walker = input.begin();
  while (walker != input.end()) {
    do_node(walker, v, score);
  }
  std::cout << "p1 :: meta sum::  " << v.num << std::endl;
  std::cout << "p2 :: root score: " << score << std::endl;
}

int main() {
  std::cout << "d8" << std::endl;
  std::istringstream read{getInput()};
  std::vector<int> input{};
  std::copy(std::istream_iterator<int>(read), std::istream_iterator<int>(),
            std::back_inserter(input));
  walk(input);
};

std::string getTestInput() { return "2 3 0 3 10 11 12 1 1 0 1 99 2 1 1 2"; }

std::string getInput() {
  std::ifstream in("d8/input.txt");
  std::string line;
  std::getline(in, line);
  return line;
}
