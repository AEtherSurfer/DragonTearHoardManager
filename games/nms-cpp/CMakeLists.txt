cmake_minimum_required(VERSION 3.14)
project(dthm_oracle)

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(dthm_oracle STATIC
    src/SaveManager.cpp
    src/InventoryParser.cpp
)

target_include_directories(dthm_oracle PUBLIC src)
target_link_libraries(dthm_oracle PRIVATE core nlohmann_json::nlohmann_json)

add_executable(dthm_oracle_exe src/main.cpp)
target_link_libraries(dthm_oracle_exe PRIVATE dthm_oracle core nlohmann_json::nlohmann_json)
