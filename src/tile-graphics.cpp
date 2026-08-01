#include "tile-graphics.hpp"
#include "color.hpp"
#include "tile.hpp"
#include <array>
#include <cmath>
#include <format>

namespace Game {
namespace {

Color::Modifier tileColor(ull value) {
  constexpr std::array<Color::Modifier, 11> colors = {
      Color::red,     Color::yellow, Color::magenta, Color::blue,
      Color::cyan,    Color::yellow, Color::red,     Color::yellow,
      Color::magenta, Color::blue,   Color::green};
  const int log = log2(value);
  const int index = log < 12 ? log - 1 : 10;
  return colors[index];
}
} // namespace

std::string drawTileString(tile_t currentTile) {
  if (!currentTile.value) {
    return "    ";
  }
  return std::format("{}{}{:>4}{}{}", tileColor(currentTile.value),
                     Color::bold_on, currentTile.value, Color::bold_off,
                     Color::def);
}
} // namespace Game