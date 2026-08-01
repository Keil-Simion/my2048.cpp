// =====================================================================
//  tile-graphics.cpp —— 单个 2048 tile 的绘制
// ---------------------------------------------------------------------
//  本文件改动要点：
//    1) tileColor 函数中的颜色表从 `std::vector<Color::Modifier>`
//       改为 `std::array<Color::Modifier, 11>`：
//         - constexpr：编译期常量，无运行时构造开销；
//         - 长度固定为 11（与下面 log < 12 ? log - 1 : 10 索引上限一致），
//           比 vector 更明确表达"颜色档位是固定的"这一意图；
//         - 替代 std::vector 是为了让 tileColor 自身能成为 constexpr。
//       注意：CTAD（类模板实参推导）在 MSVC C++20 模式下对
//       `std::array {a, b, c, ...}` 推导有时不稳定，所以这里显式
//       写 `<Color::Modifier, 11>`。
//
//    2) drawTileString 从
//         std::ostringstream + Color::Modifier 的链式 <<
//       改为 std::format：
//         - {:>4} 让 tile 数值右对齐到 4 个字符宽度，正好对齐棋盘格；
//         - 颜色常量显式写 Color::xxx，与项目"避免 using namespace"
//           的现代风格一致。
// =====================================================================

#include "tile-graphics.hpp"
#include "color.hpp"
#include "tile.hpp"
#include <array>
#include <cmath>
#include <format>

namespace Game {
namespace {

Color::Modifier tileColor(ull value) {
  // 11 个槽位对应 2^1 (=2) 到 2^11 (=2048) 这一段的颜色，
  // 更大的 tile（理论上）会落到 index=10 的最后一档（绿色）。
  constexpr std::array<Color::Modifier, 11> colors = {
      Color::red,     Color::yellow, Color::magenta, Color::blue,
      Color::cyan,    Color::yellow, Color::red,     Color::yellow,
      Color::magenta, Color::blue,   Color::green};
  const int log = log2(value);
  const int index = log < 12 ? log - 1 : 10;
  return colors[index];
}
} // namespace

std::string drawTileString(const tile_t &currentTile) {
  if (!currentTile.value) {
    return "    ";
  }
  // 旧写法：
  //   std::ostringstream tile_richtext;
  //   tile_richtext << tileColor(currentTile.value)
  //                 << bold_on << std::setw(4) << currentTile.value
  //                 << bold_off << def;
  //   return tile_richtext.str();
  // 新写法：std::format 一行搞定，{:>4} 是 std::format 的对齐语法
  // （右对齐，宽度 4），完全等价于 std::setw(4)。
  return std::format("{}{}{:>4}{}{}", tileColor(currentTile.value),
                     Color::bold_on, currentTile.value, Color::bold_off,
                     Color::def);
}
} // namespace Game