#!/bin/sh

for dir in include source pyircbot plugins program tests; do
    find $dir -name '*.hpp' -exec ./format.sh {} \;
    find $dir -name '*.cpp' -exec ./format.sh {} \;
done
