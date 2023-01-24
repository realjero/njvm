
#ifndef STACK_H
#define STACK_H

#include "njvm.h"

void push_objref(ObjRef object);

ObjRef pop_objref();

void push_number(int number);

int pop_number();

void stack_print();

void sda_print();

#endif //STACK_H
