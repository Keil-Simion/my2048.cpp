// =====================================================================
//  game-graphics.cpp —— 游戏界面绘图函数
// ---------------------------------------------------------------------
//  本文件集中输出"游戏内"用到的所有提示文本（标题、胜/负提示、
//  输入错误、按键帮助、得分框等）。
//
//  本次重构的核心改动：
//    1) 把所有"用 std::ostringstream 链式 << 拼接 ANSI 转义序列 +
//       文本"的写法，改为 std::format + Color::xxx 的写法。
//       对比：
//         旧（典型样式）：
//           std::ostringstream oss;
//           oss << red << bold_on << title_card_2048 << bold_off << def;
//           oss << "\n\n\n";
//           return oss.str();
//         新：
//           return std::format("{}{}{}{}{}\n\n\n",
//                              Color::red, Color::bold_on, title_card_2048,
//                              Color::bold_off, Color::def);
//       改进点：
//         - 一步到位，少了"先建 ostringstream 再 str()"的开销；
//         - 颜色顺序在调用处一目了然（不再散落在多行 << 中）；
//         - 编译期可对格式串做静态校验（参数数量、类型匹配），少一类
//           "忘了写 bold_off"导致的隐性 bug；
//         - 显式 `Color::xxx` 写法符合本项目"避免 using namespace"的
//           现代编码风格（参见 archive 移除后的决定）。
//
//    2) 所有输入命令帮助文本（InputCommandListPrompt 等）同样改为
//       std::format 直接拼接，逻辑不变但更紧凑。
//
//    3) GameScoreBoardBox 的内层拼接改用 lambda + std::format，
//       把"右对齐填充空格"这段重复代码抽出来，更易读。
// =====================================================================

#include "game-graphics.hpp"
#include "color.hpp"
#include "global.hpp"
#include <format>
#include <sstream>
#include <string>
#include <tuple>

