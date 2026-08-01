#include "global.hpp"
#include "color.hpp"
#include <format>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#  include <conio.h>

// Read one raw byte from the console without buffering or translation.
// `std::cin >> c` on Windows is line-buffered and skips whitespace, which
// makes it unreliable for reading non-ASCII bytes like the 0xE0 prefix that
// Windows uses for arrow keys / function keys.
void getKeypressDownInput(char &c) {
  c = static_cast<char>(_getch());
}

#else
#  include <termios.h>
#  include <unistd.h>

char getch() {
  char buf = 0;
  struct termios old = {0};
  if (tcgetattr(0, &old) < 0)
    perror("tcsetattr()");
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0)
    perror("tcsetattr ICANON");
  if (read(0, &buf, 1) < 0)
    perror("read()");
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0)
    perror("tcsetattr ~ICANON");
  return (buf);
}

void getKeypressDownInput(char &c) {
  c = getch();
}

#endif

void pause_for_keypress() {
  char c{};
  getKeypressDownInput(c);
}

void wait_for_any_letter_input(std::istream &is) {
  char c;
  is >> c;
}

void clearScreen() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
};

std::string secondsFormat(double sec) {
  const int total = static_cast<int>(sec);
  const int h = total / 3600;
  const int m = (total / 60) % 60;
  const int s = total % 60;
  std::string result;
  if (h) {
    result += std::format("{}h ", h);
  }
  if (m) {
    result += std::format("{}m ", m);
  }
  result += std::format("{}s", s);
  return result;
}
