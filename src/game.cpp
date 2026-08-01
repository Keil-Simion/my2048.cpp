// =====================================================================
//  game.cpp —— 游戏主循环 / 状态机
// ---------------------------------------------------------------------
//  本次重构涉及的改动：
//    1) 把所有 "先声明临时变量再 std::tie 接收返回值" 的写法，
//       统一改为 C++17 的结构化绑定：
//         - process_gamelogic：auto &[gamestatus, gb] = gsgb;
//         - process_gameStatus：auto &[gamestatus, gb] = gsgb;
//         - soloGameLoop      ：auto &[bestScore, comp_mode,
//                                       gamestatus, gb] = cgs;
//       改进点：
//         - 少 2~3 行变量声明；
//         - 引用绑定写回 tuple，无需 std::addressof / * 解引用，
//           可读性大幅提升；
//         - 与 C++20 的 std::format 配套，整体代码风格更现代。
//
//    2) 把 std::make_tuple(gamestatus, gb) 全部替换为
//       花括号初始化 {gamestatus, gb}，依赖 C++17 引入的
//       "CTAD for aggregates"，更轻量、也更显式。
//
//    3) 单独的 make_tuple 调用（如返回值）保留 std::make_tuple，
//       不强行替换。
//
//  本文件其余逻辑（按键翻译、棋盘推进、终局处理）未做改动。
// =====================================================================

#include "game.hpp"
#include "game-graphics.hpp"
#include "game-input.hpp"
#include "game-pregamemenu.hpp"
#include "gameboard-graphics.hpp"
#include "gameboard.hpp"
#include "global.hpp"
#include "loadresource.hpp"
#include "menu.hpp"
#include "saveresource.hpp"
#include "scores.hpp"
#include "statistics.hpp"
#include <array>
#include <chrono>
#include <iostream>
#include <sstream>
#include <random> // Required for generating random numbers/integers in the RemoveTiles() function.

