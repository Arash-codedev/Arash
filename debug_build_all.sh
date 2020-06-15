#!/bin/bash

set -e

cwd=$(pwd)
cd $cwd/01_streamer
./debug_build.sh
cd $cwd/02_tokenizer
./debug_build.sh
cd $cwd/03_blocker
./debug_build.sh
cd $cwd/04_abstractor-init
./debug_build.sh
cd $cwd/05_pertainer
./debug_build.sh
cd $cwd/06_injector
./debug_build.sh
cd $cwd/07_resolver
./debug_build.sh
cd $cwd/08_optimizer
./debug_build.sh
cd $cwd/09_reformer
./debug_build.sh
cd $cwd/10_codegen/llvm
./debug_build.sh
