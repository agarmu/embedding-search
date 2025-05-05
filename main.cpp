#include <iostream>
#include <z3++.h>

int main() {
  z3::context c;
  z3::solver s(c);

  z3::expr x = c.int_const("x");
  z3::expr y = c.int_const("y");

  s.add(x + y == 10);
  s.add(x > 0);
  s.add(y > 0);

  if (s.check() == z3::sat) {
    z3::model m = s.get_model();
    std::cout << "x = " << m.eval(x) << "\n";
    std::cout << "y = " << m.eval(y) << "\n";
  } else {
    std::cout << "unsat\n";
  }
}
