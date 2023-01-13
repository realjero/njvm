cd ..
mkdir "bin"
for t in asm/*.asm; do
  file="${t%.*}"
  fileoexten="${file##*/}"
  echo bin/$fileoexten.bin
  ./nja $t bin/$fileoexten.bin
done;
