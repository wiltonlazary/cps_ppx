#!/bin/bash
cd tests
echo "" > src/js/IndexJsCpsed.re
bsb -make-world -clean-world -backend js && node lib/js/src/js/IndexJs.js