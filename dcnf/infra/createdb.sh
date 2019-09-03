#!/bin/bash

# Download the files instances locally
# wget http://www.qbflib.org/QBFEVAL_18_DATASET.zip -cP ./

mkdir -p Database 

# Iterate over each file and create a directory  
for filename in DQBF/*.dqdimacs; do
    [ -e "$filename" ] || continue
		cd Database
		touch $filename
done

