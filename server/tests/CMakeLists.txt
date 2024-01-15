CMAKE_MINIMUM_REQUIRED(VERSION 3.21)
project(tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE)

FetchContent_MakeAvailable(googletest)

find_package(cpr REQUIRED)
FIND_PACKAGE(PostgreSQL REQUIRED)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/source)

include_directories(
  ${INCLUDE_DIR})

file(GLOB_RECURSE SOURCES
  ${INCLUDE_DIR}/*.hpp
  ${SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} gtest gtest_main cpr::cpr PostgreSQL::PostgreSQL)
