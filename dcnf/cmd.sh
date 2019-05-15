#!/bin/bash
echo "filename pn pc sha1 autarky assmt result time" > "file.out" 
for file in ./examples/dqbf18/*
do
	echo "$file" >> "file.out"
  ./dcnf_autarky -i "$file" -r 3 >> "file.out"
	echo "" >> "file.out"
done
