#include "scores-graphics.hpp"
#include "color.hpp"
#include <array>
#include <format>
#include <sstream>

namespace Scoreboard {
namespace Graphics {

std::string ScoreboardOverlay(scoreboard_display_data_list_t sbddl) {
  constexpr auto no_save_text = "No saved scores.";
  constexpr auto header_border_text =
      "┌─────┬────────────────────┬──────────┬──────┬───────┬──────────────┬──────────────┐";
  constexpr auto mid_border_text =
      "├─────┼────────────────────┼──────────┼──────┼───────┼──────────────┼──────────────┤";
  constexpr auto bottom_border_text =
      "└─────┴────────────────────┴──────────┴──────┴───────┴──────────────┴──────────────┘";
  constexpr auto score_title_text = "SCOREBOARD";
  constexpr auto divider_text = "──────────";
  constexpr auto sp = "  ";

  std::string result;
  result += std::format("{}{}{}{}{}{}\n", Color::green, Color::bold_on, sp,
                        score_title_text, Color::bold_off, Color::def);
  result += std::format("{}{}{}{}{}{}\n", Color::green, Color::bold_on, sp,
                        divider_text, Color::bold_off, Color::def);

  if (sbddl.empty()) {
    result += std::format("{}{}\n\n\n", sp, no_save_text);
    return result;
  }

  result += std::format("{}{}\n", sp, header_border_text);
  result += std::format(
      "{}│ {}{:<3}{} │ {}{:<18}{} │ {}{:>8}{} │ {:>4} │ {:>5} │ {:>12} │ {:>12} │\n",
      sp, Color::bold_on, "No.", Color::bold_off, Color::bold_on, "Name",
      Color::bold_off, Color::bold_on, "Score", Color::bold_off, "Won?",
      "Moves", "Largest Tile", "Duration");
  result += std::format("{}{}\n", sp, mid_border_text);

  for (const auto &row : sbddl) {
    result += std::format(
        "{}│ {:>2}. │ {:<18} │ {:>8} │ {:>4} │ {:>5} │ {:>12} │ {:>12} │\n", sp,
        std::get<0>(row), std::get<1>(row), std::get<2>(row), std::get<3>(row),
        std::get<4>(row), std::get<5>(row), std::get<6>(row));
  }
  result += std::format("{}{}\n\n\n", sp, bottom_border_text);
  return result;
}

std::string EndGameStatisticsPrompt(finalscore_display_data_t finalscore) {
  constexpr auto stats_title_text = "STATISTICS";
  constexpr auto divider_text = "──────────";
  constexpr auto sp = "  ";

  enum FinalScoreDisplayDataFields {
    IDX_FINAL_SCORE_VALUE,
    IDX_LARGEST_TILE,
    IDX_MOVE_COUNT,
    IDX_DURATION,
    MAX_NUM_OF_FINALSCOREDISPLAYDATA_INDEXES
  };
  const auto data_stats =
      std::array<std::string, MAX_NUM_OF_FINALSCOREDISPLAYDATA_INDEXES>{
          std::get<IDX_FINAL_SCORE_VALUE>(finalscore),
          std::get<IDX_LARGEST_TILE>(finalscore),
          std::get<IDX_MOVE_COUNT>(finalscore),
          std::get<IDX_DURATION>(finalscore)};
  const auto stats_attributes_text = {"Final score:", "Largest Tile:",
                                      "Number of moves:", "Time taken:"};

  std::string result;
  result += std::format("{}{}{}{}\n", Color::yellow, sp, stats_title_text,
                        Color::def);
  result += std::format("{}{}{}{}\n", Color::yellow, sp, divider_text, Color::def);

  for (auto i = 0u; i < data_stats.size(); ++i) {
    result += std::format("{}{:<19}{}{}{}\n", sp,
                          std::string{std::begin(stats_attributes_text)[i]},
                          Color::bold_on, data_stats[i], Color::bold_off);
  }
  result += "\n\n";
  return result;
}

} // namespace Graphics
} // namespace Scoreboard