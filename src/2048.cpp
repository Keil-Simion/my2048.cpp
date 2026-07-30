#include "2048.hpp"
#include "menu.hpp"
#ifdef _WIN32
#  include <windows.h>
#endif

int main() {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
#endif
  Menu::startMenu();
  return 0;
}
