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
    bigObjRef->size = dataSize;
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

    cmpObj->size = size | MSB;
    return cmpObj;
}