#include <array>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

#include "ansi.h"

int main(int, char **) {
  int clr = 0;
  ansi::SGR d;

  std::cerr << ansi::fg::blue << "Hello"
            << (ansi::bold | ansi::dbl_underline | ansi::invert)
            << " world\n";

  unsigned char f = 12;
  std::cerr << f << "\n";

  return 0;
}
