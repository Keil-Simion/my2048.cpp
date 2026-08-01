// =====================================================================
//  statistics.cpp —— 游戏统计（best score / win count 等）的读写
// ---------------------------------------------------------------------
//  本次重构要点：
//    1) loadFromFileStatistics 由
//         if (statistics) { ... }
//         else return {false, {}};
//       改为更地道的 early-return 风格（避免一层 else）。
//    2) load_game_best_score 用结构化绑定 + 三元运算符：
//         auto [stats_file_loaded, stats] = loadFromFileStatistics(...);
//         return stats_file_loaded ? stats.bestScore : 0;
//       替代原本"先声明 stats_file_loaded / stats / tempscore 一堆临时
//       变量再做条件赋值"的写法，行数砍半，意图更直接。
//    3) saveEndGameStats 同步精简：
//         - bestScore 用 std::max 替代手写三元表达式；
//         - winCount += finalscore.win 直接累加 bool（true 增 1、false 不变）；
//         - 不再用 std::tie + 临时变量；改成结构化绑定 + std::ignore
//           别名。
//    4) 删除了冗余的 generateStatsFromInputData / generateFilefromStatsData
//       包装层（它们只是把流读写转发一层再返回 true，毫无价值）。
// =====================================================================

#include "statistics.hpp"
#include "color.hpp"
#include "scores-graphics.hpp"
#include "scores.hpp"
#include "statistics-graphics.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <sstream>

namespace Statistics {

namespace {

std::string receive_input_player_name(std::istream &is) {
  std::string name;
  is >> name;
  return name;
}

// 直接把 stats 写入文件；返回值改为 ofstream 的 bool 状态，
// 比"先写再 return true"更诚实（如果写入失败至少能反映）。
bool saveToFileEndGameStatistics(std::string filename, total_game_stats_t s) {
  std::ofstream filedata(filename);
  filedata << s;
  return static_cast<bool>(filedata);
}

Scoreboard::Graphics::finalscore_display_data_t
make_finalscore_display_data(Scoreboard::Score finalscore) {
  return {
      std::to_string(finalscore.score), std::to_string(finalscore.largestTile),
      std::to_string(finalscore.moveCount), secondsFormat(finalscore.duration)};
};

} // namespace

load_stats_status_t loadFromFileStatistics(std::string filename) {
  std::ifstream statistics(filename);
  if (!statistics) {
    return {false, {}};
  }
  total_game_stats_t stats;
  statistics >> stats;
  return {true, stats};
}

ull load_game_best_score() {
  // 结构化绑定 + 三元表达式：原本要写"先临时声明两个变量再按
  // 条件赋值"的 8 行代码，现在 2 行搞定。
  auto [stats_file_loaded, stats] =
      loadFromFileStatistics("../data/statistics.txt");
  return stats_file_loaded ? stats.bestScore : 0;
}

void saveEndGameStats(Scoreboard::Score finalscore) {
  // 不关心是否成功加载 —— 即使没读到旧值，也会从默认零值开始累加。
  auto [_, stats] = loadFromFileStatistics("../data/statistics.txt");
  stats.bestScore = std::max(stats.bestScore, finalscore.score);
  stats.gameCount++;
  // bool 隐式转 int：true → 1，false → 0，等价于原
  // "stats.winCount = finalscore.win ? stats.winCount + 1 : stats.winCount"。
  stats.winCount += finalscore.win;
  stats.totalMoveCount += finalscore.moveCount;
  stats.totalDuration += finalscore.duration;

  saveToFileEndGameStatistics("../data/statistics.txt", stats);
}

void CreateFinalScoreAndEndGameDataFile(std::ostream &os, std::istream &is,
                                        Scoreboard::Score finalscore) {
  const auto finalscore_display_data = make_finalscore_display_data(finalscore);
  DrawAlways(os, DataSuppliment(finalscore_display_data,
                                Scoreboard::Graphics::EndGameStatisticsPrompt));

  DrawAlways(os, Graphics::AskForPlayerNamePrompt);
  finalscore.name = receive_input_player_name(is);

  Scoreboard::saveScore(finalscore);
  saveEndGameStats(finalscore);
  DrawAlways(os, Graphics::MessageScoreSavedPrompt);
}

} // namespace Statistics

using namespace Statistics;

// 旧实现无变化：total_game_stats_t 的序列化格式与存档文件兼容。
std::istream &operator>>(std::istream &is, total_game_stats_t &s) {
  is >> s.bestScore >> s.gameCount >> s.winCount >> s.totalMoveCount >>
      s.totalDuration;
  return is;
}

std::ostream &operator<<(std::ostream &os, total_game_stats_t &s) {
  os << s.bestScore << "\n"
     << s.gameCount << "\n"
     << s.winCount << "\n"
     << s.totalMoveCount << "\n"
     << s.totalDuration;
  return os;
}