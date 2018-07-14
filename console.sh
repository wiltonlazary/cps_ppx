#!/bin/bash
cd $(dirname $0)
unset GIT_WORK_TREE
unset GIT_DIR
source .env.sh
bash