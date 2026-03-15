cmake_minimum_required(VERSION 3.10)
project(DragonTearHoardManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add core source code (header-only mostly, but include dir is needed)
include_directories(${CMAKE_SOURCE_DIR}/src/core)

# Testing
enable_testing()
find_package(GTest REQUIRED)

add_executable(dthm_tests tests/test_logic.cpp)
target_link_libraries(dthm_tests GTest::gtest_main)

add_test(NAME TearLogicTests COMMAND dthm_tests)
