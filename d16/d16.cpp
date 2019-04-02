#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <array>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <numeric>

using cpu = std::array<int, 4>;

std::ostream& operator<<(std::ostream& o, cpu& c) {
  o << "[" << c[0] << ", " << c[1] << ", " << c[2] << ", " << c[3] << "]" << std::endl;
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
    if (lines.size() == 3) {
      result.push_back(parse_obs(lines));
      lines.clear();
      std::getline(file, line);
    }

  }
  return result;
}

std::vector<ins> read_p2() {
  std::vector<ins> result;
  std::ifstream file("d16/input_p2.txt");
  std::string line;

  while (std::getline(file, line)) {
    ins i{};
    std::stringstream read(line);
    read >> i.opcode >> i.op1 >> i.op2 >> i.out;
    result.push_back(i);
  }
  return result;
}

auto validate_one = [](obs ob, op opr){ return ob.after == opr(ob.instruction, ob.before); };

auto count_valid = [](obs ob1, std::vector<op> ops) { return std::count_if(ops.begin(), ops.end(), [=](op o){ return validate_one(ob1,o); });  };


std::unordered_map<int, op> resolve_unique(std::unordered_map<int, op> resolved, std::vector<obs> observed, std::vector<op> to_resolve) {
  
  if (to_resolve.size() == 0) {
    return resolved;
  }

  for (int i = 0; i < observed.size(); ++i) {
    int num_resolved = 0;
    int resolved_idx = 0;
    for (int j = 0; j < to_resolve.size(); ++j) {
      if (validate_one(observed[i], to_resolve[j])) {
	num_resolved++;
	resolved_idx = j;
	if (num_resolved > 1) {
	  break;
	}
      }
    }
    if(num_resolved == 1) {
      resolved.insert({observed[i].instruction.opcode, to_resolve[resolved_idx]});
      to_resolve.erase(to_resolve.begin() + resolved_idx);
    }
  }
      
  return resolve_unique(resolved, observed, to_resolve);;
}

int p2(std::vector<obs> observed, std::vector<op> operations, std::vector<ins> program) {
  std::unordered_map<int, op> opcodes{};
  opcodes = resolve_unique(opcodes, observed, operations);

  cpu c{0,0,0,0};
  for (ins i : program) {
    c = opcodes[i.opcode](i, c);
  }
  
  return c[0];
  
}

int p1(std::vector<obs> observed, std::vector<op> operations) {  
  return std::count_if(observed.begin(), observed.end(), [=](obs o) { return 3<= count_valid(o, operations);  });
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

  std::cout << "P1 :: " << p1(read_p1(), ops) << std::endl;
  std::cout << "P2 :: " << p2(read_p1(), ops, read_p2()) << std::endl;
  
}


