#!/bin/sh

for dir in strong weak; do
	cd $dir
	./fptp.sh
	./list.sh
	cd ..
done
