#include "heap.h"
#include "bigint.h"

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

void swap_heap() {
    njvm.heap.next = njvm.heap.passive;
    njvm.heap.passive = njvm.heap.active;
    njvm.heap.active = njvm.heap.next;
    njvm.heap.end = njvm.heap.active + (njvm.heap.size / 2);
}

ObjRef copyObjectToFreeMem(ObjRef src) {
    ObjRef dest;
    if (IS_PRIMITIVE(src)) {
        dest = newPrimObject(src->size);
        memcpy(dest->data, src->data, src->size);
    } else {
        dest = newCompositeObject(GET_ELEMENT_COUNT(src));
        memcpy(dest->data, src->data, (GET_ELEMENT_COUNT(src) * sizeof(void*)));
    }
    return dest;
}

ObjRef relocate(ObjRef o) {
    ObjRef dest;
    if (o == NULL) {
        dest = NULL;
    } else {
        if (o->broken_heart) {
            dest = o->forward_pointer;
        } else {
            dest = copyObjectToFreeMem(o);
            o->broken_heart = true;
            o->forward_pointer = dest;
        }
    }
    return dest;
}

void scan(void) {
    char *scan = njvm.heap.active;
    while (scan < njvm.heap.next) {
        ObjRef object = (ObjRef) scan;
        if (!IS_PRIMITIVE(object)) {
            ObjRef *refs = GET_REFS_PTR(object);
            for (int i = 0; i < GET_ELEMENT_COUNT(object); i++) {
                refs[i] = relocate(refs[i]);
            }
        }
        if(IS_PRIMITIVE(object)) {
            scan += sizeof(void *) +
                    sizeof(unsigned int) +
                    sizeof(bool) +
                    (object->size * sizeof(unsigned char));
        } else {
            scan += sizeof(void *) +
                    sizeof(unsigned int) +
                    sizeof(bool) +
                    (GET_ELEMENT_COUNT(object) * sizeof(void *));
        }
    }
}

void copyRootObjects() {
    for (int i = 0; i < njvm.stack.stack_pointer; i++) {
        if (njvm.stack.stack[i].isObjRef) {
            njvm.stack.stack[i].u.objRef = relocate(njvm.stack.stack[i].u.objRef);
        }
    }
    for (int i = 0; i < njvm.sda.size; i++) {
        njvm.sda.sda[i] = relocate(njvm.sda.sda[i]);
    }
    njvm.rvr = relocate(njvm.rvr);
    bip.op1 = relocate(bip.op1);
    bip.op2 = relocate(bip.op2);
    bip.res = relocate(bip.res);
    bip.rem = relocate(bip.rem);
}

void garbage_collect() {
    swap_heap();
    memset(njvm.heap.active, 0, njvm.heap.size / 2);
    copyRootObjects();
    scan();
    if (njvm.heap.purge) {
        memset(njvm.heap.passive, 0, njvm.heap.size / 2);
    }
}

void print_heap() {
    char *scan = njvm.heap.active;
    printf("njvm.heap.active: %p\n", njvm.heap.active);
    while (scan < njvm.heap.next) {
        ObjRef obj = (ObjRef) scan;
        if(obj != NULL) {
            if (IS_PRIMITIVE(obj)) {
                printf("(ObjRef) Address: %p, size: %d\n", (void *)obj, obj->size);
                scan += sizeof(void *) +
                        sizeof(unsigned int) +
                        sizeof(bool) +
                        (obj->size * sizeof(unsigned char));
            } else {
                printf("(CmpObj) Address: %p, size: %d ", (void *)obj, GET_ELEMENT_COUNT(obj));
                for (int i = 0; i < GET_ELEMENT_COUNT(obj); i++) {
                    if(GET_REFS_PTR(obj)[i] == NULL) {
                        printf("GET_REFS_PTR(obj)[i]: NULL\t");
                    } else {
                        printf("GET_REFS_PTR(obj)[i]: %p\t", (void *)GET_REFS_PTR(obj)[i]);
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