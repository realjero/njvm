/*
 * support.h -- object representation and support functions
 */


#ifndef _SUPPORT_H_
#define _SUPPORT_H_

#include "njvm.h"

/* support functions */

void fatalError(char *msg);        /* print a message and exit */
void *newPrimObject(int dataSize);    /* create a new primitive object */
void *getPrimObjectDataPointer(void *primObject);

ObjRef newCompositeObject(int size);

#endif /* _SUPPORT_H_ */
