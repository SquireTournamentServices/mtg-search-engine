cmake_minimum_required(VERSION 3.9)

project(
  mtg-search-engine
  VERSION 1.0
  LANGUAGES C CXX)

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

# __FILENAME__
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
add_definitions("-D__FILENAME__=(&__FILE__[SOURCE_PATH_SIZE])")

if (NOT WIN32)
  set(CMAKE_C_FLAGS "-Wpedantic -Werror -Wno-unused-parameter -g -Og")
endif()

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

# Packages
find_package(CURL)

# Jansson find_package is broken On non-unix systems.
# find_package(jansson)

# you need to provide your own pthreads implementation Have a look at win32 pthreads for example

find_package(absl)
set(ABSL_LIBS
    absl::base
    absl::core_headers
    absl::fixed_array
    absl::flags
    absl::flat_hash_map
    absl::flat_hash_set
    absl::inlined_vector
    absl::optional
    absl::span
    absl::str_format
    absl::strings)

add_subdirectory(./re2)

# Parser generators
find_package(BISON)
find_package(FLEX)

option(USE_JEMALLOC OFF)
set(LIBS
    jansson
    CURL::libcurl
    pthread
    ${ABSL_LIBS}
    re2)
if(USE_JEMALLOC)
  set(LIBS ${LIBS} jemalloc)
endif()

if(WIN32)
  set(ADDITIONAL_FLEX_FLAGS "--wincompat")
else()
  set(ADDITIONAL_FLEX_FLAGS "")
  set(LIBS ${LIBS} m)
endif()

flex_target(
  query_lexer mse/query_parser.l ${CMAKE_CURRENT_BINARY_DIR}/mse_query_lexer.c
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/mse_query_lexer.h
  COMPILE_FLAGS ${ADDITIONAL_FLEX_FLAGS})
bison_target(query_parser mse/query_parser.y
             ${CMAKE_CURRENT_BINARY_DIR}/mse_query_parser.c)
add_flex_bison_dependency(query_lexer query_parser)

set(PARSER_SRC mse/query_parser.h ${BISON_query_parser_OUTPUTS}
               ${FLEX_query_lexer_OUTPUTS})

# Define the sources
set(SRC
    ./testing_h/ansi_colour.h
    ./testing_h/testing.h
    ./testing_h/testing.c
    ./testing_h/logger.h
    ./testing_h/logger.c
    ./mse/card.h
    ./mse/card.c
    ./mse/card_str_match.h
    ./mse/card_str_match.c
    ./mse/card_txt_fields_trie.h
    ./mse/card_txt_fields_trie.c
    ./mse/set.h
    ./mse/set.c
    ./mse/uuid.h
    ./mse/uuid.c
    ./mse/mtg_json.h
    ./mse/mtg_json.c
    ./mse/mtg_json_indexes.h
    ./mse/mtg_json_indexes.c
    ./mse/thread_pool.h
    ./mse/thread_pool.c
    ./mse/io_utils.h
    ./mse/io_utils.c
    ./mse/avl_tree.h
    ./mse/avl_tree.c
    ./mse/search.h
    ./mse/search.c
    ./mse/generator_set.h
    ./mse/generator_set.c
    ./mse/generators.h
    ./mse/generators.c
    ./mse/generator_double_fields.h
    ./mse/generator_double_fields.c
    ./mse/generator_oracle.h
    ./mse/generator_oracle.c
    ./mse/generator_colour.h
    ./mse/generator_colour.c
    ./mse/generator_name.h
    ./mse/generator_name.c
    ./mse/interpretor.h
    ./mse/interpretor.c
    ./mse/consumer.h
    ./mse/consumer.c
    ./mse/save.h
    ./mse/save.c
    ./mse/mse.h
    ./mse/mse.c
    ./mse/re2_wrapper.h
    ./mse/re2_wrapper.cpp
    ./strptime/strptime.h
    ./strptime/strptime.c
    ${PARSER_SRC}
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
    ./tests/test_generator_txt.c
    ./tests/test_generator_colours.c
    ./tests/test_consumer.c
    ./tests/test_interpretor.h
    ./tests/test_interpretor.c
    ./tests/test_parser.h
    ./tests/test_parser.c
    ./tests/test_mse_langs.h
    ./tests/test_mse_langs.c
    ./tests/test_save.h
    ./tests/test_save.c
    ./tests/test_re2.h
    ./tests/test_re2.c
    ./tests/system_test.h
    ./tests/system_test.c)
