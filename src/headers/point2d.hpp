// 头文件保护：使用 `#pragma once` 替代传统的 `#ifndef/#define/#endif` 三件套。
// 理由：现代编译器（MSVC / GCC / Clang 均支持）下更简洁，且避免了宏名冲突
// 与"忘记写 #endif"等老问题；同时减少预处理时重复解析头文件的开销。
#pragma once

#include <tuple>

class point2D_t {
  // Simple {x,y} datastructure = std::tuple<int, int>...
  using point_datatype_t = typename std::tuple<int, int>;
  point_datatype_t point_vector{};
  explicit point2D_t(const point_datatype_t pt) : point_vector{pt} {}

public:
  enum class PointCoord { COORD_X, COORD_Y };

  point2D_t() = default;
  point2D_t(const int x, const int y) : point2D_t(std::make_tuple(x, y)) {}

  template<PointCoord dimension>
  int get() const {
    return std::get<static_cast<int>(dimension)>(point_vector);
  }
  template<PointCoord dimension>
  void set(int value) {
    std::get<static_cast<int>(dimension)>(point_vector) = value;
  }
  point_datatype_t get() const { return point_vector; }
  void set(point_datatype_t value) { point_vector = value; }

  void set(const int x, const int y) { set(std::make_tuple(x, y)); }

  point2D_t &operator+=(const point2D_t &pt) {
    this->point_vector = std::make_tuple(
        get<PointCoord::COORD_X>() + pt.get<PointCoord::COORD_X>(),
        get<PointCoord::COORD_Y>() + pt.get<PointCoord::COORD_Y>());
    return *this;
  }

  point2D_t &operator-=(const point2D_t &pt) {
    this->point_vector = std::make_tuple(
        get<PointCoord::COORD_X>() - pt.get<PointCoord::COORD_X>(),
        get<PointCoord::COORD_Y>() - pt.get<PointCoord::COORD_Y>());
    return *this;
  }
};

inline point2D_t operator+(point2D_t l, const point2D_t &r) {
  l += r;
  return l;
}

inline point2D_t operator-(point2D_t l, const point2D_t &r) {
  l -= r;
  return l;
}
