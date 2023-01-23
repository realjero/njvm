#include "heap.h"

void *alloc(unsigned int size) {
    void *obj;
    if ((njvm.heap.next + size) <= njvm.heap.end) {
        obj = njvm.heap.next;
        njvm.heap.next += size;
    } else {
        if(njvm.heap.gcrunning) {
            perror("Garbage Collection is running already");
        }
        printf("GarbageCollectneeded\n");
        // Garbage Collect
    }
    return obj;
}

void copyRootObjects() {
    // der statische Datenbereich, das Return-Value-Register, diejenigen Stack-Slots
    for(int i = 0; i < njvm.sda.size; i++) {

        njvm.sda.sda[i] = memcpy(alloc(sizeof(unsigned)), njvm.sda.sda[i], si)
    }
}