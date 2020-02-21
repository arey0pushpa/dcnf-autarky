import os
import shutil
#!/usr/bin/python3

import sys
import glob
import re

if len(sys.argv) != 3:
    print("Incorrect number of arguments. use -h for more help.")
    sys.exit(0)

if sys.argv[1] == "-h" or sys.argv[1] == "--help":
    print("./run [data_file] [dest_directory]")
    sys.exit(0)

data_file = sys.argv[1]
dest = sys.argv[2]

with open (data_file, "r") as fileHandler:
    # Read each line in loop
    for line in fileHandler:
        src = line.strip() 
        print("The source is:", src)
        head, tail = os.path.split(src)
        print("The file is:", tail)
        shutil.copy2(src, dest+ "/" + tail)
        print("The final dest is:", dest+ "/" + tail)
