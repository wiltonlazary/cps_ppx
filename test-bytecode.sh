#!/bin/bash
cd tests
echo "" > src/bytecode/IndexBytecodeCpsed.re
bsb -make-world -clean-world -backend bytecode && lib/bs/bytecode/indexbytecode.byte