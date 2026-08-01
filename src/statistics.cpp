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
  auto [stats_file_loaded, stats] =
      loadFromFileStatistics("../data/statistics.txt");
  return stats_file_loaded ? stats.bestScore : 0;
}

void saveEndGameStats(Scoreboard::Score finalscore) {
  auto [_, stats] = loadFromFileStatistics("../data/statistics.txt");
  stats.bestScore = std::max(stats.bestScore, finalscore.score);
  stats.gameCount++;
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