#pragma once

// =====================================================================
//  color.hpp —— 终端 ANSI 颜色修饰符
// ---------------------------------------------------------------------
//  本文件被项目中所有"绘图"型 .cpp（game-graphics / menu-graphics /
//  scores-graphics / statistics-graphics / tile-graphics 等）所使用，
//  用于在终端输出中嵌入 ANSI 转义序列，从而改变前景色、背景色、
//  加粗等样式。
//
//  本次重构涉及的关键改动：
//    1) 原本所有修饰符（red / green / bold_on / bold_off / def 等）
//       都是 file-scope 的 `static Color::Modifier` 变量。这有几个
//       问题：
//         - 静态初始化顺序不可控（虽然此处问题不大）；
//         - 与 constexpr 思想相悖；
//         - 与用户期望的"显式 Color::xxx 调用"风格不一致。
//    2) 重构后改为 `inline constexpr` 常量：
//         - inline（C++17 引入）保证跨翻译单元的唯一性，无 ODR 违例；
//         - constexpr 允许在常量表达式场景中使用；
//         - 仍是 `const Modifier`，所有原有 `os << red` 调用继续可用。
//    3) 为了让 `std::format("{}", Color::red)` 这类写法生效，提供
//       std::formatter 特化，直接把 Modifier 格式化为对应的 ANSI
//       转义序列。这是 C++20 std::format 的标准扩展机制。
//    4) 使用 `inline namespace Modifiers` 让成员既可写成
//       `Color::bold_on`，又可通过外层命名空间查询（保留未来扩展
//       灵活性，例如单个 `using Color::bold_on;`）。
// =====================================================================

#include <format>
#include <ostream>

namespace Color {

// ---------------------------------------------------------------------
//  ANSI 控制码枚举
//  这里只列举项目实际使用到的几种；值与标准 VT100 / xterm 兼容。
// ---------------------------------------------------------------------
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

// ---------------------------------------------------------------------
//  Modifier —— 轻量包装类型，仅持有一个 Code。
//  通过 `operator<<` 输出 ANSI 转义序列 `\033[<code>m`。
//  之前为非 constexpr 的普通类；重构后构造与访问函数都是 constexpr，
//  这样上面那一堆 inline constexpr 变量才能合法。
// ---------------------------------------------------------------------
class Modifier {
  Code code;

public:
  constexpr Modifier(Code pCode) : code(pCode) {}
  constexpr Code value() const { return code; }

  // 与 std::ostream 的协作：原代码已存在，重构保留（兼容旧式 ostringstream
  // 流式写法）。std::format 的格式化走下方的 std::formatter 特化。
  friend std::ostream &operator<<(std::ostream &os, const Modifier &mod) {
    return os << "\033[" << static_cast<int>(mod.code) << "m";
  }
};

// ---------------------------------------------------------------------
//  修饰符常量集合
//  使用 `inline namespace` 让成员在 `Color::` 作用域内可以直接访问，
//  同时保留 `Color::Modifiers::bold_on` 这种完全限定写法。
//
//  对比之前的写法：
//    旧：`static Color::Modifier bold_off(Color::Code::RESET);`（file-scope）
//    新：`inline constexpr Color::Modifier bold_off{Code::RESET};`
//  改进点：
//    - 不再有静态初始化顺序问题；
//    - 可以安全地在头文件中包含而无 ODR 违例（inline 保证唯一实体）；
//    - 可在 constexpr 上下文使用；
//    - 类型安全（每个修饰符都有明确的 `Modifier` 类型，而不是裸 int）。
// ---------------------------------------------------------------------
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

// ---------------------------------------------------------------------
//  std::formatter 特化（C++20 引入的标准扩展机制）
//
//  让 `std::format("{}{}hello{}{}", Color::red, Color::bold_on, ...)`
//  这类写法生效：直接把 Modifier 格式化为对应 ANSI 转义序列。
//
//  旧实现下，要嵌入颜色必须写：
//      std::ostringstream oss;
//      oss << Color::red << Color::bold_on << "hello" << Color::bold_off;
//      std::cout << oss.str();
//  现在可以一行搞定：
//      std::print("{}{}hello{}", Color::red, Color::bold_on);  // C++23
//      std::format("{}{}hello{}", Color::red, Color::bold_on); // C++20
//
//  parse 直接返回 ctx.begin() 表示不接受自定义格式说明（与默认行为一致）。
// ---------------------------------------------------------------------
template<>
struct std::formatter<Color::Modifier> {
  constexpr auto parse(std::format_parse_context &ctx) const {
    return ctx.begin();
  }
  auto format(Color::Modifier mod, std::format_context &ctx) const {
    return std::format_to(ctx.out(), "\033[{}m", static_cast<int>(mod.value()));
  }
};