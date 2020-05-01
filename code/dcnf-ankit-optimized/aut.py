#!/usr/bin/python3

import sys
import glob
import os
import re
import shutil
import subprocess
import hashlib
import asyncio
import sys

from pathlib import Path
from multiprocessing import Process

if len(sys.argv) != 2:
    print("Incorrect number of arguments. use -h for more help.")
    sys.exit(0)

if sys.argv[1] == "-h" or sys.argv[1] == "--help":
    print("./RunExperiments [-h] [cnf_instances_directory]")
    sys.exit(0)

inputpath = sys.argv[1]
outputpath = './Exper-E1A1-Pan/'
suffix = '.qdimacs'

MAX_PROCESSES = 4

async def process_csv(files, structure, dirpath, sem):
    async with sem:  # controls/allows running 10 concurrent subprocesses at a time
        print("The filename is: ", files)
        f = os.path.splitext(files)[0]
        Path(structure).mkdir(parents=True, exist_ok=True)
        fff = structure + '/' + f + suffix
        print("fff is: ", fff)
        if not os.path.isfile(fff): 
            file_dir = dirpath + '/' + files;
            with open(fff, 'w+') as out:
                proc = await asyncio.create_subprocess_exec('./dcnf_autarky', '-i', file_dir, '-r', '3', stdout = out)
                await proc.wait()


async def main():
    sem = asyncio.Semaphore(MAX_PROCESSES)
    for dirpath, dirnames, filenames in os.walk(inputpath):
        structure = os.path.join(outputpath, dirpath[len(inputpath):]) 
        print("The structure is: ", structure)
        await asyncio.gather(*[process_csv(files, structure, dirpath, sem) for files in filenames])

asyncio.run(main())
