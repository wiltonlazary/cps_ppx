#!/bin/bash
refmt --print=binary tests/src/bytecode/IndexBytecode.re | ./cps_ppx.exe /dev/stdin /tmp/cps-ppx.out.tmp
refmt --print-width=140 --parse=binary --print=re /tmp/cps-ppx.out.tmp > tests/src/bytecode/IndexBytecodeCpsed.re