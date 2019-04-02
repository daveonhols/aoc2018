#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <array>
#include <functional>

using cpu = std::array<int, 4>;
struct ins {
  int opcode;
  int op1;
  int op2;
  int out;
};

using op = std::function<cpu(ins,cpu)>;

// observation
struct obs {
  cpu before;
  ins instruction;
  cpu after;
};

obs parse_obs(std::vector<std::string> parts) {

  std::string dummy;
  std::stringstream rb(parts[0].substr(9));
  std::stringstream ri(parts[1]);
  std::stringstream ra(parts[2].substr(9));
  cpu b;
  cpu a;
  ins i;
  rb >> b[0] >> dummy >> b[1] >> dummy >> b[2] >> dummy >> b[3];
  ri >> i.opcode >> i.op1 >> i.op2 >> i.out;
  ra >> a[0] >> dummy >> a[1] >> dummy >> a[2] >> dummy >> a[3];

  obs result{};
  result.before = b;
  result.instruction = i;
  result.after = a;

  return result;
}

std::vector<obs> read_p1() {
  std::vector<obs> result{};
  std::ifstream file("d16/input_p1.txt");
  std::string line;
  std::vector<std::string> lines;
  while (std::getline(file, line)) {
    lines.push_back(line);
    if (lines.size() == 4) {
      result.push_back(parse_obs(lines));
      lines.clear();
    }
  }
  return result;
}

int main() {
  std::cout << "hello d16" << std::endl;

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

  std::vector<obs> observed = read_p1();
  
  //op mulr;
  
  
}


