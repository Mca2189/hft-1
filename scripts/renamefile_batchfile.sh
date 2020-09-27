#!/bin/bash

ds=`ls /running/csv/ | grep 20 | grep -`

for d in $ds
do
	echo "handling /running/csv/$d"
	rename " " "" /running/csv/$d/*
done
