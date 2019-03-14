#include <iostream>
#include <deque>
#include <list>
#include <vector>
#include <algorithm>

class circle {

private:
  std::list<int>::iterator _curr;
  std::list<int> _circle;
public:
  int place(int);
  friend std::ostream& operator<<(std::ostream& o, const circle& c) {
    std::for_each(c._circle.begin(), c._circle.end(), [&o] (int e) { o << e << " ";});
    o << "(" << *c._curr << ")";
    o << std::endl;
    return o;
  };  
}; 

int circle::place(int marble) {
  if (_circle.size() == 0) {
    _circle.push_back(marble);
    _curr = _circle.begin();
    return 0;
  }
  if (0 == marble % 23) {
    int score = marble;
    for (int i = 0; i < 7; ++i) {
      if (_curr == _circle.begin()) {
	_curr = _circle.end()--;
      }
      --_curr;
    }
    score += *_curr;
    _curr = _circle.erase(_curr);
    return score;
  }
  for (int i = 0; i < 2; ++i) {
    ++_curr;
    if (_curr == _circle.end()) {
      _curr = _circle.begin();
    }    
  }
  _curr = _circle.insert(_curr, marble);
  return 0;
};

long game(int players, int marbles) {
  circle c;
  std::vector<long> scores(players);
  int player = 0;  
  for (int m = 0; m <= marbles; ++m) {
    scores[player++] += c.place(m);
    if(player == players) {
      player = 0;
    }
    // std::cout << c;
  }
  return *std::max_element(scores.begin(), scores.end());
}

int main() {
  //game(10, 1618);
  std::cout << "P1 = " << game(478, 71240) << std::endl;
  std::cout << "P2 = " << game(478, 7124000) << std::endl;
}
