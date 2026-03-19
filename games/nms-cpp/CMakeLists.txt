cmake_minimum_required(VERSION 3.14)
project(dthm_oracle)

include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.15.3
)
FetchContent_MakeAvailable(nlohmann_json httplib)

find_package(Threads REQUIRED)

add_library(dthm_oracle STATIC
    src/SaveManager.cpp
    src/InventoryParser.cpp
    src/SaveWatcher.cpp
    src/WebServer.cpp
)

target_include_directories(dthm_oracle PUBLIC src)
target_link_libraries(dthm_oracle core nlohmann_json::nlohmann_json httplib::httplib lz4_static)
target_include_directories(dthm_oracle PRIVATE ${lz4_SOURCE_DIR}/lib)

add_executable(dthm_oracle_exe src/main.cpp)
target_link_libraries(dthm_oracle_exe PRIVATE dthm_oracle core nlohmann_json::nlohmann_json httplib::httplib Threads::Threads)
