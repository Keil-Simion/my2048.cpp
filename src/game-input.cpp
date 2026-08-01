// =====================================================================
//  game-input.cpp —— 按键 → 方向意图的翻译层
// ---------------------------------------------------------------------
//  本文件把一次按键（一个或两个字节）翻译成四个方向的"意图"标志
//  (FLAG_MOVE_UP / DOWN / LEFT / RIGHT)。
//
//  本次重构新增一个分支：Windows 控制台方向键的 0xE0 / 0x00 前缀
//  处理。
//
//  背景：
//    在 Windows 原生控制台（cmd / powershell）中，方向键不是 ANSI
//    ESC [ A/B/C/D 序列，而是 2 字节序列：
//        ↑ : 0xE0 0x48
//        ↓ : 0xE0 0x50
//        → : 0xE0 0x4D
//        ← : 0xE0 0x4B
//    而 ESC [ x 则是 VT100 / xterm 等终端用的序列（VS Code 内置
//    终端、Linux 终端等走这一支）。
//
//  如果不处理 0xE0 / 0x00 前缀：
//    - 第一个字节 0xE0 会被三个 check 函数全部当作"无效"返回；
//    - 第二个字节（如 0x48 = 'H'）会被 check_input_vim 当成
//      "H = VIM LEFT" 错误触发左移，方向完全错位。
//
//  修复方式：在 check_input_ansi 中检测 0xE0 / 0x00 前缀，并直接把
//  下一个字节作为扫描码映射到四个方向。
//
//  注意：第二个字节与 WASD / VIM / 特殊键的字符值会发生碰撞
//  （H / P / M / K），所以必须在这里显式消费掉，否则方向完全错乱。
// =====================================================================

#include "game-input.hpp"
#include "global.hpp"

namespace Game {
namespace Input {
bool check_input_ansi(char c, intendedmove_t &intendedmove) {
  using namespace Keypress::Code;

  // ANSI / VT100 方向键：ESC [ A/B/C/D
  if (c == CODE_ANSI_TRIGGER_1) {
    getKeypressDownInput(c);
    if (c == CODE_ANSI_TRIGGER_2) {
      getKeypressDownInput(c);
      switch (c) {
      case CODE_ANSI_UP:
        intendedmove[FLAG_MOVE_UP] = true;
        return false;
      case CODE_ANSI_DOWN:
        intendedmove[FLAG_MOVE_DOWN] = true;
        return false;
      case CODE_ANSI_RIGHT:
        intendedmove[FLAG_MOVE_RIGHT] = true;
        return false;
      case CODE_ANSI_LEFT:
        intendedmove[FLAG_MOVE_LEFT] = true;
        return false;
      }
    }
    return true;
  }

  // Windows 控制台方向键：0xE0（或 0x00）前缀 + 扫描码
  //   ↑    = 0x48, ↓ = 0x50, → = 0x4D, ← = 0x4B
  // 不在这里消费的话，下一轮 getKeypressDownInput 会读到扫描码
  // （H/P/M/K），被 check_input_vim / check_input_other 误识别。
  if (c == '\xE0' || c == '\x00') {
    getKeypressDownInput(c);
    // 强制以 unsigned char 比较，避免 signed char 下负值参与 switch
    // 时的实现差异。
    switch (static_cast<unsigned char>(c)) {
    case 0x48:
      intendedmove[FLAG_MOVE_UP] = true;
      return false;
    case 0x50:
      intendedmove[FLAG_MOVE_DOWN] = true;
      return false;
    case 0x4D:
      intendedmove[FLAG_MOVE_RIGHT] = true;
      return false;
    case 0x4B:
      intendedmove[FLAG_MOVE_LEFT] = true;
      return false;
    }
    return true;
  }

  return true;
}

bool check_input_vim(char c, intendedmove_t &intendedmove) {
  using namespace Keypress::Code;
  switch (toupper(c)) {
  case CODE_VIM_UP:
    intendedmove[FLAG_MOVE_UP] = true;
    return false;
  case CODE_VIM_LEFT:
    intendedmove[FLAG_MOVE_LEFT] = true;
    return false;
  case CODE_VIM_DOWN:
    intendedmove[FLAG_MOVE_DOWN] = true;
    return false;
  case CODE_VIM_RIGHT:
    intendedmove[FLAG_MOVE_RIGHT] = true;
    return false;
  }
  return true;
}

bool check_input_wasd(char c, intendedmove_t &intendedmove) {
  using namespace Keypress::Code;
  switch (toupper(c)) {
  case CODE_WASD_UP:
    intendedmove[FLAG_MOVE_UP] = true;
    return false;
  case CODE_WASD_LEFT:
    intendedmove[FLAG_MOVE_LEFT] = true;
    return false;
  case CODE_WASD_DOWN:
    intendedmove[FLAG_MOVE_DOWN] = true;
    return false;
  case CODE_WASD_RIGHT:
    intendedmove[FLAG_MOVE_RIGHT] = true;
    return false;
  }
  return true;
}
} // namespace Input
} // namespace Game