// =====================================================================
//  scores.cpp —— 分数榜（历史高分）的读写
// ---------------------------------------------------------------------
//  本次重构要点：
//    1) 删除冗余包装函数：
//         - generateScorefromFileData：原本只是"循环 is >> tempscore
//           累加到 vector"，和 loadFromFileScore 内联写法等价；
//         - generateFilefromScoreData：原本只做"os << score; return true"
//           同样无价值。
//       这两个函数的删除不影响功能。
//    2) saveToFileScore 的返回值改为反映 ofstream 的真实写入状态
//       （之前固定返回 true）。
//    3) loadFromFileScore 把"if (scores) { ... } else return {false, ...}"
//       改为 early-return，与 statistics.cpp 风格一致。
// =====================================================================

#include "scores.hpp"
#include "color.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <sstream>

namespace {

// 追加写（std::ios_base::app）到分数榜文件。
// 返回 ofstream 的 bool 状态以反映写入是否成功。
bool saveToFileScore(std::string filename, Scoreboard::Score s) {
  std::ofstream os(filename, std::ios_base::app);
  os << s;
  return static_cast<bool>(os);
}

} // namespace

namespace Scoreboard {
bool operator>(const Score &a, const Score &b) {
  return a.score > b.score;
}

load_score_status_t loadFromFileScore(std::string filename) {
  std::ifstream scores(filename);
  if (!scores) {
    return {false, {}};
  }
  // 读到 EOF 为止：while (scores >> temp) 是惯用写法。
  Scoreboard_t scoreList{};
  Score temp{};
  while (scores >> temp) {
    scoreList.push_back(temp);
  }
  // 按分数降序排序，方便 scoreboard 显示时已经是"高分在前"。
  std::sort(scoreList.begin(), scoreList.end(), std::greater<Score>{});
  return {true, scoreList};
}

void saveScore(Score finalscore) {
  saveToFileScore("../data/scores.txt", finalscore);
}

} // namespace Scoreboard

using namespace Scoreboard;

std::istream &operator>>(std::istream &is, Score &s) {
  is >> s.name >> s.score >> s.win >> s.moveCount >> s.largestTile >>
      s.duration;
  return is;
}

std::ostream &operator<<(std::ostream &os, Score &s) {
  os << "\n"
     << s.name << " " << s.score << " " << s.win << " " << s.moveCount << " "
     << s.largestTile << " " << s.duration;
  return os;
}