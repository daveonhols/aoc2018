#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <array>
#include <set>
#include <optional>

using pos = std::pair<int, int>;

std::ostream& operator<<(std::ostream&o, pos& p) {
  o << "(" << p.first << "," << p.second << ")" << std::endl;
  return o;
}

enum class state_t {
  solid, empty, filled, flowing, escape, spring, floating, spreading
};

using ground_t = std::vector<std::vector<state_t>>;

struct world {
  ground_t ground;
  pos spring;
};

std::set<pos> process_line(std::set<pos> found, std::string line) {
  
  std::pair<int, int> x_r;
  std::pair<int, int> y_r;

  std::stringstream reader{line};

  char const_axis;
  char junk;
  int i1, i2, i3;

  reader >> const_axis;
  reader >> junk;

  reader >> i1;
  reader >> junk >> junk >> junk;
  reader >> i2;
  reader >> junk >> junk;
  reader >> i3;

  if (const_axis == 'x') {
    x_r = {i1, i1};
    y_r = {i2, i3};
  } else {
    x_r = {i2, i3};
    y_r = {i1, i1};
  }

  //  std::cout << "Read " << const_axis << ": " << i1 << ", " << i2 << ", " << i3 << std::endl;
  for (int x = x_r.first; x <= x_r.second; ++x) {
    for (int y = y_r.first; y <= y_r.second; ++y) {
      found.insert({y, x});
    }
  }
  return found;
};


world constrain(std::set<pos> clay, pos spout) {

  int min_x = 99999;
  int min_y = 99999;
  int max_x = 0;
  int max_y = 0;
  
  for (pos p : clay) {
    //std::cout << "P:: (" << p.first << "," << p.second << ")" << std::endl;
    if (p.first < min_x) {
      min_x = p.first;
    }
    if (p.first > max_x) {
      max_x = p.first;
      //std::cout << "max x" << max_x << std::endl;
    }
    if (p.second < min_y) {
      min_y = p.second;
    }
    if (p.second > max_y) {
      max_y = p.second;
    }
  }
  
  //padding
  min_x -= 1;
  min_y -= 1;
  max_x += 1;
  max_y += 1;

  int width = max_x - min_x;
  int height = max_y - min_y;

    
  ground_t ground;

  for (int x = min_x; x <= max_x; ++x) {
    std::vector<state_t> next{};
    ground.push_back(std::vector<state_t>{});
    for (int y = min_y; y <= max_y; ++y) {
      ground[x - min_x].push_back(clay.find({x,y}) == clay.end() ? state_t::empty : state_t::solid);
    }
  }  
  world w;
  w.ground = ground;
  w.spring = {0, spout.second - min_y};
  return w;
}


//  solid, empty, filled, flowing, escape
char draw_as(state_t s) {
  switch (s) {
  case state_t::empty:
    return '.';
    break;
  case state_t::solid:
    return '#';
    break;
  case state_t::flowing:
    return '|';
    break;
  case state_t::escape:
    return 'V';
    break;
  case state_t::filled:
    return '~';
    break;
  case state_t::spring:
    return '+';
    break;
  case state_t::floating:
    return 'F';
    break;
  case state_t::spreading:
    return '_';
    break;
  }
  return '?';
}

void render(ground_t g) {
  for (int i = 0; i < g.size(); ++i) {
    for (int j = 0; j < g[i].size(); ++j) {
      state_t s = g[i][j];
      char draw = draw_as(s);
      std::cout << draw;
    }
    std::cout << std::endl;
  }
}

//handle case where water is spreading horizontally
std::vector<pos> spread(pos from, ground_t& g) {
  std::vector<pos> springs;
  std::vector<pos> spread_to;
  spread_to.push_back(from);
  bool overflow = false;
  pos next = from;
  while (true) {
    next = {next.first, next.second - 1};
    if (g[next.first][next.second] == state_t::solid) {
      break; // hit a wall
    }
    spread_to.push_back(next);
    if (g[next.first + 1][next.second] == state_t::empty || g[next.first + 1][next.second] == state_t::flowing) {
      overflow = true;
      springs.push_back(next);
      break;
    }
  }
  next = from;
  while (true) {
    next = {next.first, next.second + 1};
    if (g[next.first][next.second] == state_t::solid) {
      break; // hit a wall
    }
    spread_to.push_back(next);
    if (g[next.first + 1][next.second] == state_t::empty || g[next.first + 1][next.second] == state_t::flowing) {
      overflow = true;
      springs.push_back(next);
      break;
    }
  }

  for (pos s : spread_to) {
    g[s.first][s.second] = overflow ? state_t::spreading : state_t::filled;
  }

  // dripping from here now
  for (pos d : springs) {
    g[d.first][d.second] = state_t::spring;
  }
  
  return springs;

}

// handle case where water is falling downwards
// return the place the drop landed
// drop may not "land" and may in fact fall out bottom -> optional
// modify ground to indicate where we flowed.
std::optional<pos> drip(pos source, std::deque<pos>& sources, ground_t& ground) {

  // flooded case
  if (ground[source.first][source.second] == state_t::filled) {
    sources.pop_front();
    sources.push_front({source.first - 1, source.second});
    ground[source.first - 1][source.second] = state_t::empty;
    ground[source.first - 2][source.second] = state_t::floating;    
    return std::optional<pos>{{source.first - 1, source.second}};
    }
   
  while(ground[source.first][source.second] == state_t::empty || ground[source.first][source.second] == state_t::flowing) {
    //render(ground);
    if (source.first > ground.size() - 2) {
      return std::optional<pos>{};
    }
    ground[source.first][source.second] = state_t::flowing;
    source = {source.first + 1, source.second};
  }
  if (ground[source.first][source.second] == state_t::spreading) {
    return std::optional<pos>{};
  }
  return std::optional<pos>{{source.first - 1, source.second}};  
}

void run(world start) {
  ground_t ground = start.ground;
  std::deque<pos> springs;
  springs.push_back(start.spring);
  //render(ground);
  while(springs.size() > 0) {
    std::optional<pos> land = drip(*springs.begin(), springs, ground);
    //render(ground);
    // doesn't land means it fell off the bottom or is flowing into another spring, finish with this one
    if(!land) {
      springs.pop_front();
      continue;
    }

    // we can create new springs when spreading
    std::vector<pos> new_springs = spread(*land, ground);
    for (pos new_spring : new_springs) {
      springs.push_back({new_spring.first + 1, new_spring.second});
      //render(ground);
      if (springs.size() > 500) {
	//render(ground);
	return;
      }
    }
    //render(ground);
  }

  int reached = 0;
  int held = 0;
  for (int i = 0; i < ground.size(); ++i) {
    for (int j = 0; j < ground[i].size(); ++j) {
      if (ground[i][j] == state_t::filled || ground[i][j] == state_t::flowing || ground[i][j] == state_t::spreading || ground[i][j] == state_t::spring) {
	reached +=1;
      }
      if (ground[i][j] == state_t::filled) {
	held +=1;
      }      
    }
  }
  //render(ground);
  std::cout << "P1 :: " << reached - 1 << std::endl;
  std::cout << "P2 :: " << held << std::endl;
  
  return;
}

int main() {
  std::cout << "hello d17" << std::endl;
    
  std::ifstream file("d17/input.txt");
  std::string line;
  std::set<pos> found;
  
  while (std::getline(file, line)) {
    found = process_line(found, line);
  }


  world w = constrain(found, {0, 500});

  run(w);
  
  return 0;
}
