#!/bin/bash

FILE="$1"
cp "$FILE" test_file.c

shift

build/MPIMutationTool -extra-arg="-I/usr/lib/x86_64-linux-gnu/openmpi/include" -extra-arg="-I/usr/lib/x86_64-linux-gnu/openmpi/include/openmpi" -extra-arg="-L/usr/lib/x86_64-linux-gnu/openmpi/lib" -extra-arg="-lmpi" $@ test_file.c

clang-format -i test_file.c
diff "$FILE" test_file.c
