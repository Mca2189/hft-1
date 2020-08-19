#!/bin/bash

for file in `ls | grep future_`
do
        newfile=${file/_/}
        #echo $file
        mv $file $newfile
done
