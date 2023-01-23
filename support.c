#include "support.h"
#include "makro.h"
#include "heap.h"

void fatalError(char *msg) {
    printf("Fatal error: %s\n", msg);
    exit(1);
}

void *newPrimObject(int dataSize) {
    ObjRef bigObjRef;

    bigObjRef = alloc(sizeof (void*) +
            sizeof(unsigned int) +
            sizeof (bool) +
            dataSize * sizeof(unsigned char));

    if (bigObjRef == NULL) {
        fatalError("newPrimObject() got no memory");
    }
    bigObjRef->forward_pointer = NULL;
    bigObjRef->size = dataSize;
    bigObjRef->broken_heart = false;
    return bigObjRef;
}

void *getPrimObjectDataPointer(void *obj) {
    ObjRef oo = ((ObjRef) (obj));
    return oo->data;
}

ObjRef newCompositeObject(int size) {
    ObjRef cmpObj = alloc(sizeof(void*) +
                          sizeof(unsigned int) +
                          sizeof (bool) +
                          (size * sizeof(void *)));

    if(cmpObj == NULL) {
        fatalError("newCompositeObject() got no memory");
    }

    cmpObj->forward_pointer= NULL;
    cmpObj->size = size | MSB;
    cmpObj->broken_heart = false;
    return cmpObj;
}

ObjRef stack_pop_objref() {
    if(njvm.stack.stack_pointer == 0) {
        fatalError("stack underflow");
    }
    njvm.stack.stack_pointer--;
    StackSlot stackSlot = njvm.stack.stack[njvm.stack.stack_pointer];

    if(!stackSlot.isObjRef) {
        fatalError("popObjRef detected number on top of stack");
    }

    ObjRef object = stackSlot.u.objRef;
    return object;
}

int objref_data_size(ObjRef o) {
    return IS_PRIMITIVE(o) ? o->size : GET_ELEMENT_COUNT(o);
}