// =====================================================================
//  menu-graphics.cpp —— 主菜单相关文本绘制
// ---------------------------------------------------------------------
//  本次重构要点：
//    - 把所有 std::ostringstream + Color::Modifier << 链式拼接
//      改为 std::format + Color::xxx；
//    - 注意 MainMenuOptionsPrompt 里 "\n" 出现在格式串最前，
//      对应旧写法中先 << "\n" 再循环输出菜单项最后 << "\n"，
//      顺序一致。
//    - 输入菜单错误提示、输入提示全部统一为 std::format 风格。
// =====================================================================

#include "menu-graphics.hpp"
#include "color.hpp"
#include "global.hpp"
#include <format>
#include <sstream>

namespace Game {
namespace Graphics {
namespace Menu {

std::string MainMenuTitlePrompt() {
  constexpr auto greetings_text = "Welcome to ";
  constexpr auto gamename_text = "2048!";
  constexpr auto sp = "  ";
  // 注意这里的颜色顺序：bold_on → 问候语 → blue（蓝色 2048!）→ def → bold_off，
  // 与旧代码 color 切换顺序完全一致。
  return std::format("{}{}{}{}{}{}{}\n", Color::bold_on, sp, greetings_text,
                     Color::blue, gamename_text, Color::def, Color::bold_off);
}

std::string MainMenuOptionsPrompt() {
  // 整个菜单选项是一段静态文本，直接拼成单个字面量最清晰。
  // 旧写法是一个循环逐项拼，行为等价。
  return std::format(
      "\n        1. Play a New Game\n        2. Continue Previous Game\n"
      "        3. View Highscores and Statistics\n        4. Exit\n\n");
}

std::string InputMenuErrorInvalidInputPrompt() {
  constexpr auto err_input_text = "Invalid input. Please try again.";
  constexpr auto sp = "  ";
  return std::format("{}{}{}{}\n\n", Color::red, sp, err_input_text, Color::def);
}

std::string InputMenuPrompt() {
  constexpr auto sp = "  ";
  return std::format("{}{}", sp, "Enter Choice: ");
}

std::string MainMenuGraphicsOverlay(bool input_error_choice_invalid) {
  std::ostringstream str_os;
  DrawAlways(str_os, MainMenuTitlePrompt);
  DrawAlways(str_os, MainMenuOptionsPrompt);
  DrawOnlyWhen(str_os, input_error_choice_invalid,
               InputMenuErrorInvalidInputPrompt);
  DrawAlways(str_os, InputMenuPrompt);
  return str_os.str();
}

} // namespace Menu
} // namespace Graphics
} // namespace Game