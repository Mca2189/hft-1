#!/bin/bash

for f in `find /running/csv/ -name "* *.csv"`
na=$(echo $name | tr ' ' ‘-') 
do
	echo $f
	command="gzip -f "$f""
	echo $command
	#eval $command
done
