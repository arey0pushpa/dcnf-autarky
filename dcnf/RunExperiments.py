#!/usr/bin/python3

import sys
import glob
import os
import re
from pathlib import Path

if len(sys.argv) != 2:
    print("Incorrect number of arguments. use -h for more help.")
    sys.exit(0)

if sys.argv[1] == "-h" or sys.argv[1] == "--help":
    print("./RunExperiments [-h] [cnf_instances_directory]")
    sys.exit(0)

path = sys.argv[1]

# Create three directories 
Path("./Autarkies").mkdir(parents=True, exist_ok=True)
Path("./Database").mkdir(parents=True, exist_ok=True)
Path("./Experiments").mkdir(parents=True, exist_ok=True)

if os.path.isfile(path):
    print(file)
else:
    for x in os.walk(path):
        for file in glob.glob(os.path.join(x[0], "*.dqdimacs")):
            print(file)
