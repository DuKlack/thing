cmake_minimum_required(VERSION 3.12)
project(CacheProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find POCO package and its required components
find_package(Poco REQUIRED COMPONENTS  Net )

# Include current directory and all third-party header paths
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson
    ${CMAKE_CURRENT_SOURCE_DIR}/cachelot
)

# Define all your implementation source files visible in the project
set(SOURCES
    main.cpp
)

# Create the executable target
add_executable(CacheApp ${SOURCES})

# Link POCO libraries using the proper imported targets
target_link_libraries(CacheApp PRIVATE
    rapidjson
    Poco::Net
)
