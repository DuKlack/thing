
project(b LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# ----------------------------------------------------
# 1. Add Local Subdirectories
# ----------------------------------------------------
add_subdirectory(cachelot)
add_subdirectory(cpp-httplib)
add_subdirectory(rapidjson)

# ----------------------------------------------------
# 2. Executable Target
# ----------------------------------------------------
add_executable(${PROJECT_NAME}
    main.cpp

)

# ----------------------------------------------------
# 3. Includes & Target Links
# ----------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cachelot/src
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp-httplib
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    httplib::httplib
    cachelot
    Threads::Threads
)