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
    ObjRef cmpObj = malloc(sizeof(unsigned int) + (size * sizeof(ObjRef)));
    cmpObj->size = size | MSB;
    return cmpObj;
}