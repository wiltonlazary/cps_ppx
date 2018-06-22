#!/bin/bash
echo "" > tests/src/native/IndexNativeCpsed.re
make test &> test.out.txt ; cat test.out.txt