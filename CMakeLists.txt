cmake_minimum_required(VERSION 3.7...3.21)
project(2048 LANGUAGES CXX)

# MSVC 下显式要求源文件使用 UTF-8 编码（避免中文 / Unicode 注释乱码）。
if(MSVC)
    add_compile_options(/utf-8)
endif()

enable_testing()

# 项目源文件列表。
# 注意：所有 header（*.hpp）均不直接列出，通过 src/*.cpp 中的
# #include 引入即可（PRIVATE include dir 见 target_include_directories）。
set(SOURCES src/2048.cpp src/gameboard.cpp src/gameboard-graphics.cpp src/game.cpp src/game-graphics.cpp src/game-input.cpp src/game-pregamemenu.cpp src/global.cpp src/loadresource.cpp src/menu.cpp src/menu-graphics.cpp src/saveresource.cpp src/scores.cpp src/scores-graphics.cpp src/statistics.cpp src/statistics-graphics.cpp src/tile.cpp src/tile-graphics.cpp)

# 各编译器的告警选项（保留原状，未做修改）。
if(CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  add_compile_options(-Wall)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL Clang)
  add_compile_options(-Wall)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^Intel")
  if(WIN32)
    add_compile_options(/W3)
  else()
    add_compile_options(-w2)
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL NVHPC)
  add_compile_options(-a)
endif()

add_executable(2048 ${SOURCES})
target_include_directories(2048 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/headers/)

# ---------------------------------------------------------------------
#  C++ 标准升级：cxx_std_17 → cxx_std_20
# ---------------------------------------------------------------------
#  升级原因（与重构配套）：
#    1) src/headers/color.hpp 使用了 std::formatter 特化 —— 这是
#       C++20 才引入的标准扩展机制（C++17 仅有 std::vformat 不支持
#       用户自定义类型的 format）。
#    2) 各 graphics .cpp 使用 std::format 替代 std::ostringstream
#       流式拼接，依赖 C++20 的 std::format（GCC 13+/MSVC 19.29+
#       才完整支持）。
#    3) 结构化绑定（structured bindings）虽然 C++17 已可用，但
#       本次大量使用 `auto& [...]` 引用绑定 + `std::tie` 替换写法，
#       在 C++20 下能更稳定地被 MSVC 接受。
#
#  兼容性影响：
#    - 不再支持 C++17 编译器。CI / 开发者机器需要 MSVC 19.29+、
#      GCC 13+、Clang 14+。
# ---------------------------------------------------------------------
target_compile_features(2048 PRIVATE cxx_std_20)

# --- test

find_program(PWSH NAMES pwsh powershell)
if(PWSH)
  add_test(NAME basic COMMAND ${PWSH} -c \"echo 4 | $<TARGET_FILE:2048>\")
elseif(UNIX)
  add_test(NAME basic COMMAND sh -c "echo 4 | $<TARGET_FILE:2048>")
endif()

# --- install

install(TARGETS 2048
  RUNTIME DESTINATION bin)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
        DESTINATION ${CMAKE_INSTALL_PREFIX})