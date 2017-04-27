#!/bin/sh

for n in 1 2 4 8 16 32; do
	echo ====$n==== | tee -a fptp_weak.results
	salloc -Q -n $n mpirun ../../../votecounter $n -T | grep count | tee -a fptp_weak.results
	salloc -Q -n $n mpirun ../../../votecounter $n -T | grep count | tee -a fptp_weak.results
done
mv fptp_weak.results ..
