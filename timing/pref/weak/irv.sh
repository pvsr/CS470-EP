#!/bin/sh

for n in 1 2 4 8 16 32; do
	echo ====$n==== | tee -a irv_weak.results
	salloc -Q -n $n mpirun ../../../votecounter $n -m irv -T | grep count | tee -a irv_weak.results
	salloc -Q -n $n mpirun ../../../votecounter $n -m irv -T | grep count | tee -a irv_weak.results
done
mv irv_weak.results ..
