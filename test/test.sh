for dir in */; do
    cd $dir
    ./test.sh | grep failed
    rc=$?
    if [[ rc != 0 ]]; then
        echo "$dir passed!"
    fi
    cd ..
done
