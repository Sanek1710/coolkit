

#include <bitset>
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <ratio>
#include <tuple>
#include <type_traits>

#include "coolkit/fmtstruct.h"
#include "coolkit/indentos.h"
#include "coolkit/macro.h"
#include "coolkit/pprint.h"
#include "coolkit/structinfo.h"
#include "fmt/base.h"
#include "fmt/color.h"
#include "fmt/format.h"
#include "fmt/ostream.h"
#include "fmt/ranges.h"

struct Point {
  int x, y;
};

class Person {
 public:
  std::string name;
  int age;
  std::vector<std::string> hobbies;
  std::vector<Point> points;

  Person(std::string name, int age, std::vector<std::string> hobbies,
         std::vector<Point> points, int height)
      : name(name),
        age(age),
        hobbies(hobbies),
        points(points),
        height(height) {}

 private:
  int height = 0;

  friend fmt::formatter<Person>;
};

template <>
struct fmt ::formatter<Person> : struct_formatter {
  template <typename Context>
  auto format(const Person& obj, Context& ctx) const -> decltype(ctx.out()) {
    structf("Person", ctx)
        .fieldf("name", obj.name)
        .fieldf("age", obj.age)
        .fieldf("hobbies", obj.hobbies)
        .fieldf("points", obj.points, "{::cnt}")
        .fieldf("height", obj.height);
    return ctx.out();
  }
};

template <>
struct fmt ::formatter<Point> : struct_formatter {
  template <typename Context>
  auto format(const Point& obj, Context& ctx) const -> decltype(ctx.out()) {
    structf("Point", ctx, flags | flag_values_only, {",", "(", ")"})
        .fieldf("x", obj.x)
        .fieldf("y", obj.y);
    return ctx.out();
  }
};

int main() {
  Person p1{
      "John", 30, {"reading", "codingcodingcoding"}, {{1, 2}, {3, 4}}, 12};
  Person p2{"Simon", 24, {"writing", "cooking"}, {{1, 5}, {4, 8}}, 10};
  Person p3{
      "Cassidy", 19, {"working", "chilling"}, {{7, 3}, {1, 9}, {5, 3}}, 9};

  std::vector<Person> persons{p1, p2, p3};

  {
    indent::ctrl::ostream indentos{std::cout};
    fmt::print(std::cout, "{:nct}\n", p1);
  }

  indent::ctrl::ostream_iterator cit{std::cout};
  fmt::format_to(cit, "{:nct}\n", p1);

  std::cerr << fmt::format("{:nc}\n", p1);
  std::cerr << fmt::format("{:c}\n", p1);

  return 0;
}