#!/bin/bash

set -x
BINDIR=../bin/build_resolver
PROJBIN=resolver
BUILDDIR=_built_d
BUILDTYPE=Debug
mkdir -p ${BINDIR}
export MAKEFLAGS=-j$(nproc)
cd cmake && cmake -DCMAKE_BUILD_TYPE=${BUILDTYPE} -H. -B../${BINDIR}/${BUILDDIR} && cd ../${BINDIR}/${BUILDDIR}/ && make $1 && cp arash ../../${PROJBIN}
