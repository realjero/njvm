#include "heap.h"
#include "bigint.h"
#include "gc.h"

void *alloc(int size) {
    void *obj;
    if ((njvm.heap.next + size) < njvm.heap.end) {
        obj = njvm.heap.next;
        njvm.heap.next += size;
    } else {
        garbage_collect();
        if ((njvm.heap.next + size) < njvm.heap.end) {
            obj = njvm.heap.next;
            njvm.heap.next += size;
        } else {
            fatalError("Heap Overflow");
        }
    }
    return obj;
}

void print_heap() {
    char *scan = njvm.heap.active;
    printf("njvm.heap.active: %p\n", njvm.heap.active);
    while (scan < njvm.heap.next) {
        ObjRef obj = (ObjRef) scan;
        if (obj != NULL) {
            if (IS_PRIMITIVE(obj)) {
                printf("(ObjRef) Address: %p, size: %d\n", (void *) obj, obj->size);
                scan += sizeof(void *) +
                        sizeof(unsigned int) +
                        sizeof(bool) +
                        (obj->size * sizeof(unsigned char));
            } else {
                printf("(CmpObj) Address: %p, size: %d ", (void *) obj, GET_ELEMENT_COUNT(obj));
                for (int i = 0; i < GET_ELEMENT_COUNT(obj); i++) {
                    if (GET_REFS_PTR(obj)[i] == NULL) {
                        printf("GET_REFS_PTR(obj)[i]: NULL\t");
                    } else {
                        printf("GET_REFS_PTR(obj)[i]: %p\t", (void *) GET_REFS_PTR(obj)[i]);
                    }

                }
                printf("\n");
                scan += sizeof(void *) +
                        sizeof(unsigned int) +
                        sizeof(bool) +
                        (GET_ELEMENT_COUNT(obj) * sizeof(void *));
            }
        } else {
            scan += sizeof(unsigned char);
        }
    }
}