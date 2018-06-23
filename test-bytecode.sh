#!/bin/bash
cd tests
echo "" > src/js/IndexJsCpsed.re
bsb -make-world -clean-world -backend bytecode
#ocamlrun lib/bs/bytecode/indexbytecode.byte