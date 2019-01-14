#include "d6.hpp"
#include "gtest/gtest.h"

// Tests factorial of negative numbers.
TEST(distance, all) {

  point_t p1{3, 2};
  point_t p2{3, 4};
  point_t p3{2, 3};
  point_t p4{4, 3};
  point_t p5{2, 2};
  point_t p6{2, 4};

  node_t n1{1, point_t{3, 3}};
  EXPECT_EQ(1, distance(p1, n1));
  EXPECT_EQ(1, distance(p2, n1));
  EXPECT_EQ(1, distance(p3, n1));
  EXPECT_EQ(1, distance(p4, n1));
  EXPECT_EQ(2, distance(p5, n1));
  EXPECT_EQ(2, distance(p6, n1)); // sort out the abs bug
}
