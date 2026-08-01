#include "statistics-graphics.hpp"
#include "color.hpp"
#include <array>
#include <format>
#include <iomanip>
#include <sstream>

namespace Statistics {
namespace Graphics {

std::string AskForPlayerNamePrompt() {
  constexpr auto score_prompt_text =
      "Please enter your name to save this score: ";
  constexpr auto sp = "  ";
  return std::format("{}{}{}{}", Color::bold_on, sp, score_prompt_text,
                     Color::bold_off);
}

std::string MessageScoreSavedPrompt() {
  constexpr auto score_saved_text = "Score saved!";
  constexpr auto sp = "  ";
  return std::format("\n{}{}{}{}{}{}\n", Color::green, Color::bold_on, sp,
                     score_saved_text, Color::bold_off, Color::def);
}

std::string TotalStatisticsOverlay(total_stats_display_data_t tsdd) {
  constexpr auto stats_title_text = "STATISTICS";
  constexpr auto divider_text = "──────────";
  constexpr auto header_border_text = "┌────────────────────┬─────────────┐";
  constexpr auto footer_border_text = "└────────────────────┴─────────────┘";
  constexpr auto no_save_text = "No saved statistics.";
  constexpr auto any_key_exit_text =
      "Press any key to return to the main menu... ";
  constexpr auto sp = "  ";

  enum TotalStatsDisplayDataFields {
    IDX_DATA_AVAILABLE,
    IDX_BEST_SCORE,
    IDX_GAME_COUNT,
    IDX_GAME_WIN_COUNT,
    IDX_TOTAL_MOVE_COUNT,
    IDX_TOTAL_DURATION,
    MAX_TOTALSTATSDISPLAYDATA_INDEXES
  };

  const auto stats_file_loaded = std::get<IDX_DATA_AVAILABLE>(tsdd);

  if (stats_file_loaded) {
    constexpr auto num_of_stats_attributes_text = 5;
    auto data_stats = std::array<std::string, num_of_stats_attributes_text>{
        std::get<IDX_BEST_SCORE>(tsdd), std::get<IDX_GAME_COUNT>(tsdd),
        std::get<IDX_GAME_WIN_COUNT>(tsdd),
        std::get<IDX_TOTAL_MOVE_COUNT>(tsdd),
        std::get<IDX_TOTAL_DURATION>(tsdd)};
    const auto stats_attributes_text = {
        "Best Score", "Game Count", "Number of Wins", "Total Moves Played",
        "Total Duration"};

    std::string result;
    result += std::format("{}{}{}{}{}{}\n", Color::green, Color::bold_on, sp,
                          stats_title_text, Color::bold_off, Color::def);
    result += std::format("{}{}{}{}{}{}\n", Color::green, Color::bold_on, sp,
                          divider_text, Color::bold_off, Color::def);
    result += std::format("{}{}\n", sp, header_border_text);

    for (auto i = 0u; i < data_stats.size(); ++i) {
      result += std::format(
          "{}│ {}{:<18}{} │ {:>11} │\n", sp, Color::bold_on,
          std::string{std::begin(stats_attributes_text)[i]}, Color::bold_off,
          data_stats[i]);
    }
    result += std::format("{}{}\n", sp, footer_border_text);
    result += "\n\n\n";
    result += std::format("{}{}", sp, any_key_exit_text);
    return result;
  }
  return std::format("{}{}\n\n\n{}{}", sp, no_save_text, sp, any_key_exit_text);
}

} // namespace Graphics
} // namespace Statistics