#!/usr/bin/python3
import os
import sys
import re
import time


for i in range(2):
    os.system("""/bin/bash -c "printf '\n%.0s' {1..1000}  | pdflatex --shell-escape codebook.tex >/dev/null" """) #Run latex twice to fix links
