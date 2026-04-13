cmake_minimum_required(VERSION 3.16)
project(PoopDropGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(PoopDropGame
    src/main.cpp
    src/Game.cpp
    src/Board.cpp
    src/Piece.cpp
    src/ResourceManager.cpp
)

target_include_directories(PoopDropGame PRIVATE include)
target_link_libraries(PoopDropGame PRIVATE sfml-graphics sfml-window sfml-system)

if (WIN32)
    add_custom_command(TARGET PoopDropGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:PoopDropGame>/assets)
endif()
