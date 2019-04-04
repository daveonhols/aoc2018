#include <iostream>
#include <vector>
#include <fstream>
#include <deque>
#include <algorithm>


// define types of squares that exist in the simulation
enum class square {
  open, tree, lumberyard
};

// type defining the overall grid fo squares
using area_t = std::vector<std::vector<square>>;

// count the neighbours of a square
// max overall is eight neighbours
struct neighbours {
  int trees;
  int spaces;
  int lumberyards;
};

// convert a char in input file to enumerated tree / lumberyard / open space
square read(char c) {
  switch (c) {
  case '.':
    return square::open;
    break;
  case '#':
    return square::lumberyard;
    break;
  case '|':
    return square::tree;
    break;
  }
  throw std::logic_error("impossible square read in file");
}

// Debug -- convert an enumerated square type to a char for display
char write(square s) {
  switch (s) {
  case square::open:
    return '.';
    break;
  case square::lumberyard:
    return '#';
    break;
  case square::tree:
    return '|';
    break;
  }
  throw std::logic_error("impossible square type to write");

}

// count up types of objects neighbouring a given cell (row / col)
neighbours get_neighbours(const area_t& area, int row, int col) {

  neighbours result{};

  result.spaces = 0;
  result.trees = 0;
  result.lumberyards = 0;

  // consider left (y -1) right (y + 1)
  // up (x - 1) down (y - 1)
  // skip if out of bound and skip self (x - 0 && y - 0)
  for (int i = -1; i < 2; ++i) {
    for (int j = -1; j < 2; ++j) {
      int nr = row + i; // neighbour row
      int nc = col + j; // neighbour col
      if ((nr >= 0) && (nc >=0) && (nr < area.size()) && (nc < area.size()) && !((i == 0) && (j == 0))) {
	switch(area[nr][nc]) {
	case square::open:
	  result.spaces += 1;
	  break;
	case square::tree:
	  result.trees += 1;
	  break;
	case square::lumberyard:
	  result.lumberyards += 1;
	  break;
	}
      }
    }
  }
  return result;

}

// implement rule changing a square from one generation to next as per puzzle description based on neighbours
square get_next(square prev, const neighbours& n) {
  if (prev == square::open && n.trees > 2) {
    return square::tree;
  }
  if (prev == square::tree && n.lumberyards > 2) {
    return square::lumberyard;
  }
  if (prev == square::lumberyard) {
    if (n.trees > 0 && n.lumberyards > 0) {
      return square::lumberyard;
    } else {
      return square::open;
    }    
  }
  return prev;
}

// implement changing a whole grid from one generatio for another
// changes cell by cell, finding neighbours each time
area_t grow(const area_t& prev) {
  area_t next(prev);
  for (int row = 0; row < prev.size(); ++row) {
    for (int col = 0; col < prev[row].size(); ++col) {
      neighbours n = get_neighbours(prev, row, col);
      next[row][col] = get_next(prev[row][col], n);
    }
  }
  return next;
}

// Debug -- draw the full area to std::cout for validation
void render(const area_t& ground) {
  for (int row = 0; row < ground.size(); ++row) {
    for (int col = 0; col < ground[row].size(); ++col) {
      std::cout << write(ground[row][col]);
    }
    std::cout << std::endl;
  }  
}

// score is number of wood squares * number of lumber squares in the whole grid
// simple iterate, count and multiply
int calc_score(const area_t& ground) {
  int wood = 0;
  int lumber = 0;
  for (int row = 0; row < ground.size(); ++row) {
    for (int col = 0; col < ground[row].size(); ++col) {
      if (ground[row][col] == square::tree) {
	wood +=1;
      }
      if (ground[row][col] == square::lumberyard) {
	lumber += 1;
      }
    }
  }
  return wood * lumber;
}

// we are asked to calculate the score after a billion iterations
// however that is probably not realistic.  The puzzle gives a clue that the pattern will repeat
// and since this is a "game of life" style problem, we can definitely expect that.
// We search for cycles by keeping a list of previously seen scores, and once we have the same score S three times
// we check to see if the values from S1 to S2 are the same as those from S2 to S3.
// if they do, we probably have a cycle
bool check_cycle(const std::deque<int>& scores, int score) {
  auto s1 = std::find(scores.begin(), scores.end(), score);
  auto s2 = std::find(s1 + 1, scores.end(), score);
  auto s3 = std::find(s2 + 1, scores.end(), score);
  return std::search(s1, s2, s2, s3) != s2;
}

// once we know the scores are repeating, we can use the cycle pattern to project the score out to generation 1000000000
// we do this by looking for the length of the cycle, the generation it starts at and then using maths
int project(const std::deque<int>& scores, int generations, int last_gen) {
  auto s1 = std::find(scores.begin(), scores.end(), scores.back());
  auto s2 = std::find(s1 + 1, scores.end(), scores.back());
  int cycle_length = std::distance(s1,s2);
  return *(s1 + ((generations - last_gen) % cycle_length));
}


// build up the data grid from input file
area_t read_input() {
  area_t ground;
  std::ifstream file("d18/input.txt");
  std::string line;

  int row =0;
  while(std::getline(file, line)) {
    ground.push_back(std::vector<square>{});
    for (int col = 0; col < line.length(); ++col) {
      char next = line[col];
      ground[row].push_back(read(next));
    }
    ++row;
  }
  return ground;
}

int main() {
  std::cout << "hello d18" << std::endl;

  area_t ground = read_input();

  // keep buffer of scores to look for cycles
  std::deque<int> buffer{};

  
  int generations = 1000000000;
  int gen = 0;

  for (; gen < generations; ++gen) {
    int score = calc_score(ground);

    
    // for part 1 we just want the score after ten iterations
    if (gen == 10)  {
      std::cout << "P1 :: Gen(10)" << " :: " << score << std::endl;
    }

    // track buffer
    buffer.push_back(score);

    // if we have three instances of the same score, check if we have a repeating pattern from S1 to S2 and S2 to S3
    if (3 == std::count(buffer.begin(), buffer.end(), score)) {
      if (check_cycle(buffer, score)) {
	std::cout << "Cycle found at gen :: " << gen << std::endl;
	std::cout << "P2 :: Projected Score (" << generations << ") :: " << project(buffer, generations, gen) << std::endl;
	break;
      }
    }

    // keep the size of the score buffer to 100 (cycle length for my puzzle input is 28)
    if (buffer.size() > 100) {
      buffer.pop_front();
    }    
    ground = grow(ground);
  }

  // no
  if (gen == generations - 1) {
    std::cout << "P2 :: Gen(" << generations << ") (Ran to completion (!!)"  << " :: " << calc_score(ground) << std::endl;
  }  

}
