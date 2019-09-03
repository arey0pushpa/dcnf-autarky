#!/bin/bash

# Download the files instances locally
#git clone https://qbflib.sagelab.it/qbflib/qdimacs

mkdir -p Autarkies Database Experiments

# Create a database: direcetory structure similar to tht of the instances
cd qdimacs
find . -type d -o -type f -exec bash -c '
  for path; do mkdir -p "../Database/${path/file/folder}"; done
' bash {} +

# Iterate over each file and create a directory  
for filename in qdimacs/*.dqdimacs; do
  while IFS= read -r line;do
    if [[ $line =~ ^[[:space:]]*p.* ]]; then
      echo "$line"
    fi
  done < "$filename"
done
