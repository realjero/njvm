
#ifndef HEAP_H
#define HEAP_H

#include "njvm.h"
#include "makro.h"
#include "support.h"

void *alloc (int size);
void copyRootObjects();
void garbage_collect();

#endif //HEAP_H
