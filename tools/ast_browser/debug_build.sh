#!/bin/bash

set -x
BINDIR=../../bin/build_tools_ast_browser
PROJBIN=ast_browser
BUILDDIR=_built_d
BUILDTYPE=Debug
mkdir -p ${BINDIR}
export MAKEFLAGS=-j$(nproc)
cp -r html_resources ../../bin/
cd cmake && cmake -DCMAKE_BUILD_TYPE=${BUILDTYPE} -H. -B../${BINDIR}/${BUILDDIR} && cd ../${BINDIR}/${BUILDDIR}/ && make $1 && cp ast_browser ../../${PROJBIN}
