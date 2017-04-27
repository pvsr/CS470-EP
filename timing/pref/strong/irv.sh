#!/bin/sh

for n in 1 2 4 8 16 32; do
	echo ====$n==== | tee -a irv_strong.results
	salloc -Q -n $n mpirun ../../../votecounter $n -m irv -T | grep count | tee -a irv_strong.results
	salloc -Q -n $n mpirun ../../../votecounter $n -m irv -T | grep count | tee -a irv_strong.results
done
mv irv_strong.results ..
