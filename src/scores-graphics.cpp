// =====================================================================
//  scores-graphics.cpp —— 分数榜 / 终局统计的绘制
// ---------------------------------------------------------------------
//  本次重构要点：
//    1) ScoreboardOverlay：把表头循环 7 次
//         std::begin(score_attributes_text)[i]
//       改为在格式串里直接列 7 个字段名 + 显式宽度。
//       等价输出，但格式串里一行的对齐一目了然。
//    2) 表内每一行的循环也改用 std::format 的对齐语法（{:<18} /
//       {:>8} 等），替代旧的 std::setw + std::left / std::right 切换。
//    3) EndGameStatisticsPrompt 同理：lambda + counter 改为 for + 索引。
//    4) 颜色统一用 Color::xxx 显式调用。
// =====================================================================

#include "scores-graphics.hpp"
#include "color.hpp"
#include <array>
#include <format>
#include <sstream>

namespace Scoreboard {
namespace Graphics {

std::string ScoreboardOverlay(const scoreboard_display_data_list_t &sbddl) {
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
  // 表头：颜色在三个文字字段上切换（No. / Name / Score），其余直接输出。
  // 对齐语法 {:<18} 左对齐宽度 18，{:>8} 右对齐宽度 8，依次类推。
  result += std::format(
      "{}│ {}{:<3}{} │ {}{:<18}{} │ {}{:>8}{} │ {:>4} │ {:>5} │ {:>12} │ {:>12} │\n",
      sp, Color::bold_on, "No.", Color::bold_off, Color::bold_on, "Name",
      Color::bold_off, Color::bold_on, "Score", Color::bold_off, "Won?",
      "Moves", "Largest Tile", "Duration");
  result += std::format("{}{}\n", sp, mid_border_text);

  for (const auto &row : sbddl) {
    // 数据行：每个字段单独 std::get<i>(row) 取出。tuple 解构可读，
    // 也方便日后调整列顺序（只需调整 std::get 索引）。
    result += std::format(
        "{}│ {:>2}. │ {:<18} │ {:>8} │ {:>4} │ {:>5} │ {:>12} │ {:>12} │\n", sp,
        std::get<0>(row), std::get<1>(row), std::get<2>(row), std::get<3>(row),
        std::get<4>(row), std::get<5>(row), std::get<6>(row));
  }
  result += std::format("{}{}\n\n\n", sp, bottom_border_text);
  return result;
}

std::string EndGameStatisticsPrompt(const finalscore_display_data_t &finalscore) {
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
  // 黄色作为这一节的强调色。
  result += std::format("{}{}{}{}\n", Color::yellow, sp, stats_title_text,
                        Color::def);
  result += std::format("{}{}{}{}\n", Color::yellow, sp, divider_text, Color::def);

  for (auto i = 0u; i < data_stats.size(); ++i) {
    // 属性名左对齐 19 列；数值加粗。
    result += std::format("{}{:<19}{}{}{}\n", sp,
                          std::string{std::begin(stats_attributes_text)[i]},
                          Color::bold_on, data_stats[i], Color::bold_off);
  }
  result += "\n\n";
  return result;
}

} // namespace Graphics
} // namespace Scoreboard