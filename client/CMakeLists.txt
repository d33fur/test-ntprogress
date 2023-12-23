cmake_minimum_required(VERSION 3.5)

project(stock_exchange_client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Network)

file(GLOB_RECURSE SOURCES
        *.h
        *.cpp)

add_executable(${PROJECT_NAME} WIN32 main.cpp ${SOURCES})

target_link_libraries(${PROJECT_NAME}
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Network)

install(TARGETS ${PROJECT_NAME} DESTINATION .)