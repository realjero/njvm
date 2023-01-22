#include "heap.h"

void *alloc(unsigned int size) {
    void *obj;
    if ((njvm.heap.next + size) <= njvm.heap.end) {
        obj = njvm.heap.next;
        njvm.heap.next += size;
    } else {
        printf("GarbageCollectneeded\n");
        // Garbage Collect
    }
    return obj;
}