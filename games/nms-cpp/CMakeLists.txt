cmake_minimum_required(VERSION 3.10)
project(dthm_oracle)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

add_library(dthm_oracle STATIC
    src/SaveManager.cpp
    src/ConfigLoader.cpp
    src/InventoryParser.cpp
)

target_include_directories(dthm_oracle PUBLIC src)
target_link_libraries(dthm_oracle PRIVATE OpenSSL::SSL OpenSSL::Crypto)
