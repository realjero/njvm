cd ..
cd ..
gcc -g -Wall -std=c99 -pedantic -I./include -L./lib njvm.c heap.c instruction.c stack.c support.c -lbigint -o vm
valgrind ./vm --stack 64 --heap 8192 tests/nj/fib.bin
rm vm