#include <fstream>
#include <deque>
#include <functional>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "d15.hpp"

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
  
  std::multiset<route, std::function<bool(route, route)>> routes([](route r1, route r2){ return r1.length < r2.length; });

  for (pos p : in_range) {
    // std::cout << "Route :: " << std::endl;
    std::optional<route> route = measure_astar_map(moving.first, p, occ, g);
    if(route) {
      routes.insert(*route);
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
  //std::sort(routes.begin(), routes.end(), );
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
  pos next_step = best_first_step_map(best, occ, g);
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


// overall gameloop handler
// checks who has next turn, does each turn, and decides the game state after each turn
// game state logic is delegated to state_t state_manager as termination and retry rules are different in P1 vs P2
std::optional<int> run(grid_t g, elves_t elves, goblins_t goblins, state_t state_manager) {
  int gen = 0;
  while (true) {
    elves_t next_elves {};
    goblins_t next_goblins {};
    bool done_round = false;
    while (!done_round) {
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
      case move_type::round: // finished current round, all pieces have moved, reset to unmoved
	done_round = true;
	break;
      case move_type::elves_won:
	std::cout << "Elves won at " << gen << " with " << score_survivors(elves, next_elves) << std::endl;
	return std::optional<int>{gen * score_survivors(elves, next_elves)};
	break;
      case move_type::goblins_won:
	std::cout << "Goblins won at " << gen << " with " << score_survivors(goblins, next_goblins) <<  std::endl;
	return std::optional<int>{gen * score_survivors(goblins, next_goblins)};
	break;	
      case move_type::retry:
	// for part 2, we search for a minimum elf hit points to win without dying
	// if any elf died, we can break out and retry the simulation from start with a new hp value
	return std::optional<int>{};
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


// helper to drive running either part 1 or part 2
// reads the input file runs as necessary
// part 1 uses elves fixed with 3 attack points
// part 2 uses elves with variable attack points, elf builder works as a factory to support that
// state manager decides when the game is finished and the termination reason
// for part 1, we finish when done, for part 2, we need to retry as we search for a minimum elf attack points value
std::optional<int> run_part(elf_builder eb, state_t t) {

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
  }
  return run(g, elves, goblins, t);
  
}

// part 1 runner, 
// run the driver with default elf builder and p1 state manager
void run_p1() {
  std::optional<int> result = run_part([](){ return elf();}, next_move_p1 );
  std::cout << "P1 :: " << *result << std::endl;
}

// part 2 runner
// search for the minimum elf attack point value which allows them to win without losing a unit
// we do a binary search assuming the attack points a split from <lose> to <win> somewhere
// each iteration through the binary search plugs a different ap value in to the elf builder
// the next_move_p2 helper will return false when the elves do not win without a single lost unit and true when they do
void run_p2() {
  std::vector<int> hp {};
  for (int i = 3; i < 190; ++i) {
    hp.push_back(i);
  }

  std::map<int, int> hp_scores{};
  
  auto p2_pred = [] (int x) {
    std::cout << "Trying elves with HP = " << x << std::endl;
    std::optional<int> result = run_part([=](){ return elf(x); }, next_move_p2);
    std::cout << (result ? "Elves won without loss" : "Elves suffered a loss") << std::endl;
    return result;
  };
  
  while (hp.size() > 1) {
    int mid = (2 == hp.size()) ? 0 : hp.size() / 2;
    int try_hp = hp[mid];
    std::optional<int> r = p2_pred(try_hp);
    hp.erase(r ? hp.begin() + mid + 1 : hp.begin(), r ? hp.end() : hp.begin() + mid + 1);
    if (r) {
      hp_scores.insert({try_hp, *r});
    }
  }
  std::cout << "HP == " << hp[0] << std::endl;
  std::cout << "P2 :: " << hp_scores.find(hp[0])->second << std::endl;
}


int main() {
  std::cout << "hello d15" << std::endl;
  run_p1();
  run_p2();
}
