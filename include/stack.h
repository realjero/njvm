
#ifndef STACK_H
#define STACK_H

#include "njvm.h"

void stack_push(StackSlot s);
StackSlot stack_pop();
StackSlot stackslot_new_obj(ObjRef object);
StackSlot stackslot_new(unsigned int n);
void stack_print();
void sda_print();

#endif //STACK_H
