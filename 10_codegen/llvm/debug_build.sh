#!/bin/bash

set -x
BINDIR=../../bin/build_codegen
PROJBIN=codegen
BUILDDIR=_built_d
BUILDTYPE=Debug
mkdir -p ${BINDIR}
export MAKEFLAGS=-j$(nproc)
export ASAN_OPTIONS=abort_on_error=1:disable_coredump=0:unmap_shadow_on_exit=1
cd cmake && cmake -DCMAKE_BUILD_TYPE=${BUILDTYPE} -H. -B../${BINDIR}/${BUILDDIR} && cd ../${BINDIR}/${BUILDDIR}/ && make $1 && cp arash ../../${PROJBIN}
