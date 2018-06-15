#!/bin/sh

FILE=$1
STYLE=Google

clang-format -i -style=$STYLE $FILE
