#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>

// required to support putting an x,y coordinate pair in an unordered map
struct pair_hash {
  std::size_t operator() (const std::pair<int, int> &p) const {
    return std::hash<int>{}(p.first) ^ std::hash<int>{}(p.second);
  }
};

using pos = std::pair<int, int>; // x, y

// derive a new point based on cartesian movement directions
pos left(const pos& p) { return {p.first - 1, p.second}; };
pos right(const pos& p) { return {p.first + 1, p.second}; };
pos up(const pos& p) { return {p.first, p.second - 1}; };
pos down(const pos& p) { return {p.first, p.second + 1}; };

// map from location to a track identifying char, i.e | - + \ / etc
using track_map = std::unordered_map<pos, char, pair_hash>;


// encapsulate a cart with direction of movement and intersection turn state
class cart {
private:
  char dir; // "U D L R"

  // how to behave at next intersection
  char turn; // "L S R"
public:
  cart(char dir);
  pos tick(const pos& p, const char& t);
  
};


// track positions of carts.
// use a sorted structure to ensure we move them in the right order (top left before bottom right).
using cart_pos = std::map<pos, cart>; 

cart::cart(char d){
  dir = d;
  turn = 'L'; // all carts turn left on first intersection
}


// dreadful boiler plate for deciding where a cart goes next
// based on current position, track at that position and direction it is facing
// also based on intersection turning rule and state (turn left at first intersection, straight on at second, right at third etc).
pos cart::tick(const pos& p, const char& track) {
  switch (track) {
  case '|':
    switch (dir) {
    case 'U':
      return up(p);
      break;      
    case 'D':
      return down(p);
      break;
    default:
      throw std::logic_error("impossible move");
      break;      
    }
    break;
  case '-':
    switch(dir) {
    case 'L':
      return left(p);
      break;
    case 'R':
      return right(p);
      break;
    default:
      throw std::logic_error("impossible move");
      break;
    }    
    break;

    
  case '\\':
    switch (dir) {
    case 'U':
      dir = 'L';
      return left(p);
      break;
    case 'D':
      dir = 'R';
      return right(p);
      break;
    case 'L':
      dir = 'U';
      return up(p);
      break;
    case 'R':
      dir = 'D';
      return down(p);
      break;
    default:
      throw std::logic_error("impossible turn");
      break;
    }
    break;

  case '/':
    switch (dir) {
    case 'U':
      dir = 'R';
      return right(p);
      break;
    case 'D':
      dir = 'L';
      return left(p);
      break;
    case 'L':
      dir = 'D';
      return down(p);
      break;
    case 'R':
      dir = 'U';
      return up(p);
      break;
    default:
      throw std::logic_error("impossible turn");
      break;
    }
    break;

  case '+':
    switch (turn) {
    case 'L':
      turn = 'S';
      switch (dir) {
      case 'U':
	dir = 'L';
	return left(p);
	break;
      case 'D':
	dir = 'R';
	return right(p);
	break;
      case 'L':
	dir = 'D';
	return down(p);
	break;
      case 'R':
	dir = 'U';
	return up(p);
	break;
      }
      break;
    case 'S':
      turn = 'R';
      switch (dir) {
      case 'U':
	return up(p);
	break;
      case 'D':       
	return down(p);
	break;
      case 'L':
	return left(p);
	break;
      case 'R':
	return right(p);
	break;
      }      
      break;
    case 'R':
      turn = 'L';
      switch (dir) {
      case 'U':
	dir = 'R';
	return right(p);
	break;
      case 'D':
	dir = 'L';
	return left(p);
	break;
      case 'L':
	dir = 'U';
	return up(p);
	break;
      case 'R':
	dir = 'D';
	return down(p);
	break;
      }            
      break;
    }
    break;

  default:
    throw std::logic_error("impossible track");
    break;
    
  }
  throw std::logic_error("impossible tick");
}




void run(track_map& tracks, cart_pos carts) {  
  bool p1 = false;
  while (carts.size() > 1) {    
    cart_pos next{};
    int iter = 0;
    while (carts.size() > 0) {
      std::pair<pos, cart> cp = *(carts.begin());
      pos moved_to = cp.second.tick(cp.first, tracks[cp.first]);
      bool collision = false;
      if (carts.find(moved_to) != carts.end()) {
	if (!p1) {
	  std::cout << "[P1] :: ";
	  p1=true;
	}
	std::cout << "Collision with carts (" << moved_to.first << "," << moved_to.second << ")" << std::endl;
	carts.erase(moved_to);
	collision = true;
      }
      if (next.find(moved_to) != next.end()) {
	if (!p1) {
	  std::cout << "[P1] :: ";
	  p1=true;
	}	
	std::cout << "Collision with next (" << moved_to.first << "," << moved_to.second << ")" << std::endl;
	next.erase(moved_to);
	collision = true;
      }
      if (!collision) {
	next.insert({moved_to, cp.second}); 
      }
      carts.erase(cp.first);
    }
    carts = std::move(next);// go around for next iter
  }
  std::pair<pos, cart> cp = *(carts.begin());
  std::cout << "[P2] :: Last cart @ " << cp.first.first << "," << cp.first.second << std::endl;
}


int  main() {
  std::cout << "hello d13" << std::endl;

  track_map tracks;
  cart_pos carts;
  
  std::ifstream input("d13/input.txt");
  std::string line;
  int row = 0;
  while (std::getline(input, line)) {
    for (int col = 0; col < line.length(); ++col) {
      switch (line[col]) {
      case '/':
      case '\\':
      case '-':
      case '|':
      case '+':
	tracks[{col, row}] = line[col];
	break;
      case '>':
	tracks[{col, row}] = '-';
	carts.insert({{col, row}, cart('R')});
	break;	
      case '<':
	tracks[{col, row}] = '-';
	carts.insert({{col, row}, cart('L')});
	break;	
      case 'v':
	tracks[{col, row}] = '|';
	carts.insert({{col, row}, cart('D')});
	break;
      case '^':
	tracks[{col, row}] = '|';
	carts.insert({{col, row}, cart('U')});
	break;	
      }      
    }
    ++row;
  }
  run(tracks, carts);  
}
