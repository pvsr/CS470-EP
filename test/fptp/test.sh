for t in *.test; do
    echo $t
    ./"$t"
    rc=$?
    if [[ $rc != 0 ]]; then
        echo "fptp/$t failed!";
    fi
done
