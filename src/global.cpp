// =====================================================================
//  global.cpp —— 全局工具函数（按键输入、清屏、时间格式化）
// ---------------------------------------------------------------------
//  本次重构涉及两处重要改动：
//
//  1) Windows 下 getKeypressDownInput 改用 _getch()（替代 std::cin >> c）。
//     旧实现问题：
//       - std::cin >> c 是行缓冲的，要求先按 Enter；
//       - 会跳过空白字符，对非 ASCII 字节（如 0xE0，Windows 方向键前缀）
//         不可靠；
//       - 在游戏循环里会造成"按一下方向键要回车才生效"的糟糕体验。
//     新实现直接读原始字节（_getch），无缓冲、无转换。
//
//  2) secondsFormat 改用 std::format，去掉 ostringstream：
//     旧实现是手动算 h / m / s 后链式 << 拼到 ostringstream；
//     新实现一气呵成，可读性更好。注意 int 截断行为保持不变（仍是
//     整秒向下取整）。
// =====================================================================

#include "global.hpp"
#include "color.hpp"
#include <format>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#  include <conio.h>

// 直接从控制台读取原始字节，无行缓冲、无空白跳过。
// 配合 game-input.cpp 中处理 0xE0 / 0x00 前缀的逻辑，可以正确读到
// Windows 方向键 / 功能键的完整序列。
void getKeypressDownInput(char &c) {
  c = static_cast<char>(_getch());
}

#else
#  include <termios.h>
#  include <unistd.h>

// POSIX 实现：通过临时关闭 ICANON / ECHO 进入"原始"输入模式，
// 一次 read() 一个字节。和 Windows 的 _getch() 行为对齐。
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

// ---------------------------------------------------------------------
//  secondsFormat —— 把秒数格式化为 "Xh Ym Zs"
//  旧写法：ostringstream + 多次 <<，先算 h/m/s 再分类拼接。
//  新写法：一次到位，使用 std::format 的 "{}h " 等格式片段；
//  等价行为：h/m 仅在 > 0 时输出，s 永远输出。
// ---------------------------------------------------------------------
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