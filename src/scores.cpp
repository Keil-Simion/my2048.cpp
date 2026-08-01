#include "scores.hpp"
#include "color.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <sstream>

namespace {

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
  Scoreboard_t scoreList{};
  Score temp{};
  while (scores >> temp) {
    scoreList.push_back(temp);
  }
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