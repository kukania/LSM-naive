#!/bin/bash
DIR=~/SOSP23/fragmentation/parsed
RES=./results
EXE=./naive_lsm

for i in $(ls $DIR/*.res)
do
	workload=$(echo "$i" | cut -d'-' -f2)
	age=$(echo "$i" | cut -d'-' -f3)
	echo "$EXE $i >log-$workload-$age 2> $RES/mem-$workload-$age"
	#$EXE $i 2> $RES/mem-$workload-$age
done

