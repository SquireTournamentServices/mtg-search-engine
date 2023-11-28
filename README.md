# mtg-search-engine
This is an open source library and, standalone application that supports the searching of MTG cards
and sets. It has a syntax that is similar to Scryfall but it can be embedded anywhere.

**Work in progress**

[![Frontend](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/frontend.yml/badge.svg)](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/frontend.yml)
[![Main](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/main.yml/badge.svg)](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/main.yml)
[![Memory tests](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/memtests.yml/badge.svg)](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/memtests.yml)
[![Coverage](https://github.com/SquireTournamentServices/mtg-search-engine/actions/workflows/coverage.yml/badge.svg)](https://github.com/SquireTournamentServices/mtg-search-engine/actions/workflows/coverage.yml)
[![codecov](https://codecov.io/github/SquireTournamentServices/mtg-search-engine/graph/badge.svg?token=FK7LTBC9AC)](https://codecov.io/github/SquireTournamentServices/mtg-search-engine)

## Requirements
 - cmake+
 - openssl+
 - cURL+
 - jansson+
 - abseil+
 - c compiler *(i.e: gcc)*
 - c++ compiler *(i.e: g++)*
 - <!--nerf-->bison
 - flex
 - pthread *(see win32 pthread)*
 - python3
  - `pip install -r requirements.txt`

> + these can be included via submodules, see the SquireDesktop repo for an example

### Optional Requirements
 - astyle
 - cmake-formatter
 - ctest
 - valgrind
 - libmedtls *(web-api)*

## Building and Testing

This project is written in C and, targets all platforms. There is an optional frontend and web API for this project, these can simply not be built.

### Backend / Library / CLI

```sh
# .
mkdir -p build
cd build

# ./build/
# You can use -DMSE_WEB_API=ON to build the web api
# You can use -DUSE_JEMALLOC=ON to build with Jemalloc (recommended)
cmake .. 
cmake --build . -j

ctest -V -j # Runs all the tests
```

#### Testing on poor internet

Local test flag for slow internet: `MSE_TEST`, use this to make the program always read a cached copy of AllPrintings.json, 
really useful when running on crap internet.

```sh
export MSE_TEST=true
./mtg-search-engine-tests
```

### Frontend

The frontend uses NextJS with the app dir.

```sh
cd frontend
npm i
npm build
```
