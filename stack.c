#include "stack.h"
#include "support.h"

void stack_push(StackSlot slot) {
    if(njvm.stack.stack_pointer == STACK_MAX_ITEMS) {
        fatalError("stack overflow");
    }

    njvm.stack.stack[njvm.stack.stack_pointer] = slot;
    njvm.stack.stack_pointer++;
}

StackSlot stack_pop() {
    if(njvm.stack.stack_pointer == 0) {
        fatalError("stack underflow");
    }
    njvm.stack.stack_pointer--;
    return njvm.stack.stack[njvm.stack.stack_pointer];
}

StackSlot stackslot_new_obj(ObjRef object) {
    StackSlot slot;
    slot.isObjRef = true;
    slot.u.objRef = object;
    return slot;
}

StackSlot stackslot_new(unsigned int number) {
    StackSlot slot;
    slot.isObjRef = false;
    slot.u.number = number;
    return slot;
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