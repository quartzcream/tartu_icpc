# !escape crc.sh

# !begin_codebook
#!/bin/envbash

for j in `seq $2 1 $3`; do #whistespaces don't matter.
  sed '/^\s*$/d' $1 | head -$j | tr -d '[[:space:]]' \
    | cksum | cut -f1 -d ' ' | tail -c 5
done #there shouldn't be any COMMENTS.
#copy lines being checked to separate file.
# $ ./crc.sh tmp.cpp 999 999
# $ ./crc.sh tmp.cpp 1 333 | grep XXXX
# !end_codebook
