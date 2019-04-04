#include <vector>

#include "d15.hpp"
#include "gtest/gtest.h"

grid_t build_grid(std::vector<std::string> lines) {
  grid_t result{};
  int row = 0;
  while (row < lines.size()) {
    for (int col = 0; col < lines[row].length(); ++col) {
      switch (lines[row][col]) {
      case '#':
      case 'G':
      case 'g':
      case 'e':
      case 'E':
	result[{row, col}] = '#';
	break;	
      case '1':
      case '2':
      case '.':
      case 'X':
	result[{row, col}] = '.';
	break;	
      }
    }
    ++row;
  }
  return result;

  
}

// Tests factorial of negative numbers.
TEST(astar, all) {

  std::vector<std::string> lines;

  lines.push_back("################################");
  lines.push_back("################..##############");
  lines.push_back("################..##############");
  lines.push_back("##############.....#############");
  lines.push_back("##############....###.##########");
  lines.push_back("###############..####.##########");
  lines.push_back("###############...###.##########");
  lines.push_back("#############.....#....##..#####");
  lines.push_back("#############.......#.......####");
  lines.push_back("##############.......#..#...####");
  lines.push_back("#####..#######.#...............#");
  lines.push_back("#####..####.............#...#.##");
  lines.push_back("#####.........#####....###..####");
  lines.push_back("####.#...##..#######...#...#####");
  lines.push_back("###.......#.#########.##2..#####");
  lines.push_back("####..#..#..#########..#.....###");
  lines.push_back("######....###########........###");
  lines.push_back("######......#########..........#");
  lines.push_back("#######.....#########.....#....#");
  lines.push_back("######.......#########.....#.#.#");
  lines.push_back("####.#.......#######.#1.#..#####");
  lines.push_back("####........##......#.##...#####");
  lines.push_back("###.#........##.#.....##..######");
  lines.push_back("#.............######......######");
  lines.push_back("###......#......####..#...######");
  lines.push_back("###..X.#........#######.########");
  lines.push_back("###...#.###....########..#######");
  lines.push_back("###..#..##....#######.....######");
  lines.push_back("####....#############.##.#######");
  lines.push_back("####..#..###########...#########");
  lines.push_back("#####...############.#.#########");
  lines.push_back("################################");

  pos start = {25,5};
  pos end_1 = {20,22};
  pos end_2 = {14,24};
  grid_t grid = build_grid(lines);

  std::map<pos, int> o{};
  occupied<int, int> empty(o, o, o, o);
  
  route r1 = *measure_astar(start, end_1, empty, grid);
  route r2 = *measure_astar(start, end_2, empty, grid);

  route r1b = *measure_astar_map(start, end_1, empty, grid);
  route r2b = *measure_astar_map(start, end_2, empty, grid);
  
  
  EXPECT_EQ(40, r1.length);
  EXPECT_EQ(40, r2.length);

  EXPECT_EQ(40, r1b.length);
  EXPECT_EQ(40, r2b.length);
    
}
