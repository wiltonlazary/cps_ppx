#!/bin/bash
cd $(dirname $0)
unset GIT_WORK_TREE
unset GIT_DIR
base=$(pwd)

ocamlVersion="4.02.3+buckle-master"
nodeVersion="10.5.0"

opam init --y
opam switch create $ocamlVersion --y &> /dev/null
opam switch $ocamlVersion --y
eval $(opam env)
opam install ppx_tools --y
opam install ppx_tools_versioned --y
opam install ocaml-migrate-parsetree --y
opam install yojson --y

unset NVM_DIR
wget -qO- https://raw.githubusercontent.com/creationix/nvm/v0.33.11/install.sh | bash
export NVM_DIR="$HOME/.nvm"
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"
nvm install node v$nodeVersion
nvm use --delete-prefix v$nodeVersion

mkdir -p ~/build &> /dev/null
cd ~/build
build=$(pwd)
git clone https://github.com/bsansouci/bsb-native.git
cd bsb-native
git pull
make && make install
cd $base

npm install -g bs-platform@file:$build/bsb-native
npm install -g json
npm install -g create-react-native-app
npm install -g reason-cli
npm link bs-platform
bash