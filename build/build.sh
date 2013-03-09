#!/bin/bash -e
export ROOT_DIR=../
export SRC_DIR=${ROOT_DIR}/src
export BIN_DIR=${ROOT_DIR}/bin
make -e -I ${ROOT_DIR}/build $@
