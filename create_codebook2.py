#!/usr/bin/python3
import os
import sys
import re
import time

com_rep = ' '

def source_code(char):
    if char == '/':
        return comment_begin, ''
    return source_code, char

def comment_begin(char):
    if char == '/':
        return inline_comment, 2*com_rep
    if char == '*':
        return block_comment, 2*com_rep
    return source_code, '/'+char

def inline_comment(char):
    if char == '\n':
         return source_code, '\n'
    return inline_comment, com_rep

def block_comment(char):
    if char == '*':
        return end_block_comment, com_rep
    if(char == '\n'):
        return block_comment, '\n'
    return block_comment, com_rep

def end_block_comment(char):
    if char == '/':
        return source_code, com_rep
    if(char == '\n'):
        return block_comment, '\n'
    return block_comment, com_rep

def remove_comments(content):
    def gen_content():
        parser = source_code
        for character in content:
            parser, text = parser(character)
            yield text

    return ''.join(gen_content())

def col_regions(content, com_rem):
    p = re.compile("/\*[lr][wypg]\*/");#len 6
    col_stack = ['w']
    exist = {'w':True, 'y':False, 'p':False, 'g':False}
    res = {'w':"", 'y':"", 'p':"", 'g':""}
    while len(content) > 0:
        if p.match(content):
            if content[2] == 'r':
                if content[3] != col_stack[-1]:
                    sys.stderr.write(f'ERROR in coloring\n')
                else:
                    col_stack.pop()
            else:
                col_stack.append(content[3])
                exist[content[3]] = True
            content = content[6:]
            com_rem = com_rem[6:]
        else:
            c = com_rem[0]
            content = content[1:]
            com_rem = com_rem[1:]
            res['g'] += c
            if col_stack[-1] == 'g' and c != '\n':
                c = ' '             
            res['p'] += c      
            if col_stack[-1] == 'p' and c != '\n':
                c = ' '             
            res['y'] += c      
            if col_stack[-1] == 'y' and c != '\n':
                c = ' '
            res['w'] += c
    fin_res = {}
    for elem in exist.items():
        if elem[1]:
            fin_res[elem[0]] = res[elem[0]]
    return fin_res

def rem_col_codes(content):
    p = re.compile("/\*[lr][wypg]\*/");#len 6
    res = ""
    while len(content) > 0:
        if p.match(content):
            content = content[6:]
        else:
            res += content[0]
            content = content[1:]
    return res


def calc_hash(content):
    fout = open("cksum.in", "w")
    fout.write(content)
    fout.close()
    os.system("bash crc.sh cksum.in 999 999 | tail -1 >cksum.out")
    fin = open("cksum.out", "r")
    res = fin.readline().rstrip()
    fin.close()
    return res

def add_hashes(line, act_line, hashes):
    hash_room_left = (47-len(act_line))//5
    line_end = ""
    added = []
    for elem in hashes:
        col, id, hash = elem
        if hash_room_left > 0:
            hash_room_left -= 1
            added.append(hash)
            line_end += "/*l"+col+"*/"
            line_end += id+hash
            line_end += "/*r"+col+"*/"
    if(len(added) > 0):
        line = line.rstrip()
        line += '/*lw*/'+(57-len(act_line)-5*len(added))*' '+'/*rw*/'+line_end
    return (line, added)

