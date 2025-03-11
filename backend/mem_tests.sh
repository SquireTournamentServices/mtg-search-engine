#!/bin/bash

./mtg-search-engine-tests help | grep "|" | grep -v "Arg | Description" | grep -Eo "[0-9]+ |" | grep -Eo '[0-9]+' | xargs -n1 python3 mem_tests.py

