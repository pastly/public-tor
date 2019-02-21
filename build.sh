#!/usr/bin/env bash
set -eu
./autogen.sh
export CFLAGS="-g"
./configure --disable-asciidoc
make -j8 src/app/tor
