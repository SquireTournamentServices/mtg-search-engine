#!/bin/bash

find ./mse ./tests* | grep ".*\\.[ch]\$" | xargs astyle --style=kr
find ./mse ./tests* | grep ".*\\.cpp\$" | xargs astyle --style=kr
cmake-format ./CMakeLists.txt > ./CMakeLists.txt.orig && cp ./CMakeLists.txt.orig ./CMakeLists.txt
black *.py