namespace Game {
namespace Graphics {

std::string AsciiArt2048() {
  constexpr auto title_card_2048 = R"(
     _   __ ______ ______ _____  __  __ ____  _       __
    / | / // ____//_  __// ___/ / / / // __ \| |     / /
   /  |/ // __/    / /   \__ \ / /_/ // / / /| | /| / /
  / /|  // /___   / /   ___/ // __  // /_/ / | |/ |/ /
 /_/ |_//_____/  /_/   /____//_/ /_/ \____/  |__/|__/
  )";
  // 旧写法：ostringstream 链式 <<
  // 新写法：std::format 把 ANSI 颜色与正文放在一个表达式里，可读性更好。
  return std::format("{}{}{}{}{}\n\n\n", Color::red, Color::bold_on,
                     title_card_2048, Color::bold_off, Color::def);
}

std::string BoardInputPrompt() {
  constexpr auto sp = "  ";
  // 注意：这里把一段长文本拆成两段（提示语 + 输入请求）拼到格式串里，
  // 等价于旧实现中"先输出提示语（带换行）再输出 Enter Choice"的串行结构。
  return std::format("{}{}{}{}{}{}", Color::bold_on, sp,
                     "(NOTE: Scores and statistics will be saved only for the "
                     "4x4 gameboard)\n",
                     sp, "Enter gameboard size - (Enter '0' to go back): ",
                     Color::bold_off);
}

std::string YouWinPrompt() {
  constexpr auto win_game_text = "You win! Congratulations!";
  constexpr auto sp = "  ";
  return std::format("{}{}{}{}{}{}\n\n\n", Color::green, Color::bold_on, sp,
                     win_game_text, Color::def, Color::bold_off);
}

std::string GameOverPrompt() {
  constexpr auto lose_game_text = "Game over! You lose.";
  constexpr auto sp = "  ";
  return std::format("{}{}{}{}{}{}\n\n\n", Color::red, Color::bold_on, sp,
                     lose_game_text, Color::def, Color::bold_off);
}

std::string EndOfEndlessPrompt() {
  constexpr auto endless_mode_text =
      "End of endless mode! Thank you for playing!";
  constexpr auto sp = "  ";
  return std::format("{}{}{}{}{}{}\n\n\n", Color::red, Color::bold_on, sp,
                     endless_mode_text, Color::def, Color::bold_off);
}

std::string QuestionEndOfWinningGamePrompt() {
  constexpr auto win_but_what_next =
      "You Won! Continue playing current game? [y/n]";
  constexpr auto sp = "  ";
  return std::format("{}{}{}{}{}{}: ", Color::green, Color::bold_on, sp,
                     win_but_what_next, Color::def, Color::bold_off);
}

std::string GameStateNowSavedPrompt() {
  constexpr auto state_saved_text =
      "The game has been saved. Feel free to take a break.";
  constexpr auto sp = "  ";
  return std::format("{}{}{}{}{}{}\n\n", Color::green, Color::bold_on, sp,
                     state_saved_text, Color::def, Color::bold_off);
}

std::string GameBoardNoSaveErrorPrompt() {
  constexpr auto no_save_found_text =
      "No saved game found. Starting a new game.";
  constexpr auto sp = "  ";
  return std::format("{}{}{}{}{}{}\n\n", Color::red, Color::bold_on, sp,
                     no_save_found_text, Color::def, Color::bold_off);
}

std::string InvalidInputGameBoardErrorPrompt() {
  constexpr auto invalid_prompt_text = "Invalid input. Please try again.";
  constexpr auto sp = "  ";
  return std::format("{}{}{}{}\n\n", Color::red, sp, invalid_prompt_text,
                     Color::def);
}

std::string BoardSizeErrorPrompt() {
  constexpr auto sp = "  ";
  // 旧写法使用 std::begin(invalid_prompt_text)[i] 来取数组元素，
  // 新写法直接把字面量写在格式串里，更直观。注意：
  //   {} 占位符数量必须 = 参数数量（编译期校验），这里 8 个 {} 对应 8 个参数。
  return std::format(
      "{}{}{}{}{}{}{}{}\n\n", Color::red, sp,
      "Invalid input. Gameboard size should range from ",
      static_cast<int>(GameBoardDimensions::MIN_GAME_BOARD_PLAY_SIZE),
      " to ",
      static_cast<int>(GameBoardDimensions::MAX_GAME_BOARD_PLAY_SIZE), ".",
      Color::def);
}

std::string InputCommandListPrompt() {
  constexpr auto sp = "  ";
  // 旧写法：循环 6 次，每次 str_os << sp << txt << "\n"。
  // 新写法：把 6 行拼成一个格式串，6 个 sp 占位（同样位置复用同一个变量）。
  return std::format("{}W or K or ↑ => Up\n{}A or H or ← => Left\n"
                     "{}S or J or ↓ => Down\n{}D or L or → => Right\n"
                     "{}Z or P => Save\n{}M => Return to menu\n",
                     sp, sp, sp, sp, sp, sp);
}

std::string EndlessModeCommandListPrompt() {
  constexpr auto sp = "  ";
  return std::format("{}{}\n", sp, "X => Quit Endless Mode");
}

std::string InputCommandListFooterPrompt() {
  constexpr auto sp = "  ";
  // 使用带索引的占位符 {0} 复用同一个 sp 参数（C++20 std::format 支持）。
  return std::format("{0} \n{0}Press the keys to start and continue.\n{0} \n\n", sp);
}

std::string GameScoreBoardBox(scoreboard_display_data_t scdd) {
  // * border padding: vvv
  // | l-outer: 2, r-outer: 0
  // | l-inner: 1, r-inner: 1
  // * top border / bottom border: vvv
  // | tl_corner + horizontal_sep + tr_corner = length: 1 + 27 + 1
  // | bl_corner + horizontal_sep + br_corner = length: 1 + 27 + 1
  enum {
    UI_SCOREBOARD_SIZE = 27,
    UI_BORDER_OUTER_PADDING = 2,
    UI_BORDER_INNER_PADDING = 1
  };
  constexpr auto score_text_label = "SCORE:";
  constexpr auto bestscore_text_label = "BEST SCORE:";
  constexpr auto moves_text_label = "MOVES:";
  constexpr auto border_padding_char = ' ';
  constexpr auto vertical_border_pattern = "│";
  constexpr auto top_board =
      "┌───────────────────────────┐"; // Multibyte character set
  constexpr auto bottom_board =
      "└───────────────────────────┘"; // Multibyte character set

  enum ScoreBoardDisplayDataFields {
    IDX_COMPETITION_MODE,
    IDX_GAMEBOARD_SCORE,
    IDX_BESTSCORE,
    IDX_MOVECOUNT,
    MAX_SCOREBOARDDISPLAYDATA_INDEXES
  };

  const auto competition_mode = std::get<IDX_COMPETITION_MODE>(scdd);
  const auto gameboard_score = std::get<IDX_GAMEBOARD_SCORE>(scdd);
  const auto temp_bestscore = std::get<IDX_BESTSCORE>(scdd);
  const auto movecount = std::get<IDX_MOVECOUNT>(scdd);

  const auto outer_border_padding =
      std::string(UI_BORDER_OUTER_PADDING, border_padding_char);
  const auto inner_border_padding =
      std::string(UI_BORDER_INNER_PADDING, border_padding_char);
  const auto inner_padding_length =
      UI_SCOREBOARD_SIZE - (inner_border_padding.length() * 2);

  // ---------------------------------------------------------------
  //  重构：抽出 pad_right 闭包
  //  旧代码在每行（SCORE / BEST SCORE / MOVES）都重复写一遍
  //  `bold_on + 标签 + bold_off + 填充空格 + 数值` 这种格式。
  //  抽成 lambda 后，三个分支共享一份格式逻辑，未来要改格式只改一处。
  // ---------------------------------------------------------------
  const auto pad_right = [&](const std::string &label,
                             const std::string &value) {
    const auto pad_count = inner_padding_length - label.length() - value.length();
    return std::format("{}{}{}{}{}{}", inner_border_padding, Color::bold_on,
                       label, Color::bold_off,
                       std::string(pad_count, border_padding_char), value);
  };

  std::string result;
  result += std::format("{}{}\n", outer_border_padding, top_board);
  result += std::format(
      "{}{}{}{}{}\n", outer_border_padding, vertical_border_pattern,
      pad_right(score_text_label, gameboard_score), inner_border_padding,
      vertical_border_pattern);
  if (competition_mode) {
    result += std::format(
        "{}{}{}{}{}\n", outer_border_padding, vertical_border_pattern,
        pad_right(bestscore_text_label, temp_bestscore), inner_border_padding,
        vertical_border_pattern);
  }
  result += std::format(
      "{}{}{}{}{}\n", outer_border_padding, vertical_border_pattern,
      pad_right(moves_text_label, movecount), inner_border_padding,
      vertical_border_pattern);
  result += std::format("{}{}\n \n", outer_border_padding, bottom_board);
  return result;
}

std::string GameScoreBoardOverlay(scoreboard_display_data_t scdd) {
  std::ostringstream str_os;
  DrawAlways(str_os, DataSuppliment(scdd, GameScoreBoardBox));
  return str_os.str();
}

std::string GameEndScreenOverlay(end_screen_display_data_t esdd) {
  enum EndScreenDisplayDataFields {
    IDX_FLAG_WIN,
    IDX_FLAG_ENDLESS_MODE,
    MAX_ENDSCREENDISPLAYDATA_INDEXES
  };
  const auto did_win = std::get<IDX_FLAG_WIN>(esdd);
  const auto is_endless_mode = std::get<IDX_FLAG_ENDLESS_MODE>(esdd);

  std::ostringstream str_os;
  const auto standardWinLosePrompt = [=] {
    std::ostringstream str_os;
    DrawOnlyWhen(str_os, did_win, YouWinPrompt);
    DrawOnlyWhen(str_os, !did_win, GameOverPrompt);
    return str_os.str();
  };
  DrawOnlyWhen(str_os, !is_endless_mode, standardWinLosePrompt);
  DrawOnlyWhen(str_os, is_endless_mode, EndOfEndlessPrompt);
  return str_os.str();
}

std::string GameInputControlsOverlay(input_controls_display_data_t gamestatus) {
  const auto is_in_endless_mode = std::get<0>(gamestatus);
  const auto is_in_question_mode = std::get<1>(gamestatus);
  std::ostringstream str_os;
  const auto InputControlLists = [=] {
    std::ostringstream str_os;
    DrawAlways(str_os, Graphics::InputCommandListPrompt);
    DrawOnlyWhen(str_os, is_in_endless_mode,
                 Graphics::EndlessModeCommandListPrompt);
    DrawAlways(str_os, Graphics::InputCommandListFooterPrompt);
    return str_os.str();
  };
  DrawOnlyWhen(str_os, !is_in_question_mode, InputControlLists);
  return str_os.str();
}

} // namespace Graphics
} // namespace Game