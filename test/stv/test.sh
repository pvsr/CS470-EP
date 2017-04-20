#!/bin/sh
for t in *.test; do
    ./"$t"
    rc=$?
    if [[ $rc != 0 ]]; then
        echo "stv/$t failed!";
    fi
done
