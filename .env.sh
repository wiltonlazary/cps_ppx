#!/bin/bash
cd $(dirname $0)
base=$(pwd)
echo "USING PROJECT ENV: .env.sh"
export PATH=$base/node_modules/.bin:$PATH
export NODE_PATH=build:./:node_modules
export NODE_OPTIONS=""
export NODE_ENV="development"