# mtg-search-engine
This is an open source library and, standalone application that supports the searching of MTG cards
and sets. It has a syntax that is similar to Scryfall but it can be embedded anywhere.

**Work in progress**

[![Coverage](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/coverage.yml/badge.svg)](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/coverage.yml)
[![Main](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/main.yml/badge.svg)](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/main.yml)
[![Memory tests](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/memtests.yml/badge.svg)](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/memtests.yml)
[![codecov](https://codecov.io/gh/MonarchDevelopment/mtg-search-engine/branch/main/graph/badge.svg?token=FK7LTBC9AC)](https://codecov.io/gh/MonarchDevelopment/mtg-search-engine)

## Requirements
 - openssl
 - cmake
 - cURL
 - jansson
 - c compiler *(i.e: gcc)*
 - <!--nerf-->bison
 - flex
 - python3
  - `pip install -r requirements.txt`

### Optional Requirements
 - astyle
 - cmake-formatter
 - ctest
 - valgrind

## Building and Testing
This project is written in C and, targets all platforms. 

```
# .
mkdir -p build
cd build

# ./build/
cmake .. # -DCMAKE_BUILD_TYPE=DEBUG # Debug mode ??
cmake --build . -j

# ctest -V -j # Run tests ??
```

### Testing on poor internet
Local test flag for slow internet: `MSE_TEST`, use this to make the program always read a cached copy of AllPrintings.json, 
really useful when running on crap internet.

```sh
export MSE_TEST=true
./mtg-search-engine-tests
```
