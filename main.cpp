#include <cxxabi.h>

#include <array>
#include <fstream>
#include <iostream>
#include <list>
#include <ostream>
#include <sstream>
#include <system_error>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ansi.h"
#include "indentos.h"
#include "pprint.h"

// Custom type with ostream operator
struct Point {
  int x, y;
  friend std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << "Point(" << p.x << "," << p.y << ")";
  }
};

// Custom type without ostream operator
struct Complex {
  double real, imag;
};

// Custom printer for Complex
template <>
struct Printer<Complex> {
  static void print(std::ostream& os, const Complex& c) {
    os << c.real << " + " << c.imag << "i";
  }
};

// Example class with print method
class Person {
 private:
  std::string name;
  int age;
  std::vector<std::string> hobbies;

 public:
  Person(std::string n, int a, std::vector<std::string> h)
      : name(std::move(n)), age(a), hobbies(std::move(h)) {}

  void print(std::ostream& os) const {
    os << "Person{";
    print_field(os, *this, &Person::name, "name");
    print_field(os, *this, &Person::age, "age");
    print_field(os, *this, &Person::hobbies, "hobbies");
    os << "}";
  }
};

struct Person2 {
  std::string name;
  int age;
  std::vector<std::string> hobbies;
};

void print(std::ostream& os, const Person2& p2) {
  os << "Person2{\n";
  {
    const indentos indent{os};
    print_field(os, p2, &Person2::name, "name");
    print_field(os, p2, &Person2::age, "age");
    print_field(os, p2, &Person2::hobbies, "hobbies");
  }
  os << "}";
}

int main(int, char**) {
  int clr = 0;
  ansi::SGR d;

  // std::cerr << ansi::fg::blue << "Hello"
  //           << (ansi::bold | ansi::dbl_underline | ansi::invert)
  //           << " world\n";

  unsigned char f = 12;
  std::cerr << f << "\n";

  std::vector<std::string> strs{"Hello", "World"};

  // Basic usage
  int x = 42;
  print(std::cout, x);  // Uses ostream operator

  print(std::cerr, Point{1, 2});

  // Nested containers
  std::vector<std::pair<int, std::string>> vec = {{1, "one"}, {2, "two"}};
  print(std::cout, vec);  // Will print: [(1, one), (2, two)]

  print(std::cerr, strs);

  std::list<int> lst = {1, 2, 3};
  print(std::cout, lst);  // [1, 2, 3]

  // Works with any map-like container
  std::map<int, std::string> map = {{1, "one"}, {2, "two"}};
  std::unordered_map<int, std::string> umap = {{1, "one"}, {2, "two"}};
  print(std::cout, map);   // {1: one, 2: two}
  print(std::cout, umap);  // {1: one, 2: two}

  // Works with any set-like container
  std::set<int> set = {1, 2, 3};
  std::unordered_set<int> uset = {1, 2, 3};
  print(std::cout, set);   // {1, 2, 3}
  print(std::cout, uset);  // {1, 2, 3}

  // Works with tuples
  std::tuple<int, std::string, double> t = {1, "hello", 3.14};
  print(std::cout, t);  // (1, hello, 3.14)

  print(std::cout, "Hello");

  print(std::cout, "Hello");                    // "Hello"
  print(std::cout, std::string("Hello"));       // "Hello"
  print(std::cout, std::string_view("Hello"));  // "Hello"

  // In containers
  std::vector<const char*> vecc = {"Hello", "World"};
  print(std::cout, vecc);  // ["Hello", "World"]

  // Usage
  Person p{"John", 30, {"reading", "coding"}};
  print(std::cout, p);  // Will use Person::print method
  Person2 p2{"John", 30, {"reading", "coding"}};
  print(std::cout, p2);  // Will use Person::print method
  return 0;
}
