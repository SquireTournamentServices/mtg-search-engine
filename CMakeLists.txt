cmake_minimum_required(VERSION 3.9)

project(
  mtg-search-engine
  VERSION 1.0
  LANGUAGES C)

set(REPO_URL "https://github.com/djpiper28/mtg-search-engine")

message(STATUS "Building ${CMAKE_PROJECT_NAME}")
message(STATUS "Git repo: ${REPO_URL}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Version: ${CMAKE_PROJECT_VERSION}")
message(STATUS "Build OS: ${CMAKE_SYSTEM_NAME}")

# Get the branch name
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Commit info: ${GIT_COMMIT_HASH}@${GIT_BRANCH}")

# Version info
add_definitions("-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")
add_definitions("-DGIT_BRANCH=\"${GIT_BRANCH}\"")
add_definitions(
  "-DVERSION=\"${CMAKE_PROJECT_VERSION} (${GIT_COMMIT_HASH}@${GIT_BRANCH})\"")
add_definitions("-DOS=\"${CMAKE_SYSTEM_NAME}\"")
add_definitions("-DREPO_URL=\"${REPO_URL}\"")
add_definitions("-DBUFFER_LENGTH=4096")
add_definitions("-DPROJECT_VERSION=\"${CMAKE_PROJECT_VERSION}\"")
add_definitions("-DPROJECT_NAME=\"${CMAKE_PROJECT_NAME}\"")

# __FILENAME__
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
add_definitions("-D__FILENAME__=(__FILE__ + SOURCE_PATH_SIZE)")
add_definitions("-DMTG_BINARY_VERSION=1")

# Define proper compilation flags
if(MSVC)
  # Visual Studio: Maximum optimization, disable warning C4251, establish C++17
  # compatibility
  set(CMAKE_CXX_FLAGS_RELEASE
      "/Ox /MD /wd4251 /Zc:__cplusplus /std:c++17 /permissive-")
  # Generate complete debugging information set(CMAKE_CXX_FLAGS_DEBUG "/Zi")
else()
  # Compiler settings compiler flags
  set(COMPILER_FLAGS "-Wall -Wextra -Wpedantic")
  set(CMAKE_C_FLAGS_DEBUG "-g -O0 ${COMPILER_FLAGS} -DDEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
  set(CMAKE_C_FLAGS_RELEASE "-O2 ${COMPILER_FLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
endif()

set(SRC
    ./testing_h/ansi_colour.h
    ./testing_h/testing.h
    ./testing_h/testing.c
    ./testing_h/logger.h
    ./testing_h/logger.c
    ./src/card.h
    ./src/card.c
    ./src/set.h
    ./src/set.c
    ./src/uuid.h
    ./src/uuid.c
    ./src/io_utils.h
    ./src/io_utils.c)
set(TESTS ${SRC} ./tests/main.c)

set(LIBS "pthread")
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)

add_library(mtg-search-engine ${SRC})
target_link_libraries(mtg-search-engine ${LIBS})

add_executable(mtg-search-engine-cli ${SRC} ./src/main.c)
target_link_libraries(mtg-search-engine-cli ${LIBS})

if(CMAKE_BUILD_TYPE STREQUAL "TEST")
  include(CodeCoverage)

  # Setup coverage (using gcovr so needs gcc/g++) set(CMAKE_C_COMPILER
  # /usr/bin/gcc) set(CMAKE_CXX_COMPILER /usr/bin/g++)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -DTEST")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -DTEST")

  append_coverage_compiler_flags()
  setup_target_for_coverage_gcovr_xml(
    NAME
    coverage
    EXECUTABLE
    SquireDesktopTests
    EXCLUDE
    "build/*"
    "tests/*")

  # tests, set the flags manually
  set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_DEBUG})
  set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_DEBUG})

  add_executable(mtg-search-engine-tests ${TESTS})
  target_link_libraries(mtg-search-engine-tests PUBLIC ${LIBS})
  target_include_directories(mtg-search-engine-tests
                             PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

  file(COPY mem_tests.sh DESTINATION ${CMAKE_BINARY_DIR})
  file(COPY mem_tests.py DESTINATION ${CMAKE_BINARY_DIR})

  enable_testing()
  add_test(NAME main-tests COMMAND mtg-search-engine-tests)
  add_test(NAME memory-tests COMMAND bash mem_tests.sh)
  message(STATUS "TEST build setup")
endif()
