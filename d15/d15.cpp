#include <fstream>
#include <deque>
#include <cmath>
#include <functional>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

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
  return std::pow(p1.first - p2.first, 2) + std::pow(p1.second - p2.second, 2); 
}

template <class A, class T>
std::map<pos, T> get_targets(
			     std::pair<pos, A> attacker,
			     std::map<pos, T> enemies,
			     std::map<pos, T> moved_enemies) {
  std::map<pos, T> targets{};
  //std::cout << "attacker (" << attacker.first.first << "," << attacker.first.second << ")" << std::endl;
  for (pos adj : adjacent(attacker.first)) {
    typename std::map<pos, T>::iterator target = enemies.find(adj);        
    if (target != enemies.end()) {
      targets.insert(*target);
    } else {
      target = moved_enemies.find(adj);
      if (target != moved_enemies.end()) {
	targets.insert(*target);
      }
    }
  }
  return targets;
}

template<class T> // target type, elf or goblin
std::pair<pos, T> get_best_target(std::map<pos, T> targets) {
  std::pair<pos, T> best = *targets.begin();
  for (std::pair<pos, T> target : targets) {
    if (target.second.hp() < best.second.hp() || ((target.second.hp() == best.second.hp()) && (target.first < best.first))) {
      best = target;
    }
  }
  return best;
}

template <class A, class T> // attacker and target
// optional because it might die
std::optional<T> attack(A attacker, T target) {
  bool alive = target.attacked(attacker.ap());
  //  std::cout << "target hp" << target.hp() << std::endl;
  if (!alive) {
    std::cout << " ** DEATH ** " << std::endl;
  }
  return alive ? std::optional<T>{target} : std::optional<T>{};
}

template <class A, class T>
void do_unit_attack(
		    std::pair<pos, A> attacker,
		    std::map<pos, T> targets,
		    std::map<pos, T>& other,
		    std::map<pos, T>& moved_others) {
    std::pair<pos, T> target = get_best_target(targets);
    //std::cout << "targetting (" << target.first.first << "," << target.first.second << ")" << std::endl;
    std::optional<T> attacked = attack(attacker.second, target.second);
    if (other.find(target.first) != other.end()) {
      other.erase(target.first);
      if (attacked) {
	other.insert({target.first, *attacked});
      }
    } else {
      moved_others.erase(target.first);
      if (attacked) {
	moved_others.insert({target.first, *attacked});
      }
    }
    return;
}

template <class T, class A> //target types / attacker type
std::multiset<pos> find_attack_pos(grid_t g,
			      std::map<pos, T> targets,
			      std::map<pos, T> moved_targets,
			      std::map<pos, A> friends,
			      std::map<pos, A> moved_friends) {

  std::multiset<pos> attack_pos{};

  for (std::pair<pos, T> target : targets) {
    for (pos possible_attack_pos : adjacent(target.first)) {
      if ((g[possible_attack_pos] == '.')
	  && targets.find(possible_attack_pos) == targets.end()
	  && moved_targets.find(possible_attack_pos) == moved_targets.end()
	  && friends.find(possible_attack_pos) == friends.end()
	  && moved_friends.find(possible_attack_pos) == moved_friends.end()) {
	attack_pos.insert(possible_attack_pos);
      }            
    }
  }

  for (std::pair<pos, T> target : moved_targets) {
    for (pos possible_attack_pos : adjacent(target.first)) {
      if ((g[possible_attack_pos] == '.')
	  && targets.find(possible_attack_pos) == targets.end()
	  && moved_targets.find(possible_attack_pos) == moved_targets.end()
	  && friends.find(possible_attack_pos) == friends.end()
	  && moved_friends.find(possible_attack_pos) == moved_friends.end()) {
	attack_pos.insert(possible_attack_pos);
      }            
    }
  }
  
  return attack_pos;

}

struct route {
  pos start;
  pos end;
  int length;
};

class route_estimate {
private:
  pos _start;
  pos _end;
  pos _curr;
  int _length;
  int _estimate;

public:
  route_estimate(pos start, pos end) { _start = start; _curr = start; _end = end; _length = 0;  _estimate = distance(start, end); };

  route as_route() { route r; r.start = _start; r.end = _end; r.length = _length; return r;  };
  
  std::vector<pos> adjacent() { return ::adjacent(_curr); };
  pos curr() { return _curr; };
  
  bool done() {
    return _curr.first == _end.first && _curr.second == _end.second;
  }
  int cost() {
    return _length + _estimate;
  };
  route_estimate step(pos next) {
    route_estimate result(*this);
    result._curr = next;
    ++result._length;
    result._estimate = distance(result._curr, result._end);
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
std::vector<pos> get_next_steps(std::vector<pos> candidates, grid_t g, const occupied<F, O>& occ, std::unordered_set<pos, pos_hash> been) {
  std::vector<pos> result;
  std::copy_if(
	       candidates.begin(),
	       candidates.end(),
	       std::back_inserter(result),
	       [&](pos p) { return (!occ.is_occupied(p)) && (g[p] == '.') &&  (been.find(p) == been.end()); });
  return result;
}

template <class F, class O>
std::optional<route> measure_astar(pos start, pos end, occupied<F, O> occ, grid_t g) {
  //  std::cout << "A* in " << std::endl;
  route r{};
  r.start = start;
  r.end = end;
  r.length = 0;

  std::function<int(route_estimate, route_estimate)> heuristic = [](route_estimate a, route_estimate b){ return a.cost() < b.cost(); };

  std::deque<route_estimate> open;
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
      //  std::cout << "A* failed (" << considered <<  std::endl;
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


template <class M, class O> // moving type, other type
std::pair<pos, M> do_unit_move(
			       grid_t g,
			       std::pair<pos, M> moving,
			       std::map<pos, M> friends,
			       std::map<pos, O> others,
			       std::map<pos, M> moved_friends,
			       std::map<pos, O> moved_others) {

  std::multiset<pos> in_range = find_attack_pos(g, others, moved_others, friends, moved_friends);
  if (in_range.size() == 0) {
    return moving;
  }
  
  occupied occ(friends, moved_friends, others, moved_others);
  
  std::vector<route> routes{};
  for (pos p : in_range) {
    std::optional<route> route = measure_astar(moving.first, p, occ, g);
    if(route) {
      //      std::cout << "Route :: (" << moving.first.first << "," << moving.first.second << ") => ("  << p.first << "," << p.second << ") [" << route->length << "]" << std::endl;
      routes.push_back(*route); 
    }
  }

  if (routes.size() == 0) {
    return moving;
  }
  
  std::sort(routes.begin(), routes.end(), [](route r1, route r2){ return r1.length < r2.length; });
  route best = *routes.begin();
  for (route candidate : routes) {
    if (candidate.length != best.length) {
      break;
    }
    if (candidate.end < best.end) {
      best = candidate;
    }
  }

  pos next_step = best_first_step(best, occ, g);
  
  return {next_step, moving.second};
}

template <class M, class O>
std::pair<pos, M> do_unit(
			  grid_t g,
			  std::map<pos, M>& move,
			  std::map<pos, O>& other,
			  std::map<pos, M> moved,
			  std::map<pos, O>& moved_others) {

  std::pair<pos, M> m = *move.begin();

  //  std::cout << "Start (" << m.first.first << "," << m.first.second << ")" << std::endl;
  
  // check if can attack
  std::map<pos, O> can_attack = get_targets(m, other, moved_others);
  if (can_attack.size() > 0) {
    //   std::cout << "attacking" << std::endl;
    do_unit_attack(m, can_attack, other, moved_others); 
  } else {
    // std::cout << "moving" << std::endl;
    m = do_unit_move(g, m, move, other, moved, moved_others);
    can_attack = get_targets(m, other, moved_others);
    if (can_attack.size() > 0) {
      //  std::cout << "attacking" << std::endl;
      do_unit_attack(m, can_attack, other, moved_others);
    }
  }
  //std::cout << "End   (" << m.first.first << "," << m.first.second << ")" << std::endl;
  return m;
}

enum class move_type { unknown, elf, goblin, round, elves_won, goblins_won  };

move_type next_move(elves_t curr_elves, elves_t next_elves, goblins_t curr_goblins, goblins_t next_goblins) {
  std::cout << "ce" << curr_elves.size() << "ne" << next_elves.size() << "cg" << curr_goblins.size() << "ng" << next_goblins.size() << std::endl;
  if (curr_elves.size() + next_elves.size() == 0) {
    return move_type::goblins_won;
  }
  if (curr_goblins.size() + next_goblins.size() == 0) {
    return move_type::elves_won;
  }
  if (curr_elves.size() == 0 && curr_goblins.size() == 0) {
    return move_type::round;
  }
  if (curr_elves.size() == 0) {
    return move_type::goblin;
  }
  if (curr_goblins.size() == 0) {
    return move_type::elf;
  }
  if (curr_elves.begin()->first < curr_goblins.begin()->first) {
    return move_type::elf;
  } else {
    return move_type::goblin;
  }
  return move_type::unknown;
}

void print(grid_t g, elves_t elf, goblins_t gob) {
  for (int i = 0; i < 35; ++i) {
    for (int j = 0; j < 35; ++j) {
      if(elf.find({i, j}) != elf.end()) {
	std::cout << "E";
	continue;
      }
      if(gob.find({i, j}) != gob.end()) {
	std::cout << "G";
	continue;
      }
      std::cout << g[{i,j}];
    }
    std::cout << std::endl;
  }
}

template <class T> // type of survivor (elf / goblin)
int score_survivors(std::map<pos, T> s, std::map<pos, T> m_s) {
  int score = 0;
  for (auto survior : s) {
    score += survior.second.hp();
  }
  for (auto moved_survivor : m_s) {
    score += moved_survivor.second.hp();
  }
  return score;
}

void run(grid_t g, elves_t elves, goblins_t goblins) {
  int gen = 1;
  int game_done = false;
  while (!game_done) {
    std::cout << "gen" << std::endl;
    print(g, elves, goblins);
    elves_t next_elves {};
    goblins_t next_goblins {};
    bool done_round = false;
    while (!done_round) {
      switch (next_move(elves, next_elves, goblins, next_goblins)) {
      case move_type::elf: {
	//	std::cout << "moving elf" << std::endl;
	std::pair<pos, elf> moved = do_unit(g, elves, goblins, next_elves, next_goblins);
	std::map<pos, elf>::iterator done = elves.begin();
	next_elves.insert(moved);
	elves.erase(done);
	break;
      }
      case move_type::goblin: {
	//std::cout << "moving goblin" << std::endl;
	std::pair<pos, goblin> moved = do_unit(g, goblins, elves, next_goblins, next_elves);
	std::map<pos, goblin>::iterator done = goblins.begin();
	next_goblins.insert(moved);
	goblins.erase(done);
	break;
      }
      case move_type::round:
	//std::cout << "done round" << std::endl;
	done_round = true;
	break;
      case move_type::elves_won:
	std::cout << "Elves won at " << gen << " with " << score_survivors(elves, next_elves) << std::endl;
	print(g, next_elves, goblins);
	game_done = true;
	done_round = true;
	break;
      case move_type::goblins_won:
	game_done = true;
	done_round = true;
	std::cout << "Goblins won" << gen << " with " << score_survivors(goblins, next_goblins) <<  std::endl;
	print(g, next_elves, goblins);
	break;
      case move_type::unknown:
      default:
	std::cout << "WTF" << std::endl;
      }
    }
    elves = next_elves;
    goblins = next_goblins;
    ++gen;
  }
}

int main() {
  std::cout << "hello d15" << std::endl;
  grid_t g{};
  elves_t elves{};
  goblins_t goblins{};

  std::ifstream input("d15/input.txt");
  std::string line;

  int row = 0;
  while (std::getline(input, line)) {
    for (int col = 0; col < line.length(); ++col) {
      switch (line[col]) {
      case '#':
      case '.':
	g[{row, col}] = line[col];
	break;
      case 'G':
	g[{row, col}] = '.';
	goblins.insert({{row, col}, goblin()});
	break;
      case 'E':
	g[{row, col}] = '.';
	elves.insert({{row, col}, elf()});
	break;
      }
    }
    ++row;
    std::cout << line << std::endl;
  }

  run(g, elves, goblins);
  
}
