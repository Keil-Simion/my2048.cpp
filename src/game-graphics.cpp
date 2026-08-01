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
  return std::format("{}{}{}{}{}\n\n\n", Color::red, Color::bold_on,
                     title_card_2048, Color::bold_off, Color::def);
}

std::string BoardInputPrompt() {
  constexpr auto sp = "  ";
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