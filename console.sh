#!/bin/bash
unset GIT_WORK_TREE
unset GIT_DIR
cd $(dirname $0)
source .env.sh
bash