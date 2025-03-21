{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {    
    nativeBuildInputs = with pkgs.buildPackages; [
      # Build tools
      cmake
      cmake-format
      bash # never know when someone has a fucked up install
      curl
      git

      # Code generation
      python312
      python312Packages.pip

      # Test
      # valgrind
      gcovr
      lldb

      # Compiler
      gcc
      clang

      # Libraries
      jemalloc
      mbedtls
      openssl
      jansson

      # Frontend
      nodejs_22
    ];
  }
