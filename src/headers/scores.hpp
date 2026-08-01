// 头文件保护：使用 `#pragma once` 替代传统的 `#ifndef/#define/#endif` 三件套。
// 理由：现代编译器（MSVC / GCC / Clang 均支持）下更简洁，且避免了宏名冲突
// 与"忘记写 #endif"等老问题；同时减少预处理时重复解析头文件的开销。
#pragma once

#include "global.hpp"
#include <iosfwd>
#include <string>
#include <tuple>
#include <vector>

namespace Scoreboard {
struct Score {
  std::string name;
  ull score;
  bool win;
  ull largestTile;
  long long moveCount;
  double duration;
};

bool operator>(const Score &a, const Score &b);

using Scoreboard_t = std::vector<Score>;
using load_score_status_t = std::tuple<bool, Scoreboard_t>;

// List of scores read until "exhausted".
// Note: returns a tuple containing a std::vector<Score> of all read scores.
load_score_status_t loadFromFileScore(std::string filename);
void saveScore(Score finalscore);
} // namespace Scoreboard

std::istream &operator>>(std::istream &is, Scoreboard::Score &s);
std::ostream &operator<<(std::ostream &os, Scoreboard::Score &s);
