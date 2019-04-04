#include <deque>
#include <cmath>
#include <functional>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <unordered_set>
#include <set>

using pos = std::pair<int, int>;

// required to support putting an x,y coordinate pair in an unordered map
struct pos_hash {
  std::size_t operator() (const pos &p) const {
    return std::hash<int>{}(p.first) ^ std::hash<int>{}(p.second);
  }
};

class elf {
private:
  int _ap;
  int _hp;
public:
  elf() : _ap(3), _hp(200) {};
  elf(int x) : _ap(x), _hp(200) {};
  int hp() { return this->_hp; };
  int ap() { return this->_ap; };
  bool attacked(int ap) { _hp -= ap ; return _hp > 0; };

};

class goblin {
private:
  int _ap;
  int _hp;
public:
  goblin() : _ap(3), _hp(200) {};
  int hp() { return this->_hp; };
  int ap() { return this->_ap; };
  bool attacked(int ap) { _hp -= ap; return _hp > 0; };
};

using grid_t = std::unordered_map<pos, char, pos_hash>;

using elves_t = std::map<pos, elf>;
using goblins_t = std::map<pos, goblin>;

std::vector<pos> adjacent(pos p) {
  std::vector<pos> result {};
  result.push_back({p.first - 1, p.second});
  result.push_back({p.first + 1, p.second});
  result.push_back({p.first, p.second - 1});
  result.push_back({p.first, p.second + 1});
  return result;
}

int distance(pos p1, pos p2) {
  return std::sqrt(std::pow(p1.first - p2.first, 2) + std::pow(p1.second - p2.second, 2)); 
}


struct route {
  pos start;
  pos end;
  int length;
  std::vector<pos> steps;
};

class route_estimate {
private:
  std::vector<pos> _steps;
  pos _start;
  pos _end;
  pos _curr;
  int _length;
  int _estimate;

public:
  route_estimate(pos start, pos end) { _start = start; _curr = start; _end = end; _length = 0;  _estimate = distance(start, end); };

  route as_route() { route r; r.start = _start; r.end = _end; r.length = _length; r.steps = _steps; return r;  };
  
  std::vector<pos> adjacent() { return ::adjacent(_curr); };
  pos curr() const { return _curr; };
  
  bool done() {
    return _curr.first == _end.first && _curr.second == _end.second;
  }
  int cost() const {
    return _length + _estimate;
  };
  route_estimate step(pos next) {
    route_estimate result(this->_start, this->_end);
    result._steps = std::move(this->_steps);
    result._curr = next;
    result._length = this->_length + 1;
    result._estimate = distance(result._curr, result._end);
    result._steps.push_back(next);
    return result;    
  };
};

template <class F, class O> // friends / others
class occupied {
public:
  occupied(std::map<pos, F> f, std::map<pos, F> f_m, std::map<pos, O> o, std::map<pos, O> o_m) { this->f = f; this->f_m = f_m; this->o = o; this->o_m = o_m; };
  bool is_occupied(pos p) const { return contains(f, p) || contains(f_m, p) || contains(o, p) || contains(o_m, p); };

private:
  template <class Y>
  bool contains(std::map<pos, Y> check, pos p) const { return !(check.find(p) == check.end()); };
  std::map<pos, F> f;
  std::map<pos, F> f_m;
  std::map<pos, O> o;
  std::map<pos, O> o_m;
  
};

template <class F, class O>
std::vector<pos> get_next_steps(std::vector<pos> candidates, grid_t& g, const occupied<F, O>& occ, std::unordered_set<pos, pos_hash>& been) {
  std::vector<pos> result;
  std::copy_if(
	       candidates.begin(),
	       candidates.end(),
	       std::back_inserter(result),
	       [&](pos p) { return (been.find(p) == been.end()) && (g[p] == '.') && (!occ.is_occupied(p)); });
  return result;
}


// we need to find the reading order first best step towards the target.
// so we just try all 4 candidates and if they are same length as we found
template <class F, class O> // friends / others
pos best_first_step(route r, occupied<F, O> occ, grid_t g) {

  int exp_len = r.length - 1;
  std::set<pos> candidates{};
  for (pos adj : adjacent(r.start)) {
    if (occ.is_occupied(adj) || g[adj] == '#') {
      continue;
    }
    std::optional<route> candidate_route = measure_astar(adj, r.end, occ, g);
    if (candidate_route) {
      if (candidate_route->length == exp_len) {
	candidates.insert(adj);
      }
    }
  }
  return *candidates.begin();
}

template <class F, class O> // friends / others
pos best_first_step_map(route r, occupied<F, O> occ, grid_t g) {

  int exp_len = r.length - 1;
  std::set<pos> candidates{};
  for (pos adj : adjacent(r.start)) {
    if (occ.is_occupied(adj) || g[adj] == '#') {
      continue;
    }
    std::optional<route> candidate_route = measure_astar_map(adj, r.end, occ, g);
    if (candidate_route) {
      if (candidate_route->length == exp_len) {
	candidates.insert(adj);
      }
    }
  }
  return *candidates.begin();
}





template <class F, class O>
std::optional<route> measure_astar(pos start, pos end, occupied<F, O> occ, grid_t g) {
  //  std::cout << "A* in " << std::endl;
  route r{};
  r.start = start;
  r.end = end;
  r.length = 0;
  
  auto heuristic = [](route_estimate a, route_estimate b){ return a.cost() < b.cost(); };

  std::deque<route_estimate> open{};
  
  std::unordered_set<pos, pos_hash> been;
  route_estimate next = route_estimate(r.start, r.end);
  been.insert(r.start);

  int considered = 0;
  while (!next.done()) {
    std::vector<pos> next_steps = get_next_steps(next.adjacent(), g, occ, been);
    for (pos step : next_steps) {
      route_estimate one_step = next.step(step);
      if(std::find_if(open.begin(), open.end(), [&](route_estimate r){ return one_step.curr() == r.curr();  }) == open.end()) {
	open.push_back(one_step);
      }
    }
    
    std::sort(open.begin(), open.end(), heuristic);
    
    if (open.size() == 0) {
      return std::optional<route>{};
    }
    
    next = open.front();
    open.pop_front();
    been.insert(next.curr());
    ++considered;
  }

  //  std::cout << "A* out (" << considered <<  std::endl;
  return std::optional<route>{next.as_route()};
}

template <class F, class O>
std::optional<route> measure_astar_map(pos start, pos end, occupied<F, O> occ, grid_t g) {
  //  std::cout << "A* in " << std::endl;
  route r{};
  r.start = start;
  r.end = end;
  r.length = 0;
  
  auto heuristic = [](const route_estimate& a, const route_estimate& b){ return a.cost() < b.cost(); };

  std::multiset<route_estimate, std::function<bool(route_estimate, route_estimate)>> open(heuristic);
  
  std::unordered_set<pos, pos_hash> been;
  route_estimate next = route_estimate(r.start, r.end);
  been.insert(r.start);

  int considered = 0;
  while (!next.done()) {
    std::vector<pos> next_steps = get_next_steps(next.adjacent(), g, occ, been);
    for (pos step : next_steps) {
      route_estimate one_step = next.step(step);
      if(std::find_if(open.begin(), open.end(), [&](const route_estimate& r){ return one_step.curr() == r.curr();  }) == open.end()) {
	open.insert(one_step);
      }
    }
    
    //std::sort(open.begin(), open.end(), heuristic);
    
    if (open.size() == 0) {
      return std::optional<route>{};
    }
    
    next = *open.begin();
    open.erase(open.begin());
    been.insert(next.curr());
    ++considered;
  }

  //  std::cout << "A* out (" << considered <<  std::endl;
  return std::optional<route>{next.as_route()};
}

template <class F, class O>
void printr(grid_t g, F f, O o, F m_f, O m_o, route r) {
  for (int i = 0; i < 32; ++i) {
    for (int j = 0; j < 32; ++j) {
      if (std::find(r.steps.begin(), r.steps.end(), std::pair(i, j)) != r.steps.end()) {
	std::cout << "+";
	continue;
      }
      if(f.find({i, j}) != f.end()) {
	std::cout << "F";
	continue;
      }
      if(m_f.find({i, j}) != m_f.end()) {
	std::cout << "f";
	continue;
      }
      if(o.find({i, j}) != o.end()) {
	std::cout << "O";
	continue;
      }
      if(m_o.find({i, j}) != m_o.end()) {
	std::cout << "o";
	continue;
      }
	    
      std::cout << g[{i,j}];
    }
    std::cout << std::endl;
  }
}


void print(grid_t g, elves_t elf, goblins_t gob, elves_t moved_elf, goblins_t moved_gob) {
  printr(g, elf, gob, moved_elf, moved_gob, route{});
}
