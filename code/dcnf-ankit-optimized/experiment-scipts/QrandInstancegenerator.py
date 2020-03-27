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

print ("c Starting Experiments...")

cnt = 40
while cnt < 500:
        cls = str(cnt) + "*1,1|2-2,2"
        quant = "a50 50"
        cmd = ["./QBRG_debug", quant, cls, "", "1,t", ""] 
        subprocess.Popen(cmd)
        cnt += 1
print ("c Experiments completed.")
