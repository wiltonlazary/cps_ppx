#!/bin/bash
echo "USING PROJECT ENV: .env.sh"
export PATH=node_modules/.bin:$PATH
export NODE_PATH=build:./:node_modules
export NODE_OPTIONS=""
export NODE_ENV="development"