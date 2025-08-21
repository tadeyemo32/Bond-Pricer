cmake_minimum_required(VERSION 3.12)
project(BondPricer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JSONCPP - check both system and local install paths
find_path(JSONCPP_INCLUDE_DIR json/json.h
    PATHS 
    /usr/local/include
    /usr/include
    /opt/homebrew/include
)

find_library(JSONCPP_LIBRARY
    NAMES jsoncpp
    PATHS
    /usr/local/lib
    /usr/lib
    /opt/homebrew/lib
    /usr/lib/x86_64-linux-gnu
)

message(STATUS "JSONCPP_INCLUDE_DIR: ${JSONCPP_INCLUDE_DIR}")
message(STATUS "JSONCPP_LIBRARY: ${JSONCPP_LIBRARY}")

if(NOT JSONCPP_INCLUDE_DIR OR NOT JSONCPP_LIBRARY)
    message(FATAL_ERROR "JSONCPP not found. Please install libjsoncpp-dev or build from source")
else()
    message(STATUS "Found JSONCPP: ${JSONCPP_LIBRARY}")
    include_directories(${JSONCPP_INCLUDE_DIR})
endif()

# SQLite3
find_library(SQLITE3_LIB sqlite3)
if(NOT SQLITE3_LIB)
    message(FATAL_ERROR "SQLite3 not found")
endif()

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/bond.cpp
    src/db.cpp
    src/market_data.cpp
    src/main.cpp
)

# Create executable
add_executable(bond_pricer ${SOURCES})

# Link libraries
target_link_libraries(bond_pricer
    ${SQLITE3_LIB}
    ${JSONCPP_LIBRARY}
)

# Set RPATH so the executable can find the libraries
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Copy Python script
configure_file(
    ${CMAKE_SOURCE_DIR}/fetch_market_data.py
    ${CMAKE_BINARY_DIR}/fetch_market_data.py
    COPYONLY
)

# Copy .env file if it exists
if(EXISTS ${CMAKE_SOURCE_DIR}/.env)
    configure_file(
        ${CMAKE_SOURCE_DIR}/.env
        ${CMAKE_BINARY_DIR}/.env
        COPYONLY
    )
endif()
