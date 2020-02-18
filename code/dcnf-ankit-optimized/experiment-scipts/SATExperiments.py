#!/usr/bin/python3

import sys
import glob
import os
import re
import shutil
import subprocess
import hashlib
from pathlib import Path
from multiprocessing import Process


if len(sys.argv) != 2:
    print("Incorrect number of arguments. use -h for more help.")
    sys.exit(0)

if sys.argv[1] == "-h" or sys.argv[1] == "--help":
    print("./RunExperiments [-h] [cnf_instances_directory]")
    sys.exit(0)

inputpath = sys.argv[1]

# Create three directories 
#Path("./Results").mkdir(parents=True, exist_ok=True)
Path("./Experiments").mkdir(parents=True, exist_ok=True)

#outputpath1 = './Database/'
outputpath2 = './Experiments/'
suffix = '.R'

print ("c Starting Experiments...")
# Run the Experiments and collect the data
for dirpath, dirnames, filenames in os.walk(inputpath):
    structure2 = os.path.join(outputpath2, dirpath[len(inputpath):])
    for files in filenames:
        f = os.path.splitext(files)[0]
        path = structure2 + '/' + f
        Path(path).mkdir(parents=True, exist_ok=True) 
        fff = path + '/' + f + suffix
        Path(fff).touch()
        file_dir = dirpath + '/' + files; 
        cmd = ["./dcnf_autarky", "-i", file_dir, "-r", "3"] 
        with open(fff, "w") as out:
            subprocess.Popen(cmd, stdout=out, stderr=subprocess.PIPE)

print ("c Experiments completed.")