set(WEB_API
    ./web-api/main.c
    ./web-api/server.c
    ./web-api/server.h
    ./web-api/async_query.h
    ./web-api/async_query.c
    ./mongoose/mongoose.c
    ./mongoose/mongoose.h)

# Setup gcovr
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/scripts/cmake)

if(UNIX)
  # Setup docs
  find_package(doxygen)
  if(DOXYGEN_FOUND)
    find_package(doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)
    doxygen_add_docs(mse-docs ${SRC} ${TESTS}
                     "Generate documentation pages for ${REPO_URL}")
  endif()

  include(CodeCoverage)

  set(GCOVR_EXCLUDE "build/*" "tests/*" "mse/main.c")

  append_coverage_compiler_flags()
  setup_target_for_coverage_gcovr_xml(
    NAME
    mse-coverage
    EXECUTABLE
    mtg-search-engine-tests
    test
    EXCLUDE
    ${GCOVR_EXCLUDE})
endif()

# Add library and cli execs
add_library(mtg-search-engine OBJECT ${SRC})
target_link_libraries(mtg-search-engine PRIVATE ${LIBS})
target_include_directories(mtg-search-engine PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
                                                    ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(mtg-search-engine-cli mse/main.c)
target_link_libraries(mtg-search-engine-cli ${LIBS} mtg-search-engine)

option(MSE_WEB_API OFF)
if(MSE_WEB_API)
  add_executable(mtg-search-engine-web-api ${WEB_API})
  target_link_libraries(
    mtg-search-engine-web-api
    ${LIBS}
    crypto
    mbedtls
    mbedcrypto
    mbedx509
    mtg-search-engine)
endif()

# Setup unit tests
if(UNIX)
  add_executable(mtg-search-engine-tests ${TESTS})
  target_link_libraries(mtg-search-engine-tests PUBLIC ${LIBS}
                                                       mtg-search-engine)

  # Copy test data
  set(TEST_CARD_JSON ${CMAKE_CURRENT_BINARY_DIR}/test_card.json)
  set(TEST_VALID_QUERIES ${CMAKE_CURRENT_BINARY_DIR}/valid_queries.txt)
  set(TEST_INVALID_QUERIES ${CMAKE_CURRENT_BINARY_DIR}/invalid_queries.txt)

  set(MEM_TESTS_SH ${CMAKE_CURRENT_BINARY_DIR}/mem_tests.sh)
  set(MEM_TESTS_PY ${CMAKE_CURRENT_BINARY_DIR}/mem_tests.py)
  set(TESTING_SCRIPTS ${MEM_TESTS_PY} ${MEM_TESTS_SH})

  set(TEST_DATA_FILES
      ${TEST_CARD_JSON} ${TESTING_SCRIPTS} ${TEST_VALID_QUERIES}
      ${TEST_INVALID_QUERIES} ${ALL_PRINTINGS})
  add_custom_command(
    OUTPUT ${TEST_DATA_FILES}
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_card.json ${TEST_CARD_JSON}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/mem_tests.sh
            ${MEM_TESTS_SH}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/mem_tests.py
            ${MEM_TESTS_PY}
    COMMAND
      ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/valid_queries.txt ${TEST_VALID_QUERIES}
    COMMAND
      ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/invalid_queries.txt
      ${TEST_INVALID_QUERIES})
  add_custom_target(TEST_DATA DEPENDS ${TEST_DATA_FILES})
  add_dependencies(mtg-search-engine-tests TEST_DATA)

  # Testing
  enable_testing()
  add_test(NAME main-tests COMMAND mtg-search-engine-tests)
  add_test(NAME memory-tests COMMAND bash ${MEM_TESTS_SH})
  add_test(NAME parser-tests COMMAND mtg-search-engine-parser-tests)
endif()
