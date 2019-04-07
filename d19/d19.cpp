#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>

#include "../d16/d16.hpp"

class runner {
private:
  cpu c;
  int ip_r;
  std::vector<ins> program;
  std::vector<op> ops;
public:
  runner(int ip_r, std::vector<ins> program, std::vector<op> ops, cpu init = {0,0,0,0,0,0}) : c(init),ip_r(ip_r), program(program), ops(ops) {};
  bool run_one() {
    int op_idx = c[ip_r];

    if (op_idx >= program.size()) {
      return false;
    }
    
    ins next = program[op_idx];
    op o = ops[next.opcode];
    c = o(next, c);
    c[ip_r]++;
    return true;
  }
  void bind_ipr(int ipr) {
    ip_r = ipr;
  }
  int read_reg(int i) {
    return c[i];
  }
};

runner build(cpu initial) {
  std::fstream input("d19/input.txt");
  std::string line;
  std::vector<ins> program;
  int ipr;
  while (std::getline(input, line)) {
    if (line.find("#ip") == 0) {
      ipr = line[4] - '0';      
    } else {
      ins instruction;
      std::stringstream read_args{line.substr(5)};
      std::string op_name = line.substr(0, 4);
      std::vector<std::string>::iterator s_op = std::find(op_names.begin(), op_names.end(), op_name);
      int op_idx = std::distance(op_names.begin(), s_op);
      instruction.opcode = op_idx;
      char junk;
      read_args >> instruction.op1 >> instruction.op2 >> instruction.out;
      program.push_back(instruction);
    }
  }
  runner result(ipr, program, ops, initial);
  return result;
}


// helper function used to deduce that the 
void test(int a, int b) {
  runner t1 = build({0,a,b,0,0,25});
  while (t1.run_one()) {

  }
  std::cout << "TEST :: " << a << " :: " << b << " :: " << t1.read_reg(0) << std::endl;
}

// by inspecting the cpu state and code behaviour, I know that the program is doing two things
// 1. setting up an initial state number in register 2
// 2. calculating the sum of all divisors of that number (including 1 and itself)
// 3. exiting with the sum of all divisors in reg 0
// the difference between part 1 and part 2 is the initial state number is much larger
// calculation of the result starts with ip = 25 (see test() above)
void p2_manually() {
  int divs = 0;
  for (int i = 1; i <= 10551430 ; ++i) {
    if (0 == 10551430 % i) {
      divs += i;
    }      
  }
  std::cout << "P2 (by inspection) :: " << divs << std::endl;
}

int main() {

  std::cout << " hello d19" << std::endl;
  runner do_p1 = build({0,0,0,0,0,0});
  while (do_p1.run_one()) {
    
  }
  std::cout << "P1 :: " << do_p1.read_reg(0) << std::endl;

  p2_manually();
  
  
}
