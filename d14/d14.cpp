#include <iostream>
#include <vector>

size_t step(size_t len, size_t curr, short num) {
  return (curr+num) % len;
}

bool check_p2(const std::vector<short>& scores, int target, int t_digits) {
  int mult = 1;
  int last = 0;
  for (int i = 1; i <= t_digits; ++i) {
    last += scores[scores.size() - i] * mult;
    mult *= 10;
  }
  if(last == target) {
    std::cout << "P2 :: " << scores.size() - t_digits << std::endl;
    return true;
  }
  return false;
}

int main() {
  std::cout << "hello d14" << std::endl;

  // track scores of generated receipies
  std::vector<short> scores{3, 7};

  // position of each elf in receipie list
  size_t e1 = 0;
  size_t e2 = 1;

  // p1 is, make this many receipies
  // p2 is, make receipies until last 6 recipies had these scores 
  int to_make = 765071;
  int to_make_digits = 6; // number of digits in above, to simplify check

  // keep going until done 
  while (true) {
    short next = scores[e1] + scores[e2]; // next receipie is adding each elfs previous

    // we canonly add two digits, so answer is in range 0-18
    if (next > 9) {
      scores.push_back(1);
      // check if p2 was satisfied after both digits have been placed separately
      if (check_p2(scores, to_make, to_make_digits)) {
	break;
      }
    }
    scores.push_back(next % 10);
    // check if p2 was satisfied after both digits have been placed separately
    if (check_p2(scores, to_make, to_make_digits)) {
      break;
    }

    // step each elf forwards 1 + last score, uses mod arith
    e1 = step(scores.size(), e1, 1 + scores[e1]);
    e2 = step(scores.size(), e2, 1 + scores[e2]);

    // P1 requires scores of ten receipies after to_make have been made
    if (scores.size() == to_make + 10) {
      std::cout << "P1 :: ";
      for (int i = scores.size() - 10; i < scores.size(); ++i) {	
	std::cout << scores[i];
      }
      std::cout << std::endl;   
    }
  }
  
  return 0;
}
