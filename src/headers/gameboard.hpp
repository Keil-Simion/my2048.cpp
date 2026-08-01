// 头文件保护：使用 `#pragma once` 替代传统的 `#ifndef/#define/#endif` 三件套。
// 理由：现代编译器（MSVC / GCC / Clang 均支持）下更简洁，且避免了宏名冲突
// 与"忘记写 #endif"等老问题；同时减少预处理时重复解析头文件的开销。
#pragma once

#include "tile.hpp"
#include <tuple>
#include <vector>

struct point2D_t;

namespace Game {

struct GameBoard {
  using tile_data_array_t = std::vector<tile_t>;
  using gameboard_data_array_t = std::tuple<size_t, tile_data_array_t>;

  gameboard_data_array_t gbda;
  bool win{};
  bool moved{true};
  ull score{};
  ull largestTile{2};
  long long moveCount{-1};

  GameBoard() = default;
  explicit GameBoard(ull playsize);
  explicit GameBoard(ull playsize, tile_data_array_t prempt_board);
};

size_t getPlaySizeOfGameboardDataArray(const GameBoard::gameboard_data_array_t &gbda);
tile_t getTileOnGameboardDataArray(const GameBoard::gameboard_data_array_t &gbda,
                                   point2D_t pt);

bool hasWonOnGameboard(GameBoard gb);
long long MoveCountOnGameBoard(GameBoard gb);

void unblockTilesOnGameboard(GameBoard &gb);
bool canMoveOnGameboard(GameBoard &gb);
bool addTileOnGameboard(GameBoard &gb);
void registerMoveByOneOnGameboard(GameBoard &gb);

void tumbleTilesUpOnGameboard(GameBoard &gb);
void tumbleTilesDownOnGameboard(GameBoard &gb);
void tumbleTilesLeftOnGameboard(GameBoard &gb);
void tumbleTilesRightOnGameboard(GameBoard &gb);

std::string printStateOfGameBoard(GameBoard gb);

} // namespace Game
