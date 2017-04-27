#!/bin/sh

for n in 1 2 4 8 16 32; do
	echo ====$n==== | tee -a stv_weak.results
	salloc -Q -n $n mpirun ../../../votecounter $n -m stv -s 3 -T | grep count | tee -a stv_weak.results
	salloc -Q -n $n mpirun ../../../votecounter $n -m stv -s 3 -T | grep count | tee -a stv_weak.results
done
mv stv_weak.results ..
