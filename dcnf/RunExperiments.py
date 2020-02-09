#!/usr/bin/python3

import sys
import glob
import os
import re
import shutil
import subprocess
import hashlib
from pathlib import Path

if len(sys.argv) != 2:
    print("Incorrect number of arguments. use -h for more help.")
    sys.exit(0)

if sys.argv[1] == "-h" or sys.argv[1] == "--help":
    print("./RunExperiments [-h] [cnf_instances_directory]")
    sys.exit(0)

inputpath = sys.argv[1]

# Create three directories 
Path("./AutarkiesQEF").mkdir(parents=True, exist_ok=True)
Path("./DatabaseQEF").mkdir(parents=True, exist_ok=True)
Path("./ExperimentsQEF").mkdir(parents=True, exist_ok=True)

outputpath1 = './DatabaseQEF/'
outputpath2 = './ExperimentsQEF/'
suffix = '.R'

print ("c Starting the directory structure setup.")

# Create the directory structure similar to the input file in Database
for dirpath, dirnames, filenames in os.walk(inputpath):
    structure = os.path.join(outputpath1, dirpath[len(inputpath):])
    if not filenames:
        continue
    for f1 in filenames:
        if not f1.endswith('.qdimacs'):
            continue
        f = os.path.splitext(f1)[0]
        path = structure + '/' + f
        Path(path).mkdir(parents=True, exist_ok=True) 
        fff = path + '/' + f + suffix
        Path(fff).touch()
        file_dir = dirpath + '/' + f1; 
        hash_object = hashlib.md5(f1.encode()) 
        with open(fff, "w") as out1:
            out1.write (hash_object.hexdigest())
            out1.write (' ')
            out1.write (f1)
            out1.write (' ')
            with open(file_dir, "r") as out2:
                for line in out2:
                    if not line or line[0] == 'c':
                        continue
                    elif line[0] == 'p':
                        word_list = line.split()
                        out1.write (word_list[2])
                        out1.write (' ')
                        out1.write (word_list[3])
                        break
                    else:
                        out1.write ('ERR.')
                       
print ("c Completed creating the Directory structure setup.\nc")

print ("c Starting Experiments...")

# Run the Experiments and collect the data
for dirpath, dirnames, filenames in os.walk(inputpath):
    structure2 = os.path.join(outputpath2, dirpath[len(inputpath):])
    if not filenames:
        continue
    for files in filenames:
        if not files.endswith('.qdimacs'):
            continue
        f = os.path.splitext(files)[0]
        path = structure2 + '/' + f
        Path(path).mkdir(parents=True, exist_ok=True) 
        fff = path + '/' + f + suffix
        Path(fff).touch()
        file_dir = dirpath + '/' + files; 
        cmd = ["timeout", "4000", "./dcnf_autarky", "-i", file_dir, "-e", "2", "-r", "1"] 
        with open(fff, "w") as out:
            subprocess.call(cmd, stdout=out, stderr=subprocess.PIPE)

print ("c Experiments completed.")
