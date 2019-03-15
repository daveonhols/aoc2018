#include <iostream>
#include <optional>
#include <tuple>
#include <vector>
#include <map>
#include <cmath>

// cache sampled values for quicker look up
// x, y, ss => power
using samples = std::map<std::tuple<int, int, int>, int>;

// make using cached values optional ... not required on part 1, but is on part 2 for perf.
using o_samples = std::optional<samples>;


using grid = std::vector<std::vector<int>>;

// x, y, power
using power_at = std::tuple<int, int, int>;

// x, y, power, sample size
using power_at_size = std::pair<power_at, int>;

// get hundreds digit, i.e. 3 from 12345
int hundreds(int num) {
  num = std::abs(num);
  return (num / 100) % 10;
}

// calculate power from given formula
int power(int x, int y, int serial = 7689) {
  int rack = x + 10;
  int power = rack * y;
  power += serial;
  power *= rack;
  return hundreds(power) - 5;
}

// build grid, default with puzzle input of 7689
grid build(int serial = 7689) {
  grid g{};
  for (int x = 0; x < 300; ++x) {
    std::vector<int> row{};
    for (int y = 0; y < 300; ++y) {
      row.push_back(power(x + 1, y + 1, serial));
    }
    g.push_back(row);
  }
  return g;
}

// calculate a sampled power value, faster because it can derived value for x, y, ss from x, y, ss - 1
// samples are stored for future invocation
// implied usage is to calculate ss - 1 before ss, i.e. can't be used for one stand alone sample size (i.e. part 1, ss = 3)
int fast_sample(samples& s, const grid& g, int x, int y, int sample_size = 3) {
  int power = 0;
  if (sample_size == 1) {
    power = g[x][y];
  } else {
    std::tuple<int, int, int> sub_sample{x, y, sample_size - 1};
    samples::iterator i_sub_sample = s.find(sub_sample);
    power = i_sub_sample->second;
    s.erase(i_sub_sample);
    for (int i = 0; i < sample_size - 1; ++i) {
      power += g[x + (sample_size - 1)][y + i];
      power += g[x + i][y + (sample_size - 1)];
    }
    power += g[x + (sample_size - 1)][y + (sample_size - 1)];    
  }
  s[std::tuple<int, int, int>{x, y, sample_size}] = power;
  return power;
}

// calculate sampled power directly
int sample(const grid& g, int x, int y, int sample_size = 3) {
  int score = 0;
  for (int xx = 0; xx < sample_size; ++xx) {
    for (int yy = 0; yy < sample_size; ++yy) {
      score += g[x + xx][y + yy];
    }
  }
  return score;
}

// calculate highest density power available in grid, based on sampling a square of nodes.
power_at best(o_samples& s, const grid& g, int sample_size = 3) {
  power_at best{};
  int max = -99999;
  for (int x = 0; x < 300 - (sample_size - 1); ++x) {
    for (int y = 0; y < 300 - (sample_size - 1); ++y) {

      int curr = s ? fast_sample(*s, g, x, y, sample_size) : sample(g, x, y, sample_size);
      
      if (curr > max) {
	max = curr;
	best = power_at{x + 1, y + 1, curr};
      }
    }
  }
  return best;
}

// calculate highest density power available in grid over all possible square sample sizes.
power_at_size all_best(grid g) {
  power_at_size max_power_at{};
  samples s{};
  o_samples os{s};
  int max_power = -99999;
  for (int ss = 1; ss < 300; ++ss) {
    power_at this_best = best(os, g, ss);
    int curr = std::get<2>(this_best);

    if (curr > max_power) {
      max_power_at = power_at_size{this_best, ss};
      max_power = curr;
    }
  }
  return max_power_at;
}

int main() {
  
  grid g = build();
  o_samples no_sampling{};  // for part 1, do not configure the optional samples map
  power_at p1 = best(no_sampling, g);
  std::cout << "P1 :: (" << std::get<0>(p1) << " , " << std::get<1>(p1) << ")" << std::endl;


  // part 2 will build an optional samples map internally, allows deriving ss N from ss N - 1
  power_at_size p2 = all_best(g);
  std::cout << "P2 :: (" << std::get<0>(p2.first) << ", " << std::get<1>(p2.first) << ", " << p2.second << ")" << std::endl;
}
