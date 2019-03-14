#!/usr/bin/python3
import os
import shlex
import subprocess

def add_hash(line, crc_line_start, crc_line_num, comment, delim = '#'):
    os.system("sed \'/^\\s*$/d\' tmp | head -" + str(crc_line_num) + " | tail -" +str(crc_line_num - crc_line_start) + " | tr -d \'[[:space:]]\' | cksum | cut -f1 -d ' ' | tail -c 5 >tmp2")
    #sed gives errors but seams to work
    fin2 = open("tmp2", "r")
    line = line.rstrip()
    if(not comment):
        line += "@ "
    line += "$ "
    line += "\\hfill "
    line += delim
    line += fin2.readline()
    line = line.rstrip()
    line += " $"
    if(not comment):
        line += " @"
    line += "\n"
    fin2.close()
    return line

fin = open("file_ord.txt", "r")
file_ord = fin.readlines()
fin.close()

fout = open("codebook.tex", "w");
for file in file_ord:
    file = file.rstrip()
    use_minted = False
    need_minted = False
    
    lang = "tex"
    if(file[-4:] == ".cpp" or file[-4:] == ".hpp" or file[-2:] == ".c" or file[-2:] == ".h" or file[-3:] == ".sh" ):
        lang = "c++"
        if(file[-3:] == ".sh" ):
            lang = "bash"
        need_minted = True
        use_minted = True
    outputting = False
    hashing = False
    multiline_comment = False
    comment = False
    define = False
    last_needs_end = False
    need_write_hash = False
    escape_str = "!escape "
    beg_str = "!begin_codebook"
    end_str = "!end_codebook"
    beg_hash_str = "//!start"
    end_hash_str = "//!finish"
    pause_hash_str = "//!pause"
    unpause_hash_str = "//!unpause"
    if(lang == "c++"):
        os.system("clang-format -style=file "+file+" >tmp_source")
    else:
        os.system("cp "+file+" tmp_source");


    if(use_minted):
        fin = open("tmp_source", "r")
        ftmp = open("tmp", "w")
        for line in fin.readlines():
            escape_idx = line.find(escape_str)
            beg_idx = line.find(beg_str)
            end_idx = line.find(end_str)
            hash_beg_idx = line.find(beg_hash_str)
            hash_end_idx = line.find(end_hash_str)
            hash_pause_idx = line.find(pause_hash_str)
            hash_unpause_idx = line.find(unpause_hash_str)
            if(escape_idx != -1):
                line = line[escape_idx+len(escape_str):]
            elif(beg_idx != -1):
                outputting = True
            elif(end_idx != -1):
                outputting = False
            elif(hash_beg_idx != -1):
                crc_line_start = crc_line_num
                hashing = True
            elif(hash_end_idx != -1):
                hashing = False
            elif(hash_pause_idx != -1):
                hashing = False
            elif(hash_unpause_idx != -1):
                hashing = True;
            else:
                if(hashing):
                    ftmp.write(line)
        ftmp.close()
        os.system("./remccoms.py <tmp | sponge tmp") #requires moreutils package
    fin = open("tmp_source", "r")
    outputting = False
    if(use_minted):
        ftmp = open("tmp", "r")
        crc_line_num = 0
        crc_since_last = 0
        it = iter(ftmp.readlines())
    for line in fin.readlines():
        escape_idx = line.find(escape_str)
        beg_idx = line.find(beg_str)
        end_idx = line.find(end_str)
        hash_beg_idx = line.find(beg_hash_str)
        hash_end_idx = line.find(end_hash_str)
        hash_pause_idx = line.find(pause_hash_str)
        hash_unpause_idx = line.find(unpause_hash_str)
        if(escape_idx != -1):
            line = line[escape_idx+len(escape_str):]
            fout.write(line)
        elif(beg_idx != -1):
            outputting = True
        elif(end_idx != -1):
            outputting = False
        elif(hash_beg_idx != -1):
            crc_line_start = crc_line_num
            hashing = True
            crc_since_last = 0
        elif(hash_end_idx != -1):
            hash_str = add_hash("", crc_line_start, crc_line_num, comment, '%');
            hash_str = hash_str.rstrip()
            fout.write(hash_str)
            need_write_hash = False
            hashing = False
        elif(hash_pause_idx != -1):
            hashing = False
        elif(hash_unpause_idx != -1):
            hashing = True;
        else:
            if(last_needs_end):
                fout.write("\n")
                last_needs_end = False
            if(outputting and not line.isspace()):
                idxd = line.rfind("#")
                define = idxd != -1
                idx1 = line.rfind("/*") #not perfect
                idx2 = line.rfind("*/")
                idx3 = line.rfind("//")
                if(not multiline_comment and idx1 != -1):
                    multiline_comment = True
                if(multiline_comment and idx2 != -1):
                    multiline_comment = False
                comment = multiline_comment
                if(idx3 != -1 and not multiline_comment and (idx2 == -1 or idx3>idx2)):
                    comment = True
                if(need_minted):
                    if(lang == "c++"):
                        fout.write("\\begin{minted}[tabsize=2,baselinestretch=1,linenos,numbersep = 1mm, breaklines, frame=lines, escapeinside=@@, texcomments=true, mathescape=true]{"+lang+"}\n")
                    else:
                        fout.write("\\begin{minted}[tabsize=2,baselinestretch=1,linenos,numbersep = 1mm, breaklines, frame=lines, texcomments=true, mathescape=true]{"+lang+"}\n")
                    need_minted = False
                write_line = line
                min_idx = 1000
                if(idxd != -1):
                    min_idx = min(min_idx, idxd)
                if(idx3 != -1):
                    min_idx = min(min_idx, idx3)
                if(multiline_comment and idx1 != -1 and idx2 == -1):
                    min_idx = min(min_idx, idx1)
                if(multiline_comment and idx1 == -1 and idx2 == -1):
                    min_idx = 0
                if(min_idx != 1000):
                    write1 = write_line[:min_idx]
                    write2 = write_line[min_idx:]
                    write2 = write2.replace("_", "\_")
                    write_line = write1+write2
                write_line = write_line.rstrip()
                line = line.rstrip()
                fout.write(write_line)
                last_needs_end = True
            if(hashing):
                strip_line = it.__next__()
                if(not strip_line.isspace()):
                    crc_line_num += 1
                    crc_since_last += 1
                    if(crc_since_last == 5):
                        need_write_hash = True
            if(need_write_hash and last_needs_end and len(line) < 50 and not comment and not define):
                hash_str = add_hash("", crc_line_start, crc_line_num, comment, '#');
                hash_str = hash_str.rstrip()
                fout.write(hash_str)
                need_write_hash = False
                crc_since_last = 0
    if(last_needs_end):
        fout.write("\n")
    if(use_minted and not need_minted):
        fout.write("\\end{minted}\n")
    if(use_minted):
        ftmp.close()
fout.close()

for i in range(2):
    os.system("""/bin/bash -c "printf '\n%.0s' {1..1000}  | pdflatex --shell-escape codebook.tex >/dev/null" """) #Run latex twice to fix links
