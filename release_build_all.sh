#!/bin/bash

set -e

cwd=$(pwd)
cd $cwd/01_streamer
./release_build.sh
cd $cwd/02_tokenizer
./release_build.sh
cd $cwd/03_blocker
./release_build.sh
cd $cwd/04_abstractor-init
./release_build.sh
cd $cwd/05_pertainer
./release_build.sh
cd $cwd/06_injector
./release_build.sh
cd $cwd/07_resolver
./release_build.sh
cd $cwd/08_optimizer
./release_build.sh
cd $cwd/09_reformer
./release_build.sh
cd $cwd/10_codegen/llvm
./release_build.sh
