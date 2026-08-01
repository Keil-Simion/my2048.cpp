// 头文件保护：使用 `#pragma once` 替代传统的 `#ifndef/#define/#endif` 三件套。
// 理由：现代编译器（MSVC / GCC / Clang 均支持）下更简洁，且避免了宏名冲突
// 与"忘记写 #endif"等老问题；同时减少预处理时重复解析头文件的开销。
#pragma once

#include <string>
#include <tuple>

namespace Game {
struct GameBoard;

namespace Saver {
void saveGamePlayState(const GameBoard &gb, const std::string &filename);

} // namespace Saver
} // namespace Game
