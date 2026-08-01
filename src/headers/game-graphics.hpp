// 头文件保护：使用 `#pragma once` 替代传统的 `#ifndef/#define/#endif` 三件套。
// 理由：现代编译器（MSVC / GCC / Clang 均支持）下更简洁，且避免了宏名冲突
// 与"忘记写 #endif"等老问题；同时减少预处理时重复解析头文件的开销。
#pragma once

#include <string>
#include <tuple>

enum class GameBoardDimensions {
  MIN_GAME_BOARD_PLAY_SIZE = 3,
  MAX_GAME_BOARD_PLAY_SIZE = 10
};
constexpr int COMPETITION_GAME_BOARD_PLAY_SIZE = 4;

namespace Game {
namespace Graphics {
std::string AsciiArt2048();
std::string BoardInputPrompt();
std::string YouWinPrompt();
std::string GameOverPrompt();
std::string EndOfEndlessPrompt();
std::string InvalidInputGameBoardErrorPrompt();
std::string QuestionEndOfWinningGamePrompt();
std::string GameStateNowSavedPrompt();
std::string GameBoardNoSaveErrorPrompt();
std::string BoardSizeErrorPrompt();
std::string InputCommandListPrompt();
std::string EndlessModeCommandListPrompt();
std::string InputCommandListFooterPrompt();
using scoreboard_display_data_t =
    std::tuple<bool, std::string, std::string, std::string>;
std::string GameScoreBoardBox(const scoreboard_display_data_t &scdd);
std::string GameScoreBoardOverlay(const scoreboard_display_data_t &scdd);
using end_screen_display_data_t = std::tuple<bool, bool>;
std::string GameEndScreenOverlay(end_screen_display_data_t esdd);
using input_controls_display_data_t = std::tuple<bool, bool>;
std::string GameInputControlsOverlay(input_controls_display_data_t gamestatus);
} // namespace Graphics
} // namespace Game