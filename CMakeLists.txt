# Dragon Tear Hoard Manager
# Copyright (C) 2024
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)
project(DragonTearHoardManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add subdirectories
add_subdirectory(core)
add_subdirectory(games/nms)
add_subdirectory(games/cyberpunk-2077)
add_subdirectory(games/nms-cpp)

# Testing
enable_testing()
find_package(GTest REQUIRED)

add_executable(dthm_tests tests/test_logic.cpp)
target_link_libraries(dthm_tests core GTest::gtest_main)

add_test(NAME TearLogicTests COMMAND dthm_tests)
