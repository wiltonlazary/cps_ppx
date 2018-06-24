#!/bin/bash

if [ ! -d "red" ]
then
  unset GIT_WORK_TREE
  unset GIT_DIR
  git clone https://github.com/frantic/red.git
fi

cd tests
echo "" > src/bytecode/IndexBytecodeCpsed.re
bsb -color -make-world -clean-world -backend bytecode
cd lib/bs/bytecode/
../../../../red/red.py indexbytecode.byte