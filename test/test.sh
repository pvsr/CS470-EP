#!/bin/sh
for dir in fptp list preferential stv; do
    cd $dir
    ./test.sh | grep failed
    rc=$?
    if [[ $rc != 0 ]]; then
        echo "$dir passed!"
    fi
    cd ..
done
