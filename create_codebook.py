#!/bin/env python
import os

fin = open("file_ord.txt", "r")
file_ord = fin.readlines()
fin.close()

fout = open("codebook.tex", "w");
for file in file_ord:
    file = file.rstrip()
    need_minted = False;
    if(file[-4:] == ".cpp"):
        need_minted = True;
    fin = open(file, "r")
    outputting = False
    for line in fin.readlines():
        escape_str = "!escape "
        beg_str = "!begin_codebook\n"
        end_str = "!end_codebook\n"
        escape_idx = line.find(escape_str)
        if(escape_idx != -1):
            line = line[escape_idx+len(escape_str):]
            fout.write(line)
        elif(line[-len(beg_str):] == beg_str):
            outputting = True
        elif(line[-len(end_str):] == end_str):
            outputting = False
        elif(outputting):
            if(need_minted):
                fout.write("\\begin{minted}[tabsize=2,baselinestretch=1,linenos,numbersep = 1mm, breaklines, frame=lines]{c++}\n")
                need_minted = False
            fout.write(line)
    if(file[-4:] == ".cpp" and not need_minted):
        fout.write("\\end{minted}\n")
fout.close()

os.system("pdflatex --shell-escape codebook.tex") #Run latex twice to fix links
os.system("pdflatex --shell-escape codebook.tex")
