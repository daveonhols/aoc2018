#include <iostream>
#include <unordered_map>
#include <fstream>
#include <queue>

using pos = std::pair<int, int>;

pos east(pos p) {
  return pos{p.first + 1, p.second};
}

pos west(pos p) {
  return pos{p.first - 1, p.second};
}

pos north(pos p) {
  return pos{p.first, p.second - 1};
}

pos south(pos p) {
  return pos{p.first, p.second + 1};
}

// required to support putting an x,y coordinate pair in an unordered map
struct pos_hash {
  std::size_t operator() (const pos &p) const {
    return std::hash<int>{}(p.first) ^ std::hash<int>{}(p.second);
  }
};

// is there a room to the north / south / east / west of this room?
struct room {
  room() : north(false), south(false), east(false), west(false), seen(false) {};
  bool north;
  bool south;
  bool east;
  bool west;
  bool seen; // for BFS walk
  int distance;
};

using grid = std::unordered_map<pos, room, pos_hash>;

bool walk(const std::string& path, int& idx, grid& map, pos curr, int& depth) {
  while (idx < path.length()) {
    pos next(curr);
    //  std::cout << "AT :: " << path[idx] << std::endl;
    switch (path[idx++]) {    
    case 'E':
      map[curr].east = true;
      next = east(curr);
      map[next].west = true;
      curr = next;
      //walk(path, idx, map, next, depth);    
      break;
    case 'W':
      map[curr].west = true;
      next = west(curr);
      map[next].east = true;
      curr = next;
      //walk(path, idx, map, next, depth);
      break;
    case 'N':
      map[curr].north = true;
      next = north(curr);
      map[next].south = true;
      curr = next;
      //walk(path, idx, map, next, depth);
      break;
    case 'S':
      map[curr].south = true;
      next = south(curr);
      map[next].north = true;
      curr = next;
      break;
    case '(':
      depth += 1;
      //bool sub_chunks = walk(path, idx, map, curr, depth); // walk first option
      while (walk(path, idx, map, curr, depth)) {}
      break;
    case ')':    
      depth -= 1;
      return false;
      break;
    case '|':
      return true;
      break;
    case '$':
      std::cout << "Found $$" << std::endl;
      return false;
      break;
    default:
      throw std::logic_error("impossible parse");
      break;
    }
  }
  return false;
}

// BFS traversal 
int find_furthest(grid& map) {
  std::queue<pos> working{};
  working.push({0,0});
  map.at({0,0}).seen = true;
  map.at({0,0}).distance = 0;

  pos next_pos = working.front();
  while (working.size() > 0) {
    next_pos = working.front();
    working.pop();
    room next_room = map.at(next_pos);
    if (next_room.north && !map.at(north(next_pos)).seen) {
      working.push(north(next_pos));
      map.at(north(next_pos)).seen = true;
      map.at(north(next_pos)).distance = map.at(next_pos).distance + 1; 
    }
    if (next_room.south && !map.at(south(next_pos)).seen) {
      working.push(south(next_pos));
      map.at(south(next_pos)).seen = true;
      map.at(south(next_pos)).distance = map.at(next_pos).distance + 1;
    }
    if (next_room.east && !map.at(east(next_pos)).seen) {
      working.push(east(next_pos));
      map.at(east(next_pos)).seen = true;
      map.at(east(next_pos)).distance = map.at(next_pos).distance + 1; 
    }
    if (next_room.west && !map.at(west(next_pos)).seen) {
      working.push(west(next_pos));
      map.at(west(next_pos)).seen = true;
      map.at(west(next_pos)).distance = map.at(next_pos).distance + 1; 
    }
  }
    
  return map.at(next_pos).distance;
}


// take care with draw as it uses chars a-z for path but will go into unprintable in case paths are too long
// only really works for small grids eg 
//input = "^ENNWSWW(NEWS|)SSSEEN(WNSE|)EE(SWEN|)NNN$";
void draw(const grid& map, int size) {

  for (int i = -1 * size; i < size; ++i) {
    std::string above;
    std::string row;
    std::string below;
    
    for (int j = -1 * size; j < size; ++j) {
      bool start = (i == 0 && j == 0);
      auto i_room = map.find(pos{j,i}); 
      room r = (i_room == map.end()) ? room() : i_room->second;
      
      above += '#';
      row += r.west ? '|' : '#';
      below += '#';
      
      above += r.north ? '-' : '#';
      row += start ? 'X' : i_room == map.end() ? '#' : r.distance + 'a';
      below += r.south ? '-' : '#';

      above += '#';
      row += r.east ? '|' : '#';
      below += '#';      
    }
    
    std::cout << above << std::endl;
    std::cout << row << std::endl;
    std::cout << below << std::endl;
  }  
}

int main() {
  std::cout << "hello d20" << std::endl;
  std::ifstream file("d20/input.txt");
    
  std::string input;
  std::getline(file, input);

  int idx = 1;
  int depth =0;
  grid map{};
  
  pos start{0,0};
  map.insert({start, room()});
  walk(input, idx, map, start, depth);
  std::cout << "P1 :: " << find_furthest(map) << std::endl;

  int over_1k = 0;
  for (auto room_at : map) {
    if(room_at.second.distance >= 1000) {
      over_1k++;
    }
  }
  std::cout << "P2 :: " << over_1k << std::endl;

  
  return 0;

  
  
  std::cout << "found :: " << map.size() << std::endl;

}



