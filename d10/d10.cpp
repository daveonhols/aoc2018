#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>


// helper to give a bounding box around points.
struct bounds {
  int left = 99999;
  int right = -99999;
  int top = 99999;
  int bottom = -99999;
};


// helper to describe a positions
struct position {

  int x, y = 0;

  position(int x, int y) {
    this->x = x; this->y = y;
  }
  bool operator<(const position& p) const {
    if (x == p.x)
      return y < p.y;
    return x < p.x;    
  }
  position up() {
    return position(x+1, y);
  }
  position down() {
    return position(x-1, y);    
  }
  position left() {
    return position(x, y-1);
  }
  position right() {
    return position(x, y+1);
  }
  friend std::ostream& operator<<(std::ostream& o, const position& p) {
    std::cout << "P: (" << p.x << ", " << p.y << ")" << std::endl;
    return o;
  }
};

// helper to describe a moving point (position + movement vector).
struct point {
  
  int x, y, dx, dy = 0;
  
  point(int x, int y, int dx, int dy) {
    this->x=x; this->y=y; this->dx=dx; this->dy=dy;
  }
  position pos() {
    return position(x, y);    
  }
  void step() {
    x += dx;
    y += dy;
  }

  friend std::ostream& operator<<(std::ostream& o, const point& p) {
    std::cout << "P: (" << p.x << ", " << p.y << ", " << p.dx << ", " << p.dy << ")" << std::endl;
    return o;
  }  
};

// prase one line of input from file
point parse_one(std::string line) {
  int x, y, dx, dy = 0;
  std::stringstream ss;
  ss.str(line.substr(10, 6));

  ss >> x;
  ss.clear();
  ss.str(line.substr(18, 6));

  ss >> y;
  ss.clear();
  ss.str(line.substr(36, 2));

  ss >> dx;
  ss.clear();
  ss.str(line.substr(40, 2));

  ss >> dy;
  
  return point{x, y, dx, dy};
}

// decide if a given point is isolated
bool isolated(point p, const std::set<position>& positions) {
  position base = p.pos();
  
  position u = base.up();
  position d = base.down();
  position l = base.left();
  position r = base.right();

  return
    positions.find(u) == positions.end()
    && positions.find(d) == positions.end()
    && positions.find(l) == positions.end()
    && positions.find(r) == positions.end();
  
}

// count number of isolated points
// we believe this will be small when the points are aligned into a message
int count_isolated(const std::vector<point>& points) {
  std::set<position> positions;
  std::transform(points.begin(), points.end(), std::inserter(positions, positions.begin()), [] (point p) { return p.pos(); });  
  int num_isolated = 0;
  std::for_each(points.begin(), points.end(), [&num_isolated, positions](point p){ num_isolated += isolated(p, positions) ? 1 : 0;  });
  return num_isolated;
}

// reduce over all points and a bounding box to find minimal space where points have aligned to a message
bounds reduce_one(bounds b, point p) {
  if (p.x < b.left) {
    b.left = p.x;
  }
  if (p.y < b.top) {
    b.top = p.y;
  }
  if (p.x > b.right) {
    b.right = p.x;
  }
  if (p.y > b.bottom) {
    b.bottom = p.y;
  }
  return b;
}

// print the message from aligned starts.  find the bounding box then print with ascii art
void print_msg( std::vector<point>& points) {

  bounds b = std::accumulate(points.begin(), points.end(), bounds{}, reduce_one);
  std::cout << "Limits :: " << b.left << "," << b.top << "," << b.right << "," << b.bottom << "." << std::endl;
  
  std::set<position> positions;
  std::transform(points.begin(), points.end(), std::inserter(positions, positions.begin()), [] (point p) { return p.pos(); });

  for (int yy = b.top; yy <= b.bottom; ++yy) {
    for (int xx = b.left; xx <= b.right; ++xx) {
      point p(xx, yy, 0, 0);
      std::cout << ((positions.find(p.pos()) == positions.end()) ? " " : (isolated(p, positions) ? "*" : "#"));
    }
    std::cout << std::endl;
  }
}

// run solution
void run() {
  std::vector<point> points;
  std::fstream input("d10/input.txt");
  std::string line;
  while (std::getline(input, line)) {
    points.push_back(parse_one(line));
  }
  int seconds = 0;
  while(seconds < 100000) {
    int num_isolated = count_isolated(points);
    if (num_isolated < 10) {
      std::cout << "(p2) Aligned at: " << seconds << "(off=" << num_isolated <<")" <<std::endl;
      print_msg(points);
      break;
    }
    std::for_each(points.begin(), points.end(), [&](point& p) { p.step(); });
    seconds++;
  }
  
}

int main() {
  std::cout << "hello d10" << std::endl;
  run();
  
}
