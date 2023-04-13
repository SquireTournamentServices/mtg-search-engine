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
add_definitions("-DMSE_OS=\"${CMAKE_SYSTEM_NAME}\"")
add_definitions("-DMSE_REPO_URL=\"${REPO_URL}\"")
add_definitions("-DMSE_PROJECT_VERSION=\"${CMAKE_PROJECT_VERSION}\"")
add_definitions("-DMSE_PROJECT_NAME=\"${CMAKE_PROJECT_NAME}\"")
add_definitions("-DMSE_BINARY_VERSION=1")

# Packages
find_package(CURL)

# __FILENAME__
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
add_definitions("-D__FILENAME__=(&__FILE__[SOURCE_PATH_SIZE])")

set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -Wno-unused-parameter -g")

# Generated sources
set(LANG_SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/mse_langs.h
                      ${CMAKE_CURRENT_BINARY_DIR}/mse_langs.c)
add_custom_command(
  OUTPUT ${LANG_SOURCE_FILES}
  COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_languages_files.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(FILTER_SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/mse_char_map.h
                        ${CMAKE_CURRENT_BINARY_DIR}/mse_char_map.c)
add_custom_command(
  OUTPUT ${FILTER_SOURCE_FILES}
  COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/generate_char_map.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(SRC
    ./testing_h/ansi_colour.h
    ./testing_h/testing.h
    ./testing_h/testing.c
    ./testing_h/logger.h
    ./testing_h/logger.c
    ./src/card.h
    ./src/card.c
    ./src/card_str_match.h
    ./src/card_str_match.c
    ./src/card_txt_fields_trie.h
    ./src/card_txt_fields_trie.c
    ./src/set.h
    ./src/set.c
    ./src/uuid.h
    ./src/uuid.c
    ./src/mtg_json.h
    ./src/mtg_json.c
    ./src/mtg_json_indexes.h
    ./src/mtg_json_indexes.c
    ./src/thread_pool.h
    ./src/thread_pool.c
    ./src/io_utils.h
    ./src/io_utils.c
    ./src/avl_tree.h
    ./src/avl_tree.c
    ./src/search.h
    ./src/search.c
    ./src/generator_set.h
    ./src/generator_set.c
    ./src/generators.h
    ./src/generators.c
    ./src/generator_double_fields.h
    ./src/generator_double_fields.c
    ./src/generator_oracle.h
    ./src/generator_oracle.c
    ./src/generator_name.h
    ./src/generator_name.c
    ${FILTER_SOURCE_FILES}
    ${LANG_SOURCE_FILES})
set(TESTS
    ./tests/main.c
    ./tests/test_thread_pool.h
    ./tests/test_thread_pool.c
    ./tests/test_mtg_json.h
    ./tests/test_mtg_json.c
    ./tests/test_uuid.h
    ./tests/test_uuid.c
    ./tests/test_io_utils.h
    ./tests/test_io_utils.c
    ./tests/test_set.h
    ./tests/test_set.c
    ./tests/test_card.h
    ./tests/test_card.c
    ./tests/test_card_str_match.h
    ./tests/test_card_str_match.c
    ./tests/test_card_txt_field_trie.h
    ./tests/test_card_txt_field_trie.c
    ./tests/test_avl_tree.h
    ./tests/test_avl_tree.c
    ./tests/test_search.h
    ./tests/test_search.c
    ./tests/test_generators.h
    ./tests/test_generators.c
    ./tests/test_generator_validation.c
    ./tests/test_generator_double_fields.c
    ./tests/test_generator_set.c
    ./tests/test_mse_langs.h
    ./tests/test_mse_langs.c)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)
include(CodeCoverage)

set(GCOVR_EXCLUDE "build/*" "tests/*" "src/main.c")

append_coverage_compiler_flags()
setup_target_for_coverage_gcovr_xml(
  NAME
  mse_coverage
  EXECUTABLE
  mtg-search-engine-tests
  test
  EXCLUDE
  ${GCOVR_EXCLUDE})

set(LIBS jansson CURL::libcurl pthread m c)

add_library(mtg-search-engine ${SRC})
target_link_libraries(mtg-search-engine PRIVATE ${LIBS})
target_include_directories(mtg-search-engine PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
                                                    ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(mtg-search-engine-cli ./src/main.c)
target_link_libraries(mtg-search-engine-cli ${LIBS} mtg-search-engine)
target_include_directories(
  mtg-search-engine-cli PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
                               ${CMAKE_CURRENT_SOURCE_DIR})

if(UNIX)
  add_executable(mtg-search-engine-tests ${TESTS})
  target_link_libraries(mtg-search-engine-tests PUBLIC ${LIBS}
                                                       mtg-search-engine)
  target_include_directories(mtg-search-engine-tests
                             PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
  target_include_directories(
    mtg-search-engine-tests PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR})

  # Copy test data
  set(TEST_CARD_JSON ${CMAKE_CURRENT_BINARY_DIR}/test_card.json)

  set(MEM_TESTS_SH ${CMAKE_CURRENT_BINARY_DIR}/mem_tests.sh)
  set(MEM_TESTS_PY ${CMAKE_CURRENT_BINARY_DIR}/mem_tests.py)
  set(TESTING_SCRIPTS ${MEM_TESTS_PY} ${MEM_TESTS_SH})

  set(TEST_DATA_FILES ${TEST_CARD_JSON} ${TESTING_SCRIPTS})
  add_custom_command(
    OUTPUT ${TEST_DATA_FILES}
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_card.json ${TEST_CARD_JSON}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/mem_tests.sh
            ${MEM_TESTS_SH}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/mem_tests.py
            ${MEM_TESTS_PY})
  add_custom_target(TEST_DATA DEPENDS ${TEST_DATA_FILES})
  add_dependencies(mtg-search-engine-tests TEST_DATA)

  # Testing
  enable_testing()
  add_test(NAME main-tests COMMAND mtg-search-engine-tests)
  add_test(NAME memory-tests COMMAND bash ${MEM_TESTS_SH})
endif()
