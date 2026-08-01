#include "game-input.hpp"
#include "global.hpp"

namespace Game {
namespace Input {
bool check_input_ansi(char c, intendedmove_t &intendedmove) {
  using namespace Keypress::Code;

  // ANSI / VT100 arrow-key sequences: ESC [ A/B/C/D
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

  // Windows console arrow-key sequences: 0xE0 (or 0x00) followed by a scan code:
  //   Up    = 0x48, Down  = 0x50, Right = 0x4D, Left  = 0x4B
  // Without this branch, the second byte would leak into the next read and be
  // matched as a WASD/VIM/special keypress, causing the wrong move.
  if (c == '\xE0' || c == '\x00') {
    getKeypressDownInput(c);
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
