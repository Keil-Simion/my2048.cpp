// 头文件保护：使用 `#pragma once` 替代传统的 `#ifndef/#define/#endif` 三件套。
// 理由：现代编译器（MSVC / GCC / Clang 均支持）下更简洁，且避免了宏名冲突
// 与"忘记写 #endif"等老问题；同时减少预处理时重复解析头文件的开销。
#pragma once

#include <string>
#include <tuple>

namespace Game {
using load_gameboard_status_t = std::tuple<bool, struct GameBoard>;

namespace Loader {
load_gameboard_status_t load_GameBoard_data_from_file(std::string filename);

// Output: {[loadfile_ok_status], [decltype(gameboard.score)],
// [decltype(gameboard.moveCount)]}
std::tuple<bool, std::tuple<unsigned long long, long long>>
load_game_stats_from_file(std::string filename);
bool load_game(std::string filename, GameBoard& gb);

} // namespace Loader
} // namespace Game
