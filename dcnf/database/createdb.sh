#!/bin/bash

# HARDEST Instance (PCNF track): C6_BMC_p1_k2048.qdimacs

# USE -> GNU Parallel
# Download the files instances locally
# settings/change this to your config
#git clone https://qbflib.sagelab.it/qbflib/qdimacs && \
#git https://qbflib.sagelab.it/qbflib/dqbf

URL="https://qbflib.sagelab.it/qbflib/dqbf"
FOLDER="dqbf"

# Can have race condition so implement a thread safe code
if [ ! -d "$FOLDER" ] ; then
    git clone $URL $FOLDER
else
    cd "$FOLDER"
    git pull $URL
fi

mkdir -p Autarkies Database Experiments

# Create a database: directory structure similar to tht of the instances
# TODO: Handle Path and make each of them variable
find . -type d -o -type f -exec bash -c '
  for path; do mkdir -p "../Database/${path/file/folder}"; done
' bash {} +

# Instantiate a file with instance file name and add basic info
shopt -s globstar

for i in ./**/*
do
  if [ -f "$i" ];
  then
    if [[ ${i##*/} == *.dqdimacs ]];
    then
      touch "../Database${i#.}/${i##*/}" 
      while IFS= read -r line;do
        if [[ $line =~ ^[[:space:]]*p.* ]]; then
          arr=($line)
          echo -n "${i##*/} " > ../Database${i#.}/${i##*/}
          echo -n ${i##*/} | sha1sum | awk '{printf $1}' >> ../Database${i#.}/${i##*/}
          echo -n " " >> ../Database${i#.}/${i##*/} 
          echo -n "${arr[2]} " >> ../Database${i#.}/${i##*/} 
          echo -n ${arr[3]} >> ../Database${i#.}/${i##*/}
          break
        fi
      done < "$i"
    fi
  fi
done
