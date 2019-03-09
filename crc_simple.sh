# !escape \section{crc.sh}

# !begin_codebook
#!/bin/envbash
for j in `seq 5 5 200`; do
  sed '/^\s*$/d' $1 | head -$j | tr -d '[[:space:]]' | cksum | cut -f1 -d ' ' | tail -c 4 #whistespaces don't matter.
done #there shouldn't be any COMMENTS.
#copy lines being checked to separate file.
# \$ ./crc.sh tmp.cpp
# !end_codebook
