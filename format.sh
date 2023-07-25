#!/bin/bash

find ./mse ./tests ./web-api | grep ".*\\.[ch]\$" | xargs astyle --style=kr
cmake-format ./CMakeLists.txt > ./CMakeLists.txt.orig && cp ./CMakeLists.txt.orig ./CMakeLists.txt
black *.py

