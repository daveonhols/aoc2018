#include "d7.hpp"
#include "gtest/gtest.h"

// Tests for d7
TEST(d7, all) {
  depends d{};
  d.add('C', 'A');
  d.add('C', 'F');
  d.init();
  EXPECT_EQ('C', d.next());
  std::vector<char> r = d.get_resolved('C');
  EXPECT_EQ(2, r.size());
  EXPECT_TRUE(r.end() != std::find(r.begin(), r.end(), 'A'));
  EXPECT_TRUE(r.end() != std::find(r.begin(), r.end(), 'F'));
  std::cout << " --..-- " << std::endl;
  EXPECT_TRUE(d.last_required('C', 'A'));
  EXPECT_TRUE(d.last_required('C', 'F'));
}
