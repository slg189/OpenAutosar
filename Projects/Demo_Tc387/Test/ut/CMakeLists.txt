# ============================================================================
# 单元测试 (GoogleTest) —— 在 host (gcc/clang) 上对可主机编译的模块做单元测试。
# 由 Tools/scripts/run_gtest.py 驱动:  cmake -S . -B build_test && ctest
# ============================================================================
cmake_minimum_required(VERSION 3.16)
project(OpenAutosar_UT CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 仓库根 (ut -> Test -> <Project> -> Projects -> 根)
get_filename_component(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE)

# 覆盖率 (run_gtest.py 传 -DCMAKE_BUILD_TYPE=Coverage)
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
  add_compile_options(--coverage -O0 -g)
  add_link_options(--coverage)
endif()

# ---- GoogleTest (FetchContent, 固定 tag 以可复现) ----
include(FetchContent)
FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
  GIT_SHALLOW    TRUE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# AUTOSAR 公共类型 (Std_Types.h)
set(COMMON_INC ${REPO_ROOT}/MCAL/inc)

# ---- 注册一个单元测试: 测试源(.cpp) + 被测模块源(.c) + include ----
function(add_autosar_test name)
  cmake_parse_arguments(T "" "" "SRCS;INCS" ${ARGN})
  add_executable(${name} ${T_SRCS})
  target_include_directories(${name} PRIVATE ${COMMON_INC} ${T_INCS})
  target_link_libraries(${name} PRIVATE gtest_main)
  gtest_discover_tests(${name})
endfunction()

# ===== MCAL / Adc =====
add_autosar_test(test_adc
  SRCS ${CMAKE_CURRENT_SOURCE_DIR}/MCAL/test_adc.cpp
       ${REPO_ROOT}/MCAL/Tc387/Adc/src/Adc.c
  INCS ${REPO_ROOT}/MCAL/Tc387/Adc/inc)

# —— 新增模块测试时, 仿照上面追加 add_autosar_test(...) ——
