cmake_minimum_required(VERSION 3.20)

project(
    contract
    LANGUAGES CXX
    VERSION   1.0
)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(external)

add_library(${PROJECT_NAME} STATIC
    include/contract.hpp
    source/contract.cpp
)
add_library(contract::contract ALIAS contract)

target_include_directories(${PROJECT_NAME}
    PUBLIC include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE spdlog::spdlog
)