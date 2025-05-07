#include <cstddef>
#include <format>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <z3++.h>

using namespace z3;

bool is_prime(int p) {
  if (p <= 10) {
    return (p == 2) || (p == 3) || (p == 5) || (p == 7);
  }
  for (int j = 2; j * j <= p; j++) {
    if (p % j == 0)
      return false;
  }
  return true;
}

using result = std::pair<std::vector<int>, std::vector<int>>;

bool verify_embedding(int n, int p, result &res) {
  std::vector<int> &f = res.first;
  std::vector<int> &g = res.second;
  if (f.size() != (1 << n) || g.size() != p)
    return false;
  for (auto x : f) {
    if (x < 0 || x >= p)
      return false;
  }
  for (auto x : g) {
    if (x < 0 || x >= (1 << n))
      return false;
  }

  for (int i = 0; i < f.size(); i++) {
    for (int j = i; i < f.size(); i++) {
      if (g[(f[i] * f[j]) % p] != (i & j))
        return false;
    }
  }
  return true;
}

void print_embedding(int n, int p, result &res) {
  auto ver = verify_embedding(n, p, res);
  if (verify_embedding(n, p, res)) {
    std::cout << "An embedding was found and verified." << std::endl;
  } else {
    std::cout << "And embedding was found but could not be verified."
              << std::endl;
  }
  std::vector<int> &f = res.first;
  std::vector<int> &g = res.second;
  std::cout << "Map `f`:" << std::endl;
  for (int i = 0; i < f.size(); i++) {
    std::cout << std::format("\tf[{}] = {}", i, f[i]) << std::endl;
  }

  std::cout << "Map `g`:" << std::endl;
  for (int i = 0; i < g.size(); i++) {
    std::cout << std::format("\tg[{}] = {}", i, g[i]) << std::endl;
  }
}

std::optional<result> find_embedding(int n, int p) {
  if (!is_prime(p)) {
    throw std::invalid_argument(
        std::format("The value p = {} is not prime.", p));
  }
  context c;
  solver s(c);

  int size = 1 << n;

  // define and constrain f
  std::vector<expr> f;
  for (int i = 0; i < size; i++) {
    std::string name = std::format("f_{}", i);
    expr fi = c.int_const(name.c_str());
    f.push_back(fi);
  }

  // define g
  expr g = c.constant("g", c.array_sort(c.int_sort(), c.int_sort()));

  // homomorphic property
  for (int i = 0; i < size; i++) {
    for (int j = i; j < size; j++) {
      int expected = i & j;
      auto fmult = mod(f[i] * f[j], c.int_val(p));
      s.add(select(g, fmult) == c.int_val(expected));
    }
  }

  std::cout << std::format("Trying to find a ({}, {})-embedding.", n, p)
            << std::endl;
  std::cout << s << std::endl;

  if (s.check() == sat) {
    auto m = s.get_model();

    std::vector<int> f_table;
    for (int i = 0; i < size; ++i) {
      f_table.push_back(m.eval(f[i]).get_numeral_int());
    }

    std::vector<int> g_table;
    for (int i = 0; i < p; ++i) {
      g_table.push_back(
          m.eval(select(g, c.int_val(i)), true).get_numeral_int());
    }

    return result{f_table, g_table};
  }
  return std::nullopt;
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Expected a value `n` and a prime `p`." << std::endl;
    return 1;
  }
  std::string nstring(argv[1]), pstring(argv[2]);
  int n, p;
  try {
    n = std::stoi(nstring);
    if (n < 0)
      throw std::invalid_argument("");
  } catch (...) {
    std::cerr << std::format("The value `{}` is not a valid n value (must be "
                             "an integer > 0)",
                             nstring)
              << std::endl;
    return 1;
  }

  try {
    p = std::stoi(pstring);
    if (p <= n || !is_prime(p))
      throw std::invalid_argument("");
  } catch (...) {
    std::cerr
        << std::format(
               "The value `{}` is not a valid p value (must be a prime > n)",
               pstring)
        << std::endl;
    return 1;
  }
  auto v = find_embedding(n, p);
  if (!v.has_value()) {
    std::cout << std::format("No embedding found for n = {}, p = {}", n, p)
              << std::endl;
    return 0;
  }
  result val = v.value();
  print_embedding(n, p, val);
}
