#!/bin/bash

mkdir -p build/stub
cd build/stub
make -f ../../../../platform/winstub/Makefile "$@"
