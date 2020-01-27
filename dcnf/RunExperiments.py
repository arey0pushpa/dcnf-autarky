#!/usr/bin/python3

import sys
import glob
import os
import re
import shutil
import subprocess
from pathlib import Path

if len(sys.argv) != 2:
    print("Incorrect number of arguments. use -h for more help.")
    sys.exit(0)

if sys.argv[1] == "-h" or sys.argv[1] == "--help":
    print("./RunExperiments [-h] [cnf_instances_directory]")
    sys.exit(0)

inputpath = sys.argv[1]

# Create three directories 
Path("./Autarkies").mkdir(parents=True, exist_ok=True)
Path("./Database").mkdir(parents=True, exist_ok=True)
Path("./Experiments").mkdir(parents=True, exist_ok=True)

outputpath1 = './Database/'
outputpath2 = './Experiments/'
suffix = '.R'

print ("c Starting the directory structure setup.")

# Create the directory structure similar to the input file in Database
for dirpath, dirnames, filenames in os.walk(inputpath):
    structure = os.path.join(outputpath1, dirpath[len(inputpath):])
    if not filenames:
        continue
    for f1 in filenames:
        f = os.path.splitext(f1)[0]
        path = structure + '/' + f
        Path(path).mkdir(parents=True, exist_ok=True) 
        fff = path + '/' + f + suffix
        Path(fff).touch()
        #with open(f1, "w") as out:

print ("c Completed creating the Directory structure setup.\nc")

print ("c Starting Experiments...")

# Run the Experiments and collect the data
for dirpath, dirnames, filenames in os.walk(inputpath):
    structure2 = os.path.join(outputpath2, dirpath[len(inputpath):])
    if not filenames:
        continue
    for files in filenames:
        f = os.path.splitext(files)[0]
        path = structure2 + '/' + f
        Path(path).mkdir(parents=True, exist_ok=True) 
        fff = path + '/' + f + suffix
        Path(fff).touch()
        file_dir = dirpath + '/' + files; 
        cmd = ["./dcnf_autarky", "-i", file_dir, "-e", "2", "-r", "3"] 
        with open(fff, "w+") as out:
            subprocess.call(cmd, stdout=out, stderr=subprocess.PIPE)

print ("c Experiments completed.")
