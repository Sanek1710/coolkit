#pragma once

#include <array>
#include <ostream>
#include <sstream>

namespace ansi {

namespace color {

enum {
  black = 0,
  red = 1,
  green = 2,
  yellow = red | green,
  blue = 4,
  magenta = red | blue,
  cyan = green | blue,
  white = red | green | blue,
  deflt = 9,

  set = 8,  // for 8 bit and 24 bit colors
};

}

struct Ansi {
  template <typename... Args>
  constexpr Ansi(char code, char command, Args... args)
      : code(code), command(command), args{args...}, nargs(sizeof...(args)) {
    static_assert(sizeof...(args) <= 5, "Too many arguments");
    static_assert(((std::is_integral_v<Args>)&&...),
                  "All arguments must be integers");
  }
  char code;
  char command;
  uint16_t nargs;
  std::array<int, 5> args;
};

inline std::ostream &operator<<(std::ostream &os, const ansi::Ansi &a) {
  os << '\e' << a.code;
  for (int i = 0; i < a.nargs; ++i) {
    if (i) os << ";";
    os << a.args[i];
  }
  return os << a.command;
}

struct AnsiGroup {
  std::string str;

  AnsiGroup &operator|=(Ansi a2) {
    std::stringstream ss;
    ss << a2;
    str += ss.str();
    return *this;
  }
  AnsiGroup operator|(Ansi a2) const {
    std::stringstream ss;
    ss << a2;
    return {str + ss.str()};
  }
  AnsiGroup operator|(AnsiGroup other) const { return {str + other.str}; }
};

inline AnsiGroup operator|(Ansi a1, Ansi a2) {
  std::stringstream ss;
  ss << a1 << a2;
  return {ss.str()};
}

inline std::ostream &operator<<(std::ostream &os, const AnsiGroup &a) {
  return os << a.str;
}

struct CSI : public Ansi {
  template <typename... Args>
  constexpr CSI(char command, Args... args) : Ansi('[', command, args...) {}
};

// clang-format off
constexpr auto up       (int n = 1) { return CSI{'A', n}; }
constexpr auto down     (int n = 1) { return CSI{'B', n}; }
constexpr auto forward  (int n = 1) { return CSI{'C', n}; }
constexpr auto back     (int n = 1) { return CSI{'D', n}; }
constexpr auto next_line(int n = 1) { return CSI{'E', n}; }
constexpr auto prev_line(int n = 1) { return CSI{'F', n}; }

constexpr auto setx (int x = 1)     { return CSI{'G', x}; }
constexpr auto move (int y, int x)  { return CSI{'H', y, x}; }

constexpr auto clear        () { return CSI{'J', 2}; }
constexpr auto clear_after  () { return CSI{'J', 0}; }
constexpr auto clear_before () { return CSI{'J', 1}; }

constexpr auto clear_line       () { return CSI{'K', 2}; }
constexpr auto clear_line_after () { return CSI{'K', 0}; }
constexpr auto clear_line_before() { return CSI{'K', 1}; }

constexpr auto scroll_up  (int n) { return CSI{'S', n}; }
constexpr auto scroll_down(int n) { return CSI{'T', n}; }

constexpr auto save   () { return CSI{'s', }; }
constexpr auto restore() { return CSI{'u', }; }
// clang-format on

struct SGR : public CSI {
  template <typename... Args>
  constexpr SGR(Args... args) : CSI('m', args...) {}
};

// clang-format off

constexpr auto reset      = SGR{0};

constexpr auto bold       = SGR{1};
constexpr auto dim        = SGR{2};
constexpr auto italic     = SGR{3};
constexpr auto underline  = SGR{4};
constexpr auto blink      = SGR{5};
constexpr auto rapid_blink = SGR{6};
constexpr auto invert     = SGR{7};
constexpr auto hide       = SGR{8};
constexpr auto strike     = SGR{9};

constexpr auto dbl_underline = SGR{21};

constexpr auto no_dim        = SGR{22};
constexpr auto no_italic     = SGR{23};
constexpr auto no_underline  = SGR{24};
constexpr auto no_blink      = SGR{25};
constexpr auto proportional_spacing = SGR{26};
constexpr auto no_invert     = SGR{27};
constexpr auto no_hide       = SGR{28};
constexpr auto no_strike     = SGR{29};
// clang-format on

namespace fg {

static constexpr int base = 30;
// clang-format off
constexpr auto black   = SGR{base + color::black};
constexpr auto red     = SGR{base + color::red};
constexpr auto green   = SGR{base + color::green};
constexpr auto yellow  = SGR{base + color::yellow};
constexpr auto blue    = SGR{base + color::blue};
constexpr auto magenta = SGR{base + color::magenta};
constexpr auto cyan    = SGR{base + color::cyan};
constexpr auto white   = SGR{base + color::white};
constexpr auto deflt   = SGR{base + color::deflt};
// clang-format on

static constexpr int bright_base = 90;
// clang-format off
constexpr auto bright_black   = SGR{bright_base + color::black};
constexpr auto bright_red     = SGR{bright_base + color::red};
constexpr auto bright_green   = SGR{bright_base + color::green};
constexpr auto bright_yellow  = SGR{bright_base + color::yellow};
constexpr auto bright_blue    = SGR{bright_base + color::blue};
constexpr auto bright_magenta = SGR{bright_base + color::magenta};
constexpr auto bright_cyan    = SGR{bright_base + color::cyan};
constexpr auto bright_white   = SGR{bright_base + color::white};
// clang-format on

constexpr auto c8bit(int value) { return SGR{base + color::set, 5, value}; }
constexpr auto rgb(int r, int g, int b) {
  return SGR{base + color::set, 2, r, g, b};
}
constexpr auto rgb(uint hex) {
  return rgb((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
}

}  // namespace fg

namespace bg {

static constexpr int base = 40;
// clang-format off
constexpr auto black   = SGR{base + color::black};
constexpr auto red     = SGR{base + color::red};
constexpr auto green   = SGR{base + color::green};
constexpr auto yellow  = SGR{base + color::yellow};
constexpr auto blue    = SGR{base + color::blue};
constexpr auto magenta = SGR{base + color::magenta};
constexpr auto cyan    = SGR{base + color::cyan};
constexpr auto white   = SGR{base + color::white};

constexpr auto deflt   = SGR{base + color::deflt};
// clang-format on

static constexpr int bright_base = 100;
// clang-format off
constexpr auto bright_black   = SGR{bright_base + color::black};
constexpr auto bright_red     = SGR{bright_base + color::red};
constexpr auto bright_green   = SGR{bright_base + color::green};
constexpr auto bright_yellow  = SGR{bright_base + color::yellow};
constexpr auto bright_blue    = SGR{bright_base + color::blue};
constexpr auto bright_magenta = SGR{bright_base + color::magenta};
constexpr auto bright_cyan    = SGR{bright_base + color::cyan};
constexpr auto bright_white   = SGR{bright_base + color::white};
// clang-format on

constexpr auto c8bit(int value) { return SGR{base + color::set, value}; }
constexpr auto rgb(int r, int g, int b) {
  return SGR{base + color::set, 2, r, g, b};
}
constexpr auto rgb(uint hex) {
  return rgb((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
}

}  // namespace bg

}  // namespace ansi
