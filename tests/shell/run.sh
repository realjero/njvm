cd ..
cd ..
gcc -g -Wall -std=c99 -pedantic \
-I./include -L./lib njvm.c instruction.c stack.c support.c -lbigint -o vm
for t in tests/bin/*.bin; do
  echo "TESTING: $t";
  echo "NJVM:";
  ./vm $t > o1;
  echo "REF:";
  ./tests/njvm $t > o2;
  DIFF=$(diff o1 o2);
  if [ "$DIFF" == "" ]; then
    echo -e "\xE2\x9C\x94 PASSED";
  else
    echo -e "\xE2\x9C\x94 FAILED";
  fi
done;
rm o1;
rm o2;
rm vm