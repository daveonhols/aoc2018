#include <vector>
#include <iostream>
#include <functional>

using cpu = std::vector<int>;

std::ostream& operator<<(std::ostream& o, cpu& c) {
  o << "[";
  for (int rv : c) {
    o << rv << ", ";    
  }
  o << "]" << std::endl;
  return o;
}

struct ins {
  int opcode;
  int op1;
  int op2;
  int out;
};

std::ostream& operator<<(std::ostream& o, ins& ins) {
  o << "[" << ins.opcode << ", " << ins.op1 << ", " << ins.op2 << ", " << ins.out << "]" << std::endl;
  return o;
}

using op = std::function<cpu(ins,cpu)>;

op addr = [](ins i, cpu c){ c[i.out] = c[i.op1] + c[i.op2]; return c;  };
op addi = [](ins i, cpu c){ c[i.out] = c[i.op1] + i.op2; return c;  };
  
op mulr = [](ins i, cpu c){ c[i.out] = c[i.op1] * c[i.op2]; return c;  };
op muli = [](ins i, cpu c){ c[i.out] = c[i.op1] * i.op2; return c;  };

op banr = [](ins i, cpu c){ c[i.out] = c[i.op1] & c[i.op2]; return c;  };
op bani = [](ins i, cpu c){ c[i.out] = c[i.op1] & i.op2; return c;  };

op borr = [](ins i, cpu c){ c[i.out] = c[i.op1] | c[i.op2]; return c;  };
op bori = [](ins i, cpu c){ c[i.out] = c[i.op1] | i.op2; return c;  };

op setr = [](ins i, cpu c){ c[i.out] = c[i.op1]; return c; };
op seti = [](ins i, cpu c){ c[i.out] = i.op1; return c; };

op gtir = [](ins i, cpu c) { c[i.out] = i.op1 > c[i.op2] ? 1 : 0; return c;  };
op gtri = [](ins i, cpu c) { c[i.out] = c[i.op1] > i.op2 ? 1 : 0; return c;  };
op gtrr = [](ins i, cpu c) { c[i.out] = c[i.op1] > c[i.op2] ? 1 : 0; return c;  };

op eqir = [](ins i, cpu c) { c[i.out] = i.op1 == c[i.op2] ? 1 : 0; return c;  };
op eqri = [](ins i, cpu c) { c[i.out] = c[i.op1] == i.op2 ? 1 : 0; return c;  };
op eqrr = [](ins i, cpu c) { c[i.out] = c[i.op1] == c[i.op2] ? 1 : 0; return c;  };

std::vector<op> ops = { addr, addi, mulr, muli, banr, bani, borr, bori, setr, seti, gtir, gtri, gtrr, eqir, eqri, eqrr };

std::vector<std::string> op_names = { "addr", "addi", "mulr", "muli", "banr", "bani", "borr", "bori", "setr", "seti", "gtir", "gtri", "gtrr", "eqir", "eqri", "eqrr" };
