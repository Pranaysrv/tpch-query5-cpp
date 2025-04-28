cmake_minimum_required(VERSION 3.10)
project(tpch_query5)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O3")

# Include directories
include_directories(include)

# Add source files
set(SOURCES
    src/main.cpp
    src/data_loader.cpp
    src/query_processor.cpp
    src/thread_pool.cpp
)

# Create executable
add_executable(tpch_query5 ${SOURCES})

# Link with pthread library
find_package(Threads REQUIRED)
target_link_libraries(tpch_query5 PRIVATE Threads::Threads)

# Install target
install(TARGETS tpch_query5 DESTINATION bin)