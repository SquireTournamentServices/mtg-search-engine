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
add_definitions("-DMSE_GIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\"")
add_definitions("-DMSE_GIT_BRANCH=\"${GIT_BRANCH}\"")
add_definitions(
  "-DMSE_VERSION=\"${CMAKE_PROJECT_VERSION} (${GIT_COMMIT_HASH}@${GIT_BRANCH})\""
)
add_definitions("-DOS=\"${CMAKE_SYSTEM_NAME}\"")
add_definitions("-DMSE_REPO_URL=\"${REPO_URL}\"")
add_definitions("-DBUFFER_LENGTH=4096")
add_definitions("-DMSE_PROJECT_VERSION=\"${CMAKE_PROJECT_VERSION}\"")
add_definitions("-DMSE_PROJECT_NAME=\"${CMAKE_PROJECT_NAME}\"")

# __FILENAME__
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
add_definitions("-D__FILENAME__=(__FILE__ + SOURCE_PATH_SIZE)")
add_definitions("-DMTG_BINARY_VERSION=1")

set(COMPILER_FLAGS "-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -g")
set(CMAKE_C_FLAGS_DEBUG "${COMPILER_FLAGS} -DDEBUG")

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
    ./src/mtg_json.h
    ./src/mtg_json.c
    ./src/thread_pool.h
    ./src/thread_pool.c
    ./src/io_utils.h
    ./src/io_utils.c)
set(TESTS
    ${SRC}
    ./tests/main.c
    ./tests/test_thread_pool.h
    ./tests/test_thread_pool.c
    ./tests/test_mtg_json.h
    ./tests/test_mtg_json.c
    ./tests/test_uuid.h
    ./tests/test_uuid.c
    ./tests/test_io_utils.h
    ./tests/test_io_utils.c)

set(LIBS jansson curl pthread m)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)

add_library(mtg-search-engine ${SRC})
target_link_libraries(mtg-search-engine ${LIBS})

add_executable(mtg-search-engine-cli ${SRC} ./src/main.c)
target_link_libraries(mtg-search-engine-cli ${LIBS})

add_executable(mtg-search-engine-tests ${TESTS})
target_link_libraries(mtg-search-engine-tests PUBLIC ${LIBS})
target_include_directories(mtg-search-engine-tests
                           PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

file(COPY mem_tests.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY mem_tests.py DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

enable_testing()
add_test(NAME main-tests COMMAND mtg-search-engine-tests)
add_test(NAME memory-tests COMMAND bash mem_tests.sh)
message(STATUS "TEST build setup")
