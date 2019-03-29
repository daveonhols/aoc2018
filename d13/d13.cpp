#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>

using pos = std::pair<int, int>; // x, y
using track_map = std::unordered_map<pos, char>;

class cart {
private:
  char dir; // "U D L R"
  char turn; // "L S R"
public:
  void tick();
  
};

pos cart::tick(const pos& p, const char& track) {
  switch (track) {
  case '|':
    switch (dir) {
    'U':
      
    }
    break;
  }
}

    
  
using cart_pos = std::map<pos, cart>; 

int main() {
  std::cout << "hello d13" << std::endl;
  std::ifstream input("d13/test_input.txt");
  std::string line;
  while (std::getline(input, line)) {

    std::cout << line << std::endl;
  }
  
  
}
