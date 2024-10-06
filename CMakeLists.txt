cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(CoEdChatApp VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include header files
include_directories(include)

# Find Boost libraries
find_package(Boost 1.70.0 REQUIRED COMPONENTS system thread filesystem)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
else()
    message(FATAL_ERROR "Boost not found")
endif()

# Include nlohmann/json
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find SQLite3
find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})
message(STATUS "SQLite3 include dirs: ${SQLite3_INCLUDE_DIRS}")

# Add source files
set(SOURCES
    src/main.cpp
    src/Server.cpp
)

# Add the executable
add_executable(CoEdChatApp ${SOURCES})

# Link Boost and SQLite3 libraries
target_link_libraries(CoEdChatApp
    Boost::system
    Boost::thread
    Boost::filesystem
    ${SQLite3_LIBRARIES}
)