#include <fstream>
#include <iostream>
#include <deque>
#include <unordered_set>


// define some helper type definitions
using rule_key = std::string;
using rule_map = std::unordered_set<rule_key>;
using pots = std::deque<char>;


// track state of pots from generation to generation
class pot_state {
  
public:
  pot_state(std::string start);  // initial start string
  void grow(const rule_map& r); // rules to apply on each iteration
  int score(); // score of this generation ( sum of occupied pots)
  friend std::ostream& operator<<(std::ostream& o, const pot_state& p); // helper for cout 
private:
  pots p;  // current pot state
  int offset;  // track how far left we have grown, we start with p[0] = pot zero but plants can grow in each direction
};

std::ostream& operator<<(std::ostream& o, const pot_state& p) {
  o << "pots [" << p.offset << "] " ;
  for( auto& a : p.p) {
    o << a;
  }
  o << std::endl;
  return o;
}

// count up sum of occupied pot index, but bearing in mind if we have grown to left, p[0] is actually the pot with -ve offset
int pot_state::score() {
  int score = 0;
  for (int i = 0; i < p.size(); ++i) {
    score += p[i] == '#' ? (i - offset) : 0;
  }
  return score;
}

// build initial state from input string
pot_state::pot_state(std::string start) {
  offset = 0;
  for ( auto& c : start) {
    p.push_back(c);
  }  
}


// apply rules to find which pots end up with a plant after a given generation
// like a game of life type of thing
void pot_state::grow(const rule_map& r) {
  
  // take care of left of center first, padded with no pots to left.
  // Requires special handling with push_front hence use of deque
  std::string l2 = std::string("....") += p[0];
  std::string l1 = (std::string("...") += p[0]) += p[1];

  // rule_map r will only contain rules that lead to growth, so presence / absense of a matching rule is all we need
  bool pl2 = !(r.find(l2) == r.end()); 
  bool pl1 = !(r.find(l1) == r.end()); 

  // we will build next generation into a new deque then copy over.
  // initialise the size of next to minimize re-allocations
  pots next(p.size() + 2, '.');

  // pot key is the look up key into rule_map required for given pot location.
  // we iterate through pots from left to right, so pot key is essentially a moving window
  // we initialise it for the p[0] case then delete from front and add to_back to move the window
  std::string potKey("..");// = std::string("..") += p[0];
  potKey += p[0];
  potKey += p[1];
  potKey += p[2];

  // we need to go from pot[0] to size + 2 because the plants can grow to right increasing the size of occuplied pots
  for (int i = 0; i < p.size() + 2; ++i) {    
    next[i] = r.find(potKey) == r.end() ? '.' : '#';  // place a plant if there is a rule matching current position
    potKey.erase(potKey.begin());  // move window LHS
    potKey.push_back(i + 3 >= p.size() ? '.' : p[i + 3]); // move window RHS
  }

  // trim pots RHS so we don't keep appending .... for  ever + clean up dead RHS plants
  while (next.back() == '.') {
    next.pop_back();
  }
  // trim pots LHS so we don't keep growing unnecessarily + clean up dead LHS plants
  // reduce offset if we have deleted a LHS pot
  while (next.front() == '.') {
    --offset;
    next.pop_front();
  }

  // handle the growing left case with special logic as we want to use push_front
  // but we only want to grow left if necessary
  // move next gen into current state before exiting
  if (pl2) {
    offset += 2;
    next.push_front(pl1 ? '#' : '.');
    next.push_front('#');
    p = std::move(next);
    return;
  }
  if (pl1) {
    offset +=1;
    next.push_front('#');
  }
  p = std::move(next);
  return;
}



int main() {
  std::cout << "hello d12" << std::endl;

  // input from AOC site
  pot_state pots{"#...##.#...#..#.#####.##.#..###.#.#.###....#...#...####.#....##..##..#..#..#..#.#..##.####.#.#.###"};
  rule_map rules;
  
  std::string line;
  std::ifstream file("d12/input.txt");
  while (std::getline(file, line)) {
    auto k = line.substr(0, 5);
    // only insert rules that cause plant growth, we can then use presence / absense of a rule and don't need to check ->second
    // this makes real input file logic match up with problem statement sample data
    if (line[9] == '#') {      
      rules.insert(k);
    }
  }

  // part one is simply the pots score @ generation 20
  // part two requires calculation up to 50 billion generations which is not realisitic
  //  so I use a ten generation look back window and use linear extrapolation to project the final score
  //  if the projected score is stable for 5 generations in a row, I assume we have reached a steady state and break out
  //  as this is a game of life style problem, we can definitely expect a stead state result eventually
  //  also by inspection of pot state, we do see a constant pattern moving to the right over time
  long generations = 50000000000L;

  // initalise look back window of correct size with dummy -1 scores
  std::deque<long> prev_scores(10,-1);
  long prev_projected = 0; // we need to know if current projection and last projection are same from gen to gen
  int stable = 0; //
  for (long i = 0; i < generations; ++i) {
    if (i == 20) {
      std::cout << "P1 :: " << pots.score() << std::endl;      
    }
    // next generation
    pots.grow(rules);

    // track latest N scores by dropping oldest and adding new
    prev_scores.pop_front();
    prev_scores.push_back(pots.score());

    // simple linear interpolation from last ten scores
    long rate = (prev_scores.back() - prev_scores.front()) / (prev_scores.size() - 1);
    long projected = ((generations - (i + 1)) * rate) + prev_scores.back();

    // if projected value changed, increase stable, else reset it to zero
    // we are looking for five *consecutive* stable values
    stable = projected == prev_projected ? stable + 1 : 0;

    prev_projected = projected;

    // check if we have settled on a stable score, finish if we have
    if (stable == 5) {
      std::cout << "P2 stabilised at :: " << i << " with score :: " << projected << std::endl;
      return 0; // exit from main      
    }    
  }
  std::cout << "P2 ran to completion (!!) with :: " << pots.score() << std::endl;
     
}