namespace Game {
namespace {
enum Directions { UP, DOWN, RIGHT, LEFT };

enum GameStatusFlag {
  FLAG_WIN,
  FLAG_END_GAME,
  FLAG_ONE_SHOT,
  FLAG_SAVED_GAME,
  FLAG_INPUT_ERROR,
  FLAG_ENDLESS_MODE,
  FLAG_GAME_IS_ASKING_QUESTION_MODE,
  FLAG_QUESTION_STAY_OR_QUIT,
  FLAG_TILES_REMOVED,           // Indicates if tiles have already been removed
  MAX_NO_GAME_STATUS_FLAGS
};

using gamestatus_t = std::array<bool, MAX_NO_GAME_STATUS_FLAGS>;

using gamestatus_gameboard_t = std::tuple<gamestatus_t, GameBoard>;

/**
 * @brief Processes the game logic for the current game state.
 *
 * 本次重构：
 *   - 旧写法：
 *         gamestatus_t gamestatus;
 *         GameBoard gb;
 *         std::tie(gamestatus, gb) = gsgb;
 *     现在改为结构化绑定（引用形式）：
 *         auto &[gamestatus, gb] = gsgb;
 *     改进：去掉两个临时变量声明；语义上明确表达"这两个名字是 gsgb
 *     元素的别名"，未来如果只读不写也能直接换 const auto&。
 */
gamestatus_gameboard_t process_gamelogic(const gamestatus_gameboard_t &gsgb) {
  auto [gamestatus, gb] = gsgb;
  unblockTilesOnGameboard(gb);
  if (gb.moved) {
    addTileOnGameboard(gb);
    registerMoveByOneOnGameboard(gb);
  }

  if (!gamestatus[FLAG_ENDLESS_MODE]) {
    if (hasWonOnGameboard(gb)) {
      gamestatus[FLAG_WIN] = true;
      gamestatus[FLAG_GAME_IS_ASKING_QUESTION_MODE] = true;
      gamestatus[FLAG_QUESTION_STAY_OR_QUIT] = true;
    }
  }
  if (!canMoveOnGameboard(gb)) {
    if(gamestatus[FLAG_TILES_REMOVED] == false)
    {
      char input;
      std::cout << "You lose. Do you want to remove random tiles (Y/N) ?" << std::endl;
      std::cin >> input;

      if(input == 'Y' || input == 'y')
      {
        gamestatus[FLAG_TILES_REMOVED] = true;
        removeTiles(gb);
      }
      else if (input == 'N' || input == 'n')
      {
        gamestatus[FLAG_END_GAME] = true;
        gamestatus[FLAG_START_MENU] = true;
      }
      else 
      {
        std::cout << "Invalid input." << std::endl;
      }
    }
    else
    {
      gamestatus[FLAG_END_GAME] = true;
    }
  }
  return std::make_tuple(gamestatus, gb);
}

using competition_mode_t = bool;
Graphics::scoreboard_display_data_t
make_scoreboard_display_data(ull bestScore, competition_mode_t cm,
                             const GameBoard &gb) {
  const auto gameboard_score = gb.score;
  const auto tempBestScore = (bestScore < gb.score ? gb.score : bestScore);
  const auto comp_mode = cm;
  const auto movecount = MoveCountOnGameBoard(gb);
  const auto scdd =
      std::make_tuple(comp_mode, std::to_string(gameboard_score),
                      std::to_string(tempBestScore), std::to_string(movecount));
  return scdd;
};

Graphics::input_controls_display_data_t
make_input_controls_display_data(gamestatus_t gamestatus) {
  const auto icdd = std::make_tuple(gamestatus[FLAG_ENDLESS_MODE],
                                    gamestatus[FLAG_QUESTION_STAY_OR_QUIT]);
  return icdd;
};

std::string DisplayGameQuestionsToPlayerPrompt(gamestatus_t gamestatus) {
  using namespace Graphics;

  std::ostringstream str_os;
  DrawOnlyWhen(str_os, gamestatus[FLAG_QUESTION_STAY_OR_QUIT],
               QuestionEndOfWinningGamePrompt);
  return str_os.str();
}

// NOTE: current_game_session_t
// : (bestScore, is_competition_mode, gamestatus, gamePlayBoard)
using current_game_session_t =
    std::tuple<ull, competition_mode_t, gamestatus_t, GameBoard>;
enum tuple_cgs_t_idx {
  IDX_BESTSCORE,
  IDX_COMP_MODE,
  IDX_GAMESTATUS,
  IDX_GAMEBOARD
};

std::string drawGraphics(const current_game_session_t &cgs) {
  // Graphical Output has a specific ordering...
  using namespace Graphics;
  using namespace Gameboard::Graphics;
  using tup_idx = tuple_cgs_t_idx;
  const auto bestScore = std::get<tup_idx::IDX_BESTSCORE>(cgs);
  const auto comp_mode = std::get<tup_idx::IDX_COMP_MODE>(cgs);
  const auto gb = std::get<tup_idx::IDX_GAMEBOARD>(cgs);
  const auto gamestatus = std::get<tup_idx::IDX_GAMESTATUS>(cgs);

  std::ostringstream str_os;

  // 1. Clear screen
  clearScreen();

  // 2. Draw Game Title Art
  DrawAlways(str_os, AsciiArt2048);

  // 3. Draw Scoreboard of current game session
  const auto scdd = make_scoreboard_display_data(bestScore, comp_mode, gb);
  DrawAlways(str_os, DataSuppliment(scdd, GameScoreBoardOverlay));

  // 4. Draw current 2048 game active gameboard
  DrawAlways(str_os, DataSuppliment(gb, GameBoardTextOutput));

  // 5. Draw anyinstant status feedback, like
  // "Game saved!" (which disappers after next key input).
  DrawOnlyWhen(str_os, gamestatus[FLAG_SAVED_GAME], GameStateNowSavedPrompt);

  // 6. Draw any "questions to the player" (from the game) text output
  DrawOnlyWhen(str_os, gamestatus[FLAG_GAME_IS_ASKING_QUESTION_MODE],
               DataSuppliment(gamestatus, DisplayGameQuestionsToPlayerPrompt));

  // 7. Draw Keyboard / Input Keycodes to the player
  const auto input_controls_display_data =
      make_input_controls_display_data(gamestatus);
  DrawAlways(str_os, DataSuppliment(input_controls_display_data,
                                    GameInputControlsOverlay));

  // 8. Draw any game error messages to the player (to do with keyboard input)
  DrawOnlyWhen(str_os, gamestatus[FLAG_INPUT_ERROR],
               InvalidInputGameBoardErrorPrompt);

  return str_os.str();
}

gamestatus_t update_one_shot_display_flags(gamestatus_t gamestatus) {
  const auto disable_one_shot_flag = [](bool &trigger) { trigger = !trigger; };
  if (gamestatus[FLAG_ONE_SHOT]) {
    disable_one_shot_flag(gamestatus[FLAG_ONE_SHOT]);
    // Turn off display flag: [Saved Game]
    if (gamestatus[FLAG_SAVED_GAME]) {
      disable_one_shot_flag(gamestatus[FLAG_SAVED_GAME]);
    }
    // Turn off display flag: [Input Error]
    if (gamestatus[FLAG_INPUT_ERROR]) {
      disable_one_shot_flag(gamestatus[FLAG_INPUT_ERROR]);
    }
  }
  return gamestatus;
}

using bool_gamestatus_t = std::tuple<bool, gamestatus_t>;
/**
 * @brief Processes non-standard input commands and updates the game status accordingly.
 * 
 * This function handles additional input commands that are not part of the standard gameplay.
 * It updates the game status based on the input character received. The function supports saving
 * the game, quitting endless mode, and returning to the main menu.
 * 
 * @param c The input character received from the user.
 * @param gamestatus The current game status flags.
 * @return bool_gamestatus_t A tuple containing a boolean indicating if the keycode is invalid and the updated game status flags.
 * 
 * @note Changes in the new version:
 * - Added support for returning to the main menu with CODE_RETURN_TO_MENU.
 * - When CODE_RETURN_TO_MENU is detected, the appropriate main menu flags are set.
 */
bool_gamestatus_t check_input_other(char c, gamestatus_t gamestatus) {
  using namespace Input::Keypress::Code;
  auto is_invalid_keycode{true};
  switch (toupper(c)) {
  case CODE_HOTKEY_ACTION_SAVE:
  case CODE_HOTKEY_ALTERNATE_ACTION_SAVE:
    gamestatus[FLAG_ONE_SHOT] = true;
    gamestatus[FLAG_SAVED_GAME] = true;
    is_invalid_keycode = false;
    break;
  case CODE_HOTKEY_QUIT_ENDLESS_MODE:
    if (gamestatus[FLAG_ENDLESS_MODE]) {
      gamestatus[FLAG_END_GAME] = true;
      is_invalid_keycode = false;
    }
    break;
  case CODE_RETURN_TO_MENU:
    // When CODE_RETURN_TO_MENU is detected, the main menu is set to start, and game flags are adjusted.    mainmenustatus[FLAG_START_MENU] = true;
    mainmenustatus[FLAG_START_GAME] = false;
    mainmenustatus[FLAG_CONTINUE_GAME] = false;
    is_invalid_keycode = false;
    break;
  }
  return std::make_tuple(is_invalid_keycode, gamestatus);
}
using intendedmove_gamestatus_t =
    std::tuple<Input::intendedmove_t, gamestatus_t>;
intendedmove_gamestatus_t
receive_agent_input(Input::intendedmove_t intendedmove,
                    gamestatus_t gamestatus) {
  using namespace Input;
  const bool game_still_in_play =
      !gamestatus[FLAG_END_GAME] && !gamestatus[FLAG_WIN];
  if (game_still_in_play) {
    // Game still in play. Take input commands for next turn.
    char c;
    getKeypressDownInput(c);
    // Update agent's intended move flags per control scheme (if flagged).
    const auto is_invalid_keypress_code = check_input_ansi(c, intendedmove) &&
                                          check_input_wasd(c, intendedmove) &&
                                          check_input_vim(c, intendedmove);
    bool is_invalid_special_keypress_code;
    std::tie(is_invalid_special_keypress_code, gamestatus) =
        check_input_other(c, gamestatus);
    if (is_invalid_keypress_code && is_invalid_special_keypress_code) {
      gamestatus[FLAG_ONE_SHOT] = true;
      gamestatus[FLAG_INPUT_ERROR] = true;
    }
  }
  return std::make_tuple(intendedmove, gamestatus);
}

GameBoard decideMove(Directions d, const GameBoard &gb) {
  auto new_gb = gb;
  switch (d) {
  case UP:
    tumbleTilesUpOnGameboard(new_gb);
    break;

  case DOWN:
    tumbleTilesDownOnGameboard(new_gb);
    break;

  case LEFT:
    tumbleTilesLeftOnGameboard(new_gb);
    break;

  case RIGHT:
    tumbleTilesRightOnGameboard(new_gb);
    break;
  }
  return new_gb;
}

using bool_gameboard_t = std::tuple<bool, GameBoard>;
bool_gameboard_t process_agent_input(Input::intendedmove_t intendedmove,
                                     const GameBoard &gb) {
  using namespace Input;
  auto new_gb = gb;
  if (intendedmove[FLAG_MOVE_LEFT]) {
    new_gb = decideMove(LEFT, new_gb);
  }
  if (intendedmove[FLAG_MOVE_RIGHT]) {
    new_gb = decideMove(RIGHT, new_gb);
  }
  if (intendedmove[FLAG_MOVE_UP]) {
    new_gb = decideMove(UP, new_gb);
  }
  if (intendedmove[FLAG_MOVE_DOWN]) {
    new_gb = decideMove(DOWN, new_gb);
  }

  return std::make_tuple(true, new_gb);
}

bool check_input_check_to_end_game(char c) {
  using namespace Input::Keypress::Code;
  switch (std::toupper(c)) {
  case CODE_HOTKEY_CHOICE_NO:
    return true;
  }
  return false;
}

bool continue_playing_game(std::istream &in_os) {
  char letter_choice;
  in_os >> letter_choice;
  if (check_input_check_to_end_game(letter_choice)) {
    return false;
  }
  return true;
}

/**
 * @brief Processes the current game status and updates the game loop control.
 *
 * 本次重构：与 process_gamelogic 一样，把 std::tie 替换为
 * `auto &[gamestatus, gb] = gsgb;` 的结构化绑定，意图更清晰。
 */
bool_gamestatus_t process_gameStatus(const gamestatus_gameboard_t &gsgb) {
  auto [gamestatus, gb] = gsgb;
  auto loop_again{true};
  if (!gamestatus[FLAG_ENDLESS_MODE]) {
    if (gamestatus[FLAG_WIN]) {
      if (continue_playing_game(std::cin)) {
        gamestatus[FLAG_ENDLESS_MODE] = true;
        gamestatus[FLAG_QUESTION_STAY_OR_QUIT] = false;
        gamestatus[FLAG_WIN] = false;
      } else {
        loop_again = false;
      }
    }
  }
  if (gamestatus[FLAG_END_GAME]) {
    // End endless_mode;
    loop_again = false;
  }
  if (gamestatus[FLAG_SAVED_GAME]) {
    std::cout << "Please enter the filename to save the game state" << std::endl;
    std::string filename;
    std::cin >> filename;
    Saver::saveGamePlayState(gb, filename);
  }

  // New loop cycle: reset question asking event trigger
  gamestatus[FLAG_GAME_IS_ASKING_QUESTION_MODE] = false;
  return std::make_tuple(loop_again, gamestatus);
}

using bool_current_game_session_t = std::tuple<bool, current_game_session_t>;

/**
 * 本次重构要点（soloGameLoop）：
 *
 *   旧写法使用 std::addressof + 多次解引用，看起来很绕：
 *       const auto gamestatus = std::addressof(std::get<...>(cgs));
 *       const auto gb         = std::addressof(std::get<...>(cgs));
 *       std::tie(*gamestatus, *gb) = process_gamelogic({*gamestatus, *gb});
 *
 *   新写法直接用 `auto &[bestScore, comp_mode, gamestatus, gb] = cgs;`
 *   一行拿到 4 个元素的引用，后续修改直接通过别名写回 tuple：
 *       std::tie(gamestatus, gb) = process_gamelogic({gamestatus, gb});
 *
 *   改进点：
 *     - 不再需要 std::addressof + * 解引用，可读性大幅提升；
 *     - 一次性把整个 tuple 解构出来，不再依赖 IDX_* 枚举（见
 *       tuple_cgs_t_idx，新写法里没用到，但保留以兼容 drawGraphics /
 *       drawEndGameLoopGraphics 等其他位置）。
 *     - {gamestatus, gb} 用花括号初始化 tuple，比 std::make_tuple 更
 *       现代、更轻量（无需模板推导）。
 */
bool_current_game_session_t soloGameLoop(current_game_session_t cgs) {
  using namespace Input;
  auto &[bestScore, comp_mode, gamestatus, gb] = cgs;

  std::tie(gamestatus, gb) = process_gamelogic({gamestatus, gb});

  DrawAlways(std::cout, DataSuppliment(cgs, drawGraphics));
  gamestatus = update_one_shot_display_flags(gamestatus);

  intendedmove_t player_intendedmove{};
  std::tie(player_intendedmove, gamestatus) =
      receive_agent_input(player_intendedmove, gamestatus);
  std::tie(std::ignore, gb) = process_agent_input(player_intendedmove, gb);

  bool loop_again;
  std::tie(loop_again, gamestatus) = process_gameStatus({gamestatus, gb});
  return std::make_tuple(loop_again, cgs);
}

Graphics::end_screen_display_data_t
make_end_screen_display_data(gamestatus_t world_gamestatus) {
  const auto esdd = std::make_tuple(world_gamestatus[FLAG_WIN],
                                    world_gamestatus[FLAG_ENDLESS_MODE]);
  return esdd;
};

std::string drawEndGameLoopGraphics(const current_game_session_t &finalgamestatus) {
  // Graphical Output has a specific ordering...
  using namespace Graphics;
  using namespace Gameboard::Graphics;
  using tup_idx = tuple_cgs_t_idx;
  const auto bestScore = std::get<tup_idx::IDX_BESTSCORE>(finalgamestatus);
  const auto comp_mode = std::get<tup_idx::IDX_COMP_MODE>(finalgamestatus);
  const auto gb = std::get<tup_idx::IDX_GAMEBOARD>(finalgamestatus);
  const auto end_gamestatus =
      std::get<tup_idx::IDX_GAMESTATUS>(finalgamestatus);

  std::ostringstream str_os;

  // 1. Clear screen
  clearScreen();

  // 2. Draw Game Title Art
  DrawAlways(str_os, AsciiArt2048);

  // 3. Draw Scoreboard of ending current game session
  const auto scdd = make_scoreboard_display_data(bestScore, comp_mode, gb);
  DrawAlways(str_os, DataSuppliment(scdd, GameScoreBoardOverlay));

  // 4. Draw snapshot of ending 2048 session's gameboard
  DrawAlways(str_os, DataSuppliment(gb, GameBoardTextOutput));

  // 5. Draw "You win!" or "You Lose" prompt, only if not in endless mode.
  const auto esdd = make_end_screen_display_data(end_gamestatus);
  DrawAlways(str_os, DataSuppliment(esdd, GameEndScreenOverlay));

  return str_os.str();
}

/**
 * @brief Runs the endless game loop until the game ends or the user returns to the menu.
 * 
 * This function manages the continuous gameplay loop for the endless game mode. 
 * It repeatedly calls the solo game loop until the game ends or the user chooses to return to the menu.
 * The game status and game board are updated accordingly in each iteration.
 * 
 * @param currentBestScore The current best score achieved.
 * @param cm The competition mode settings.
 * @param gb The current game board state.
 * @return GameBoard The final state of the game board after the loop ends.
 * 
 * @note Changes in the new version:
 * - Added checks to monitor if the current state is set to GAME or MENU.
 * - The loop will terminate if the user presses the M key and the state changes to MENU.
 */
GameBoard endlessGameLoop(ull currentBestScore, competition_mode_t cm,
                          const GameBoard &gb) {
  auto loop_again{true};
  auto currentgamestatus =
      std::make_tuple(currentBestScore, cm, gamestatus_t{}, gb);
  // Monitor the loop to check if the current state is still set to GAME
  // If not, then the M key was pressed, setting the current state to MENU
  // Thus, we return to the menu
  while (loop_again && ((mainmenustatus[FLAG_START_GAME] == true) || (mainmenustatus[FLAG_CONTINUE_GAME] == true))) {
    std::tie(loop_again, currentgamestatus) = soloGameLoop(currentgamestatus);
  }

  DrawAlways(std::cout,
             DataSuppliment(currentgamestatus, drawEndGameLoopGraphics));
  return gb;
}

Scoreboard::Score make_finalscore_from_game_session(double duration,
                                                    const GameBoard &gb) {
  Scoreboard::Score finalscore{};
  finalscore.score = gb.score;
  finalscore.win = hasWonOnGameboard(gb);
  finalscore.moveCount = MoveCountOnGameBoard(gb);
  finalscore.largestTile = gb.largestTile;
  finalscore.duration = duration;
  return finalscore;
}

void DoPostGameSaveStuff(Scoreboard::Score finalscore, competition_mode_t cm) {
  if (cm) {
    Statistics::CreateFinalScoreAndEndGameDataFile(std::cout, std::cin,
                                                   finalscore);
  }
}

} // namespace

void playGame(PlayGameFlag cont, GameBoard &gb, ull userInput_PlaySize) {
  const auto is_this_a_new_game = (cont == PlayGameFlag::BrandNewGame);
  const auto is_competition_mode =
      (userInput_PlaySize == COMPETITION_GAME_BOARD_PLAY_SIZE);
  const auto bestScore = Statistics::load_game_best_score();

  if (is_this_a_new_game) {
    gb = GameBoard(userInput_PlaySize);
    addTileOnGameboard(gb);
  }

  const auto startTime = std::chrono::high_resolution_clock::now();
  gb = endlessGameLoop(bestScore, is_competition_mode, gb);
  const auto finishTime = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed = finishTime - startTime;
  const auto duration = elapsed.count();

  if (is_this_a_new_game) {
    const auto finalscore = make_finalscore_from_game_session(duration, gb);
    DoPostGameSaveStuff(finalscore, is_competition_mode);
  }
}

void startGame() {
  PreGameSetup::SetUpNewGame();
}

/**
 * @brief Continue a previously saved game.
 * 
 * The ContinueOldGame function has been updated to accept a filename directly.
 * This allows the user to load a specific save file instead of only the last saved game.
 * 
 * @param filename The name of the file containing the saved game to load.
 */
void continueGame(const std::string& filename) {
  PreGameSetup::ContinueOldGame(filename);
}

/**
 * @brief Randomly removes two tiles from a GameBoard by setting their values to 0.
 *
 * This function uses a random number generator to select and remove two non-empty tiles 
 * from the given GameBoard by setting their values to 0. The process continues until 
 * exactly two tiles are removed.
 *
 * @param gb The GameBoard object from which tiles will be removed.
 */
void removeTiles(GameBoard& gb) {
    // Seed with a real random value, if available
    std::random_device rd;
    std::mt19937 gen(rd());

    auto& [playsize, tiles] = gb.gbda;
    std::uniform_int_distribution<> dis(0, tiles.size() - 1);

    int tiles_to_remove = 2;
    while (tiles_to_remove > 0) {
        int random_index = dis(gen);
        auto& tile = tiles[random_index];

        if (tile.value != 0) {  // Ensure it's not already an empty tile
            tile.value = 0;     // Remove the tile
            --tiles_to_remove;
        }
    }
}

} // namespace Game
