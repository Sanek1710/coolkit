#include <cxxabi.h>

#include <array>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <ostream>
#include <set>
#include <sstream>
#include <system_error>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "coolkit/ansi.h"
#include "coolkit/indentos.h"
#include "coolkit/macro.h"
#include "coolkit/memstat.h"
#include "coolkit/pprint.h"
#include "coolkit/enum.h"

// Custom type with ostream operator
struct Point {
  int x, y;
  friend std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << "Point(" << p.x << ", " << p.y << ")";
  }
};

// Custom type without ostream operator
struct Complex {
  double real, imag;
  INLINE_MEMSTAT(Complex);
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

  INLINE_PRINT(Person, name, age, hobbies);
  INLINE_MEMSTAT(Person, name, age, hobbies);
};

struct Person2 {
  std::string name;
  int age;
  std::vector<std::string> hobbies;
};

PRINT_STRUCT(Person2, name, age, hobbies);
MEMSTAT_STRUCT(Person2, name, age, hobbies);


enum Values { Some, Sort, Of };
ENUM(Values, Some, Sort, Of);
ENUM_OSTREAM(Values)

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
  printout(x);  // Uses ostream operator

  print(std::cerr, Point{1, 2});

  // Nested containers
  std::vector<std::pair<int, std::string>> vec = {{1, "one"}, {2, "two"}};
  printout(vec);  // Will print: [(1, one), (2, two)]

  print(std::cerr, strs);

  std::list<int> lst = {1, 2, 3};
  printout(lst);  // [1, 2, 3]

  // Works with any map-like container
  std::map<int, std::string> map = {{1, "one"}, {2, "two"}};
  std::unordered_map<int, std::string> umap = {{1, "one"}, {2, "two"}};
  printout(map);   // {1: one, 2: two}
  printout(umap);  // {1: one, 2: two}

  // Works with any set-like container
  std::set<int> set = {1, 2, 3};
  std::unordered_set<int> uset = {1, 2, 3};
  printout(set);   // {1, 2, 3}
  printout(uset);  // {1, 2, 3}

  // Works with tuples
  std::tuple<int, std::string, double> t = {1, "hello", 3.14};
  printout(t);  // (1, hello, 3.14)

  printout("Hello");

  printout("Hello");                    // "Hello"
  printout(std::string("Hello"));       // "Hello"
  printout(std::string_view("Hello"));  // "Hello"

  // In containers
  std::vector<const char*> vecc = {"Hello", "World"};
  printout(vecc);  // ["Hello", "World"]

  // Usage
  Person p{"John", 30, {"reading", "codingcodingcoding"}};
  printout(p);  // Will use Person::print method
  Person2 p2{"John", 30, {"reading", "coding"}};
  printout(p2);  // Will use simplified struct printing

  std::optional<std::string> opts = std::nullopt;
  printout(opts);

  std::vector<std::vector<int>> vvec{
      {1, 2, 3},
      {3, 4, 5},
      {7, 8, 9, 10},
  };

  printout(vvec);

  std::cerr << memstat(p) << "\n";
  std::cerr << memstat(p2) << "\n";

  std::vector<Person2> vp2{p2, p2};
  printout(vp2);

  std::string str = "codingcodingcoding";
  printout(sizeof(str));
  printout(str.capacity());
  str += 'a';
  printout(str.capacity());


  static constexpr auto val0 = Enum<Values>::size;
  static constexpr auto val1 = Enum<Values>::values;
  static constexpr auto val2 = Enum<Values>::string(Values::Some);
  Enum<Values>::foreach (printout<Values>);

  return 0;
}
