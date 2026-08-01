// 头文件保护：使用 `#pragma once` 替代传统的 `#ifndef/#define/#endif` 三件套。
// 理由：现代编译器（MSVC / GCC / Clang 均支持）下更简洁，且避免了宏名冲突
// 与"忘记写 #endif"等老问题；同时减少预处理时重复解析头文件的开销。
#pragma once

#include <iosfwd>
#include <string>

using ull = unsigned long long;
void getKeypressDownInput(char &);

template<typename T>
void DrawAlways(std::ostream &os, T f) {
  os << f();
}

template<typename T>
void DrawOnlyWhen(std::ostream &os, bool trigger, T f) {
  if (trigger) {
    DrawAlways(os, f);
  }
}

template<typename T>
void DrawAsOneTimeFlag(std::ostream &os, bool &trigger, T f) {
  if (trigger) {
    DrawAlways(os, f);
    trigger = !trigger;
  }
}

template<typename suppliment_t>
struct DataSupplimentInternalType {
  suppliment_t suppliment_data;
  template<typename function_t>
  std::string operator()(function_t f) const {
    return f(suppliment_data);
  }
};

template<typename suppliment_t, typename function_t>
auto DataSuppliment(suppliment_t needed_data, function_t f) {
  using dsit_t = DataSupplimentInternalType<suppliment_t>;
  const auto lambda_f_to_return = [=]() {
    const dsit_t depinject_func = dsit_t{needed_data};
    return depinject_func(f);
  };
  return lambda_f_to_return;
}

void pause_for_keypress();
void wait_for_any_letter_input(std::istream &is);
void clearScreen();
std::string secondsFormat(double);
