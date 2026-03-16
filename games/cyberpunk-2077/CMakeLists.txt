cmake_minimum_required(VERSION 3.10)

project(DTHM_Cyberpunk2077)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Core include
# CMAKE_SOURCE_DIR will be games/cyberpunk-2077 if configured individually.
# We should use a path relative to the current list file to find core.
include_directories(${CMAKE_CURRENT_LIST_DIR}/../../src/core)

# RED4ext Plugin shared library
add_library(DTHM_Cyberpunk2077 SHARED plugin.cpp)

# Position independent code for shared library
set_target_properties(DTHM_Cyberpunk2077 PROPERTIES POSITION_INDEPENDENT_CODE ON)
