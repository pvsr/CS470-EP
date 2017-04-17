for t in *.test; do
    ./"$t"
    rc=$?
    if [[ $rc != 0 ]]; then
        echo "preferential/$t failed!";
    fi
done
