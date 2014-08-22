#!/bin/bash

# I have a dir on the lustre FS with 100,000 files on which I'm running
# the following loop:

while true; do
    echo Slab iteration $(( ++i ))
    find . -type f | xargs -n 100 ls -l > /dev/null 2>&1
done

