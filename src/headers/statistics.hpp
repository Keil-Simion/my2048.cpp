// 头文件保护：使用 `#pragma once` 替代传统的 `#ifndef/#define/#endif` 三件套。
// 理由：现代编译器（MSVC / GCC / Clang 均支持）下更简洁，且避免了宏名冲突
// 与"忘记写 #endif"等老问题；同时减少预处理时重复解析头文件的开销。
#pragma once

#include "global.hpp"
#include <iosfwd>
#include <string>
#include <tuple>

namespace Scoreboard {
struct Score;
}

namespace Statistics {
struct total_game_stats_t {
  ull bestScore{};
  ull totalMoveCount{};
  int gameCount{};
  double totalDuration{};
  int winCount{};
};

using load_stats_status_t = std::tuple<bool, total_game_stats_t>;

load_stats_status_t loadFromFileStatistics(const std::string &filename);
ull load_game_best_score();
void saveEndGameStats(const Scoreboard::Score &finalscore);
void CreateFinalScoreAndEndGameDataFile(std::ostream &os, std::istream &is,
                                        Scoreboard::Score &finalscore);
} // namespace Statistics

std::istream &operator>>(std::istream &is, Statistics::total_game_stats_t &s);
std::ostream &operator<<(std::ostream &os, const Statistics::total_game_stats_t &s);
