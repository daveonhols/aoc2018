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
  std::vector<std::pair<pos, T>> to_sort{};
  for (std::pair<pos, T> t : targets) {
    to_sort.push_back(t);
  }

  std::sort(to_sort.begin(), to_sort.end(), [](auto a, auto b){ return a.second.hp() < b.second.hp(); });

  int best = to_sort.begin()->second.hp();

  std::map<pos, T> tie_break_sort{};
  
  for (std::pair<pos, T> t : to_sort) {
    if (t.second.hp() > best) {
      break;
    }
    tie_break_sort.insert(t);
  }

  return *tie_break_sort.begin();
  
}

template <class A, class T> // attacker and target
// optional because it might die
std::optional<T> attack(A attacker, T target) {
  bool alive = target.attacked(attacker.ap());
  //  std::cout << "target hp" << target.hp() << std::endl;
  if (!alive) {
    //std::cout << " ** DEATH ** " << std::endl;
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
    //std::cout << " targetting (" << target.first.first << "," << target.first.second << ")";
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
    /*
    //    std::xxxxsort(open.begin(), open.end(), heuristic);
    */
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

  //std::cout << " from (" << moving.first.first << "," << moving.first.second << ")";
  std::multiset<pos> in_range = find_attack_pos(g, others, moved_others, friends, moved_friends);
  if (in_range.size() == 0) {
    return moving;
  }
  //  std::cout << std::endl << " In Range " << std::endl;
  for (auto inr : in_range) {
    //  std::cout << "(" << inr.first << "," << inr.second << ")" << std::endl;
  }
  
  occupied occ(friends, moved_friends, others, moved_others);
  
  std::vector<route> routes{};
  for (pos p : in_range) {
    // std::cout << "Route :: " << std::endl;
    std::optional<route> route = measure_astar(moving.first, p, occ, g);
    if(route) {
      routes.push_back(*route);
      //std::cout << "(" << (*route).end.first << "," << (*route).end.second << "). L=" << (*route).length << std::endl;
      for (pos s : route->steps) {
	//std::cout << "   - (" << s.first << "," << s.second << ")" << std::endl;
      }
      if (moving.first == std::pair{25,5} && route->end == std::pair{20,22}) {
	//printr(g, friends, others, moved_friends, moved_others, *route);
	//throw std::logic_error("why not ");
      }      
    }
  }

  if (routes.size() == 0) {
    //std::cout << " but going nowhere ";
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
  //std::cout << " towards (" << best.end.first << ", " << best.end.second << ")";
  pos next_step = best_first_step(best, occ, g);
  //std::cout << " via (" << next_step.first << ", " << next_step.second << ")";

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

  //std::cout << "Unit (" << m.first.first << "," << m.first.second << ")" << std::endl;
  
  // check if can attack
  std::map<pos, O> can_attack = get_targets(m, other, moved_others);
  if (can_attack.size() > 0) {
    //std::cout << " attacking immediately ";
    do_unit_attack(m, can_attack, other, moved_others); 
  } else {
    //std::cout << " moving ";
    m = do_unit_move(g, m, move, other, moved, moved_others);
    can_attack = get_targets(m, other, moved_others);
    if (can_attack.size() > 0) {
      //std::cout << " then attacking ";
      do_unit_attack(m, can_attack, other, moved_others);
    }
  }
  //std::cout << ".  Fin." << std::endl;
  return m;
}

enum class move_type { unknown, elf, goblin, round, elves_won, goblins_won, retry  };

using state_t = std::function<move_type(elves_t, elves_t, goblins_t, goblins_t)>;

move_type next_move_p1(elves_t curr_elves, elves_t next_elves, goblins_t curr_goblins, goblins_t next_goblins) {
  if (curr_elves.size() == 0 && curr_goblins.size() == 0) {
    return move_type::round;
  }  
  if (curr_elves.size() + next_elves.size() == 0) {
    return move_type::goblins_won;
  }
  if (curr_goblins.size() + next_goblins.size() == 0) {
    return move_type::elves_won;
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


move_type next_move_p2(elves_t curr_elves, elves_t next_elves, goblins_t curr_goblins, goblins_t next_goblins) {
  if (curr_elves.size() + next_elves.size() < 10) {
    std::cout << "Elves lost a life ... retrying ..." << std::endl;
    return move_type::retry;
  }
  return next_move_p1(curr_elves, next_elves, curr_goblins, next_goblins);
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

template <class T> // type of survivor (elf / goblin)
int score_survivors(std::map<pos, T> s, std::map<pos, T> m_s) {
  int score = 0;
  for (auto survior : s) {
    std::cout << survior.second.hp() << ", ";
    score += survior.second.hp();
  }
  for (auto moved_survivor : m_s) {
    score += moved_survivor.second.hp();
    std::cout << moved_survivor.second.hp() << ", ";
  }
  return score;
}


bool run(grid_t g, elves_t elves, goblins_t goblins, state_t state_manager) {
  int gen = 0;
  int game_done = false;
  while (!game_done && gen < 200) {
    //std::cout << "gen" << std::endl;
    elves_t next_elves {};
    goblins_t next_goblins {};
    bool done_round = false;
    while (!done_round) {
      //std::cout << "next ... " << std::endl;
      switch (state_manager(elves, next_elves, goblins, next_goblins)) {
      case move_type::elf: {
	std::pair<pos, elf> moved = do_unit(g, elves, goblins, next_elves, next_goblins);
	std::map<pos, elf>::iterator done = elves.begin();
	next_elves.insert(moved);
	elves.erase(done);
	break;
      }
      case move_type::goblin: {
	std::pair<pos, goblin> moved = do_unit(g, goblins, elves, next_goblins, next_elves);
	std::map<pos, goblin>::iterator done = goblins.begin();
	next_goblins.insert(moved);
	goblins.erase(done);
	break;
      }
      case move_type::round:
	done_round = true;
	break;
      case move_type::elves_won:
	std::cout << "Elves won at " << gen << " with " << score_survivors(elves, next_elves) << std::endl;
	print(g, elves, goblins, next_elves, next_goblins);
	game_done = true;
	done_round = true;
	break;
      case move_type::goblins_won:
	game_done = true;
	done_round = true;
	//std::cout << "Goblins won at " << gen << " with " << score_survivors(goblins, next_goblins) <<  std::endl;
	//print(g, elves, goblins, next_elves, next_goblins);
	break;
      case move_type::retry:
	return false;
      case move_type::unknown:
      default:
	std::cout << "WTF" << std::endl;
      }
    }
    elves = next_elves;
    goblins = next_goblins;
    ++gen;
    }
    return true;
}



  

using elf_builder = std::function<elf(void)>;

bool run_part(elf_builder eb, state_t t) {

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
      case 'g':
	g[{row, col}] = '.';
	goblins.insert({{row, col}, goblin()});
	break;
      case 'E':
      case 'e':
	g[{row, col}] = '.';
	elves.insert({{row, col}, eb()});
	break;
      }
    }
    ++row;
    std::cout << line << std::endl;
  }
  return run(g, elves, goblins, t);
  
}



void run_p1() {
  run_part([](){ return elf();}, next_move_p1 );
}

void run_p2() {
  std::vector<int> hp {};
  for (int i = 3; i < 190; ++i) {
    hp.push_back(i);
  }

  auto pred = [](int x){ return x > 102; };
  auto p2_pred = [] (int x) {
    std::cout << "Trying elves with HP = " << x << std::endl;
    bool won = run_part([=](){ return elf(x); }, next_move_p2);
    std::cout << (won ? "Elves won without loss" : "Elves suffered a loss") << std::endl;
    return won;
  };
  while (hp.size() > 1) {
    int mid = (2 == hp.size()) ? 0 : hp.size() / 2;
    std::cout << "trying " << hp[mid];    
    bool r = p2_pred(hp[mid]);
    std::cout << " = " << r;
    hp.erase(r ? hp.begin() + mid + 1 : hp.begin(), r ? hp.end() : hp.begin() + mid + 1);
    std::cout << "new range :: " << hp[0] << ":" << hp[hp.size()-1] << std::endl;
  }
  std::cout << "HP == " << hp[0] << std::endl;
  
}


int main() {
  std::cout << "hello d15" << std::endl;
  run_p2();
}
