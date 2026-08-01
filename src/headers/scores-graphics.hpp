// 头文件保护：使用 `#pragma once` 替代传统的 `#ifndef/#define/#endif` 三件套。
// 理由：现代编译器（MSVC / GCC / Clang 均支持）下更简洁，且避免了宏名冲突
// 与"忘记写 #endif"等老问题；同时减少预处理时重复解析头文件的开销。
#pragma once

#include <string>
#include <tuple>
#include <vector>

namespace Scoreboard {
namespace Graphics {
using scoreboard_display_data_t =
    std::tuple<std::string, std::string, std::string, std::string, std::string,
               std::string, std::string>;

using scoreboard_display_data_list_t = std::vector<scoreboard_display_data_t>;
std::string ScoreboardOverlay(scoreboard_display_data_list_t sbddl);

using finalscore_display_data_t =
    std::tuple<std::string, std::string, std::string, std::string>;
std::string EndGameStatisticsPrompt(finalscore_display_data_t finalscore);
} // namespace Graphics
} // namespace Scoreboard
