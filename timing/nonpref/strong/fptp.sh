#!/bin/sh

for n in 1 2 4 8 16 32; do
	echo ====$n==== | tee -a fptp_strong.results
	salloc -Q -n $n mpirun ../../../votecounter $n -T | grep count | tee -a fptp_strong.results
	salloc -Q -n $n mpirun ../../../votecounter $n -T | grep count | tee -a fptp_strong.results
done
mv fptp_strong.results ..
