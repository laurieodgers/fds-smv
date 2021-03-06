#!/bin/bash
source $IFORT_COMPILER/bin/compilervars.sh intel64
KWDIR=../../../Utilities/keyword
SDIR=../../source

source $KWDIR/expand_file.sh $KWDIR $SDIR/smokeview $SDIR/shared/string_util.c
make -f ../Makefile clean
make -f ../Makefile intel_osx_64_db
source $KWDIR/contract_file.sh $KWDIR $SDIR/smokeview/main.c
