#include "gc.h"
#include "njvm.h"
#include "support.h"
#include "makro.h"
#include <bigint.h>

void swap_heap() {
    njvm.heap.next = njvm.heap.passive;
    njvm.heap.passive = njvm.heap.active;
    njvm.heap.active = njvm.heap.next;
    njvm.heap.end = njvm.heap.active + (njvm.heap.size / 2);
    memset(njvm.heap.active, 0, njvm.heap.size / 2);
}

ObjRef copyObjectToFreeMem(ObjRef src) {
    ObjRef dest;
    if (IS_PRIMITIVE(src)) {
        dest = newPrimObject(src->size);
        memcpy(dest->data, src->data, src->size);
    } else {
        dest = newCompositeObject(GET_ELEMENT_COUNT(src));
        memcpy(dest->data, src->data, (GET_ELEMENT_COUNT(src) * sizeof(void *)));
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
        if (IS_PRIMITIVE(object)) {
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
    copyRootObjects();
    scan();
    if (njvm.heap.purge) {
        memset(njvm.heap.passive, 0, njvm.heap.size / 2);
    }
}