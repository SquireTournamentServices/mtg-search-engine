{ pkgs ? import <nixpkgs> { } }:
pkgs.mkShell
{
  nativeBuildInputs = with pkgs; [
    # Build tools
    cmake
    cmake-format
    bash # never know when someone has a fucked up install
    curl
    git

    # Code generation
    python312
    python312Packages.pip
    python312Packages.virtualenv

    # Test and debugging
    valgrind
    gcovr
    gdb
    kdePackages.kcachegrind

    # Compiler
    clang # Mainly for clangd, also works as a compiler
    gcc

    # Libraries
    jemalloc
    mbedtls
    openssl
    jansson

    # Frontend
    nodejs_22
  ];
  shellHook = ''
    ROOT_DIR=$(pwd)
    BACKEND_DIR="./backend"
    VENV_DIR="$BACKEND_DIR/.venv"

    cd "$BACKEND_DIR" || { echo "Error: Could not change to $BACKEND_DIR. Does it exist?"; exit 1; }
    if [ ! -d "$VENV_DIR" ]; then
      echo "Virtual environment not found. Creating one in $VENV_DIR..."
      python3 -m venv "$VENV_DIR"
    fi

    source "$VENV_DIR/bin/activate"
    pip install -r requirements.txt
    cd "$ROOT_DIR"

    cd ./frontend/
    pnpm i
    cd "$ROOT_DIR"

    echo "mtg-search-engine development environment activated"
  '';
}
