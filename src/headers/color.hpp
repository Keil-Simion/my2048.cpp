#pragma once

#include <format>
#include <ostream>

namespace Color {

enum class Code {
  BOLD = 1,
  RESET = 0,
  BG_BLUE = 44,
  BG_DEFAULT = 49,
  BG_GREEN = 42,
  BG_RED = 41,
  FG_BLACK = 30,
  FG_BLUE = 34,
  FG_CYAN = 36,
  FG_DARK_GRAY = 90,
  FG_DEFAULT = 39,
  FG_GREEN = 32,
  FG_LIGHT_BLUE = 94,
  FG_LIGHT_CYAN = 96,
  FG_LIGHT_GRAY = 37,
  FG_LIGHT_GREEN = 92,
  FG_LIGHT_MAGENTA = 95,
  FG_LIGHT_RED = 91,
  FG_LIGHT_YELLOW = 93,
  FG_MAGENTA = 35,
  FG_RED = 31,
  FG_WHITE = 97,
  FG_YELLOW = 33,
};

class Modifier {
  Code code;

public:
  constexpr Modifier(Code pCode) : code(pCode) {}
  constexpr Code value() const { return code; }

  friend std::ostream &operator<<(std::ostream &os, const Modifier &mod) {
    return os << "\033[" << static_cast<int>(mod.code) << "m";
  }
};

// An inline namespace: members are accessible as `Color::bold_on` AND via
// unqualified lookup in any enclosing namespace (including the global one).
inline namespace Modifiers {
inline constexpr Modifier bold_off{Code::RESET};
inline constexpr Modifier bold_on{Code::BOLD};
inline constexpr Modifier def{Code::FG_DEFAULT};
inline constexpr Modifier red{Code::FG_RED};
inline constexpr Modifier green{Code::FG_GREEN};
inline constexpr Modifier yellow{Code::FG_YELLOW};
inline constexpr Modifier blue{Code::FG_BLUE};
inline constexpr Modifier magenta{Code::FG_MAGENTA};
inline constexpr Modifier cyan{Code::FG_CYAN};
inline constexpr Modifier lightGray{Code::FG_LIGHT_GRAY};
inline constexpr Modifier darkGray{Code::FG_DARK_GRAY};
inline constexpr Modifier lightRed{Code::FG_LIGHT_RED};
inline constexpr Modifier lightGreen{Code::FG_LIGHT_GREEN};
inline constexpr Modifier lightYellow{Code::FG_LIGHT_YELLOW};
inline constexpr Modifier lightBlue{Code::FG_LIGHT_BLUE};
inline constexpr Modifier lightMagenta{Code::FG_LIGHT_MAGENTA};
inline constexpr Modifier lightCyan{Code::FG_LIGHT_CYAN};
} // namespace Modifiers

} // namespace Color

// std::format support: embed an ANSI escape sequence directly in a format string.
template <>
struct std::formatter<Color::Modifier> {
  constexpr auto parse(std::format_parse_context &ctx) const {
    return ctx.begin();
  }
  auto format(Color::Modifier mod, std::format_context &ctx) const {
    return std::format_to(ctx.out(), "\033[{}m",
                          static_cast<int>(mod.value()));
  }
};