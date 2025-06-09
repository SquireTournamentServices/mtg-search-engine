# mtg-search-engine

This is an open source library and, standalone application that supports the searching of MTG cards
and sets. It has a syntax that is similar to Scryfall but it can be embedded anywhere.

[![Frontend](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/frontend.yml/badge.svg)](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/frontend.yml)
[![Main](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/main.yml/badge.svg)](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/main.yml)
[![Memory tests](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/memtests.yml/badge.svg)](https://github.com/MonarchDevelopment/mtg-search-engine/actions/workflows/memtests.yml)
[![Coverage](https://github.com/SquireTournamentServices/mtg-search-engine/actions/workflows/coverage.yml/badge.svg)](https://github.com/SquireTournamentServices/mtg-search-engine/actions/workflows/coverage.yml)
[![codecov](https://codecov.io/github/SquireTournamentServices/mtg-search-engine/graph/badge.svg?token=FK7LTBC9AC)](https://codecov.io/github/SquireTournamentServices/mtg-search-engine)
[![API Tests](https://github.com/SquireTournamentServices/mtg-search-engine/actions/workflows/apitests.yml/badge.svg)](https://github.com/SquireTournamentServices/mtg-search-engine/actions/workflows/apitests.yml)

## Requirements

**You can use the Nix shell (`nix-shell shell.nix`) to set the environment up for you!** ([A helpful blog post about Nix shells, and what they are](https://ghedam.at/15978/an-introduction-to-nix-shell)).
If you use the Nix shell then you can skip these steps.

- cmake+
- openssl+
- cURL+
- jansson+
- c compiler _(i.e: gcc, msvc, clang)_
- c++ compiler _(i.e: g++, msvc, clang)_
- pthread _(see win32 pthread)_
- python3
- `pip install -r requirements.txt`

> - these can be included via submodules, see the SquireDesktop repo for an example

### Optional Requirements

- astyle
- cmake-formatter
- ctest
- valgrind
- libmedtls _(web-api)_

## Building and Testing

This project is written in C and, targets all platforms\*. There is an optional frontend and web API for this project, these can simply not be built.

\* The Nix shell only works on Linux/WSL.

### Backend / Library / CLI

```sh
# You the Nix shell to install things for you
nix-shell shell.nix

cd backend/
# If you do not use the nix-shell then you should be in a python virtual environment
# python -m venv .
# If you are on windows or use fish, you must change this line
# source bin/activate

pip install -r requirements.txt

# .
mkdir -p build
cd build

# ./build/
# You can use -DMSE_DOXYGEN=ON to generate documentation
# You can use -DUSE_GCOV=OFF to disable gcov
# You can use -DDEBUG=OFF to disable debug builds (it is on by default as usable coredumps make me happy)
# You can use -DADDRESS_SANITISER=ON to enable address sanitiser
cmake -DUSE_JEMALLOC=ON ..
cmake --build . -j

ctest -V -j # Runs all the tests
```

#### Testing on poor internet

Local test flag for slow internet: `MSE_TEST`, use this to make the program always read a cached copy of AllPrintings.json,
really useful when running on crap internet.

```sh
# Forces the local JSON file to be loaded instead of calling to MTG JSON to download it
export MSE_TEST=true
./mtg-search-engine-tests
```

### Frontend

The frontend uses NextJS with the app dir.

```sh
# You the Nix shell to install things for you
nix-shell shell.nix

cd frontend/
# If you do not use the nix-shell (or change dependancies) then you should setup you node_modules
# pnpm i

pnpm build
```

### Deploy Using Docker

```bash
# Starts the backend on http://localhost:4365
#   and the frontend on http://localhost:3000
docker compose up

# Since the frontend is NextJS it talks directly to the server (hence the network bridge),
# this means that you do not need to expose the backend to the internet, or worry about CORS!!
```

You can also use [Helm](https://helm.sh/) to deploy it into your Kubernetes cluster, this is a lot more complicated and only recommended for people who know what they are doing.
