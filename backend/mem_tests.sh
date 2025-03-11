#!/bin/bash

./mtg-search-engine-tests help | grep "|" | grep -v "Arg | Description" | xargs -n1 python3 mem_tests.py

