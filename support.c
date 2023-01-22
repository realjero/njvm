#include "support.h"
#include "makro.h"

void fatalError(char *msg) {
    printf("Fatal error: %s\n", msg);
    exit(1);
}

void *newPrimObject(int dataSize) {
    ObjRef bigObjRef;

    bigObjRef = malloc(sizeof(unsigned int) +
                       dataSize * sizeof(unsigned char));
    if (bigObjRef == NULL) {
        fatalError("newPrimObject() got no memory");
    }
    bigObjRef->size = dataSize;
    return bigObjRef;
}

void *getPrimObjectDataPointer(void *obj) {
    ObjRef oo = ((ObjRef) (obj));
    return oo->data;
}

ObjRef newCompositeObject(unsigned int size) {
    ObjRef cmpObj = malloc(sizeof(ObjRef) + (size * sizeof(void *)));
    cmpObj->size = size | MSB;
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