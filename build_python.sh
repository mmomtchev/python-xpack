#!/bin/bash

set -x
unset MAKEFLAGS
cd `dirname $0`

export PYTHON_DIST=$(pwd)/dist
export PYTHON_BUILD=$(pwd)/build
export PYTHON_VERSION=$1
export PYTHON_DEST=$(pwd)/content
cd deps/static-portable-python
bash build_python.sh ${PYTHON_DEST}
