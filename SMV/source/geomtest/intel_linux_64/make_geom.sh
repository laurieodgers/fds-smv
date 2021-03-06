#!/bin/bash
platform=intel64
dir=`pwd`
target=${dir##*/}

source $IFORT_COMPILER/bin/compilervars.sh $platform

echo Building $target
rm -f *.o *.mod
make VPATH=".." -f ../Makefile $target
