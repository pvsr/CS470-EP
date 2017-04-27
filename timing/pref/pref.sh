#!/bin/sh

for dir in strong weak; do
	cd $dir
	./irv.sh
	./stv.sh
	cd ..
done
