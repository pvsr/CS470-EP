#!/bin/sh

for n in 1 2 4 8 16 32; do
	echo ====$n==== | tee -a stv_strong.results
	salloc -Q -n $n mpirun ../../../votecounter $n -m stv -s 3 -T | grep count | tee -a stv_strong.results
	salloc -Q -n $n mpirun ../../../votecounter $n -m stv -s 3 -T | grep count | tee -a stv_strong.results
done
mv stv_strong.results ..
