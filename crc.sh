# !escape \section{crc.sh}

# !begin_codebook
#!/bin/envbash
starts=($(sed '/^\s*$/d' $1 | grep -n "//\!start" | cut -f1 -d:))
finishes=($(sed '/^\s*$/d' $1 | grep -n "//\!finish" | cut -f1 -d:))
for ((i=0;i<${#starts[@]};i++)); do
  for j in `seq 10 10 $((finishes[$i]-starts[$i]+8))`; do
    sed '/^\s*$/d' $1 | head -$((finishes[$i]-1)) | tail -$((finishes[$i]-starts[$i]-1)) | \
      head -$j | tr -d '[[:space:]]' | cksum | cut -f1 -d ' ' | tail -c 4
  done #whistespaces don't matter
  echo #there shouldn't be any comments in the checked range
done #//check last number in each block
# !end_codebook
