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
  return std::format("{}{}{}{}{}{}{}\n", Color::bold_on, sp, greetings_text,
                     Color::blue, gamename_text, Color::def, Color::bold_off);
}

std::string MainMenuOptionsPrompt() {
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