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

cmake_minimum_required(VERSION 3.14)

project(DTHM_Cyberpunk2077)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Make sure core is available if building standalone
if(NOT TARGET core)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../core ${CMAKE_CURRENT_BINARY_DIR}/core)
endif()

# RED4ext Plugin shared library
add_library(DTHM_Cyberpunk2077 SHARED src/plugin.cpp)
target_link_libraries(DTHM_Cyberpunk2077 PRIVATE core)

# Position independent code for shared library
set_target_properties(DTHM_Cyberpunk2077 PROPERTIES POSITION_INDEPENDENT_CODE ON)
