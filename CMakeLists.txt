cmake_minimum_required(VERSION 3.10)

project(BondPricer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

add_executable(bond_pricer ${SOURCES})


