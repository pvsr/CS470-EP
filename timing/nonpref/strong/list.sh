#!/bin/sh

for n in 1 2 4 8 16 32; do
	echo ====$n==== | tee -a list_strong.results
	salloc -Q -n $n mpirun ../../../votecounter $n -m list -s 300 -T | grep count | tee -a list_strong.results
	salloc -Q -n $n mpirun ../../../votecounter $n -m list -s 300 -T | grep count | tee -a list_strong.results
done
mv list_strong.results ..
