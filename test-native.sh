#!/bin/bash
#echo "" > tests/src/native/IndexNativeCpsed.re
#make test &> test.out.txt ; cat test.out.txt
cd tests
echo "" > src/native/IndexNativeCpsed.re
bsb -make-world -clean-world -backend native && lib/bs/native/indexnative.native
