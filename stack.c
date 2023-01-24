#include "stack.h"
#include "support.h"

void push_objref(ObjRef object) {
    if (njvm.stack.stack_pointer >= njvm.stack.size / sizeof(StackSlot)) {
        fatalError("stack overflow");
    }
    njvm.stack.stack[njvm.stack.stack_pointer].isObjRef = true;
    njvm.stack.stack[njvm.stack.stack_pointer].u.objRef = object;
    njvm.stack.stack_pointer++;
}

ObjRef pop_objref() {
    if (njvm.stack.stack_pointer == 0) {
        fatalError("stack underflow");
    }
    njvm.stack.stack_pointer--;
    if (!njvm.stack.stack[njvm.stack.stack_pointer].isObjRef) {
        fatalError("ObjRef pop_objref() can't pop number");
    }
    return njvm.stack.stack[njvm.stack.stack_pointer].u.objRef;
}

void push_number(int number) {
    if (njvm.stack.stack_pointer >= njvm.stack.size / sizeof(StackSlot)) {
        fatalError("stack overflow");
    }
    njvm.stack.stack[njvm.stack.stack_pointer].isObjRef = false;
    njvm.stack.stack[njvm.stack.stack_pointer].u.number = number;
    njvm.stack.stack_pointer++;
}

int pop_number() {
    if (njvm.stack.stack_pointer == 0) {
        fatalError("stack underflow");
    }
    njvm.stack.stack_pointer--;
    if (njvm.stack.stack[njvm.stack.stack_pointer].isObjRef) {
        fatalError("ObjRef pop_number can't pop object");
    }
    return njvm.stack.stack[njvm.stack.stack_pointer].u.number;
}

void stack_print() {
    for (int i = njvm.stack.stack_pointer; i >= 0; i--) {
        if (i == njvm.stack.stack_pointer) {
            printf("sp\t\t--->\t%04d:\txxxx\n", i);
        } else if (i == njvm.stack.frame_pointer) {
            if (njvm.stack.stack[i].isObjRef == true) {
                printf("fp\t\t--->\t%04d:\t%p (objref)\n", i, (void *) njvm.stack.stack[i].u.objRef);
            } else {
                printf("fp\t\t--->\t%04d:\t%d (number)\n", i, njvm.stack.stack[i].u.number);
            }
        } else {
            if (njvm.stack.stack[i].isObjRef == true) {
                printf("\t\t\t\t%04d:\t%p (objref)\n", i, (void *) njvm.stack.stack[i].u.objRef);
            } else {
                printf("\t\t\t\t%04d:\t%d (number)\n", i, njvm.stack.stack[i].u.number);
            }
        }
    }
    printf("\t\t\t\t\t\t--- bottom of stack ---\n");
}

void sda_print() {
    for (int i = 0; i < njvm.sda.size; i++) {
        printf("data[%04d]:\t\t%p\n", i, (void *) njvm.stack.stack[i].u.objRef);
    }
    printf("\t\t\t\t--- end of data ---\n");
}