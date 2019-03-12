# !escape \section{crc.sh}

# !begin_codebook
#!/bin/envbash
for j in `seq 1 1 200`; do
  sed '/^\s*$/d' $1 | head -$j | tr -d '[[:space:]]' | cksum | cut -f1 -d ' ' | tail -c 5 #whistespaces don't matter.
done #there shouldn't be any COMMENTS.
#copy lines being checked to separate file.
# \$ ./crc.sh tmp.cpp | grep XXXX
# !end_codebook