def process(content, col_hash = True):
    com_rem = remove_comments('\n'.join(content)).split('\n')
    act_out = rem_col_codes('\n'.join(content)).split('\n')
    col_versions = {}
    for elem in col_regions('\n'.join(content), '\n'.join(com_rem)).items():
        col_versions[elem[0]] = elem[1].split('\n')
    keys = []
    for key in "wypg":
        if key in col_versions:
            keys.append(key)
    if not col_hash:
        keys = keys[-1:]
    since_hash = [0 for key in keys]
    res = ""
    for i in range(len(content)-1):
        hashes = [calc_hash('\n'.join(col_versions[key][:i+1])) for key in keys]
        for j in range(len(keys)):
            since_hash[j] += len(''.join(col_versions[keys[j]][i].split()))
        write_hash = [False for key in keys]
        chars_between = 100
        for j in range(len(keys)-1, -1, -1):
            if(since_hash[j] >= chars_between):
                write_hash[j] = True
            if(j != 0 and write_hash[j] and hashes[j] == hashes[j-1]):
                write_hash[j] = False
                write_hash[j-1] = True 
        hash_querys = []
        for j in range(len(keys)):
            if(write_hash[j]):
                if col_hash:
                    hash_querys.append((keys[j], ' ', hashes[j]))
                else:
                    hash_querys.append(("w", ' ', hashes[j]))
        content[i], written_hashes = add_hashes(content[i], act_out[i], hash_querys)
        for j in range(len(keys)):
            if hashes[j] in written_hashes:
                since_hash[j] = 0
    hash_querys = []
    for j in range(len(keys)):
        if col_hash:
            hash_querys.append((keys[j], '%', hashes[j]))
        else:
            hash_querys.append(("w", '%', hashes[j]))
    content[-1], written_hashes = add_hashes(content[-1], act_out[-1], hash_querys)
    return content


fin = open("file_ord.txt", "r")
file_ord = fin.readlines()
fin.close()
with open('header.html', 'r') as file:
    full_html = file.read()
full_tex = ""

for file in file_ord:
    print(file)
    file = file.rstrip()
    
    lang = "tex"
    if(file[-4:] == ".cpp" or file[-4:] == ".hpp" or file[-2:] == ".c" or file[-2:] == ".h" ):
        lang = "c++"
    elif(file[-3:] == ".sh" ):
        lang = "bash"
    else:
        continue
    outputting = False
    hashing = False
    escape_str = "!escape "
    beg_str = "!begin_codebook"
    end_str = "!end_codebook"
    beg_hash_str = "//!start"
    end_hash_str = "//!finish"
    no_col_hash_str = "//!no_col_hash"
    pause_str = "//!pause"
    unpause_str = "//!unpause"
    loc_html_beg_str = '<div class="highlight"><pre>'
    loc_html_end_str = "</pre></div>\n"
    col_hash = True


    processed = ""
    portion = []
    fin = open(file, "r")
    for line in fin.readlines():
        line = line.rstrip()
        escape_idx = line.find(escape_str)
        beg_idx = line.find(beg_str)
        end_idx = line.find(end_str)
        hash_beg_idx = line.find(beg_hash_str)
        hash_end_idx = line.find(end_hash_str)
        no_col_hash_idx = line.find(no_col_hash_str)
        pause_idx = line.find(pause_str)
        unpause_idx = line.find(unpause_str)
        if(escape_idx != -1):
            line = line[escape_idx+len(escape_str):]
            full_html += '<header>\n<b>'
            full_html += line
            full_html += '</b>\n</header>\n'
        elif(beg_idx != -1):
            outputting = True
        elif(end_idx != -1):
            outputting = False
            if len(portion) > 0:
                processed += '\n'.join(portion)+'\n'
            portion = []
        elif(hash_beg_idx != -1):
            hashing = True
        elif(hash_end_idx != -1):
            hashing = False
            if len(portion) > 0:
                processed += '\n'.join(process(portion, col_hash))+'\n'
            portion = []
        elif(no_col_hash_idx != -1):
            col_hash = False
        elif(pause_idx != -1):
            outputting = False
        elif(unpause_idx != -1):
            outputting = True;
        elif(outputting and not line.isspace() and len(line) > 0):
            portion.append(line)
    fout = open("tmp_source."+lang, "w");
    fout.write(processed)
    fout.close()
    os.system("pygmentize -O full,style=tartu_icpc -o tmp_source.html tmp_source."+lang)
    with open('tmp_source.html', 'r') as file:
        loc_html = file.read()
    loc_html_beg = loc_html.find(loc_html_beg_str)+len(loc_html_beg_str)
    loc_html_end = loc_html.find(loc_html_end_str)
    full_html += loc_html[loc_html_beg:loc_html_end]


with open('footer.html', 'r') as file:
    full_html += file.read()
fout = open("codebookpart2.html", "w")
fout.write(full_html)
fout.close()
