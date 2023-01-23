#include "instruction.h"
#include "stack.h"
#include "makro.h"
#include <bigint.h>
#include "heap.h"

void instruction_HALT() {
    free_heap();
    free_program_memory();
    free_sda();
    free_stack();
}

void instruction_PUSHC(int immediate) {
    bigFromInt(SIGN_EXTEND(immediate));
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_ADD() {
    bip.op2 = stack_pop_objref();
    bip.op1 = stack_pop_objref();
    bigAdd();
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_SUB() {
    bip.op2 = stack_pop_objref();
    bip.op1 = stack_pop_objref();
    bigSub();
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_MUL() {
    bip.op2 = stack_pop_objref();
    bip.op1 = stack_pop_objref();
    bigMul();
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_DIV() {
    bip.op2 = stack_pop_objref();
    bip.op1 = stack_pop_objref();
    bigDiv();
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_MOD() {
    bip.op2 = stack_pop_objref();
    bip.op1 = stack_pop_objref();
    bigDiv();
    stack_push(stackslot_new_obj(bip.rem));
}

void instruction_RDINT() {
    bigRead(stdin);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_WRINT() {
    bip.op1 = stack_pop_objref();
    bigPrint(stdout);
}

void instruction_RDCHR() {
    char input;
    if(!scanf("%c", &input)) {
        fatalError("failed to read character");
    }
    bigFromInt((int) input);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_WRCHR() {
    bip.op1 = stack_pop_objref();
    printf("%c", (char) bigToInt());
}

void instruction_PUSHG(int immediate) {
    if(immediate < 0 || njvm.sda.size <= immediate) {
        fatalError("static data area index out of bound");
    }
    stack_push(stackslot_new_obj(
            njvm.sda.sda[immediate]));
}

void instruction_POPG(int immediate) {
    if(immediate < 0 || njvm.sda.size <= immediate) {
        fatalError("static data area index out of bound");
    }
    njvm.sda.sda[immediate] = stack_pop_objref();
}

void instruction_ASF(int immediate) {
    if((njvm.stack.stack_pointer + immediate) >= njvm.stack.size) {
        fatalError("stack overflow");
    }
    if(immediate < 0) {
        fatalError("stack frame cannot be negativ");
    }
    stack_push(stackslot_new(njvm.stack.frame_pointer));
    njvm.stack.frame_pointer = njvm.stack.stack_pointer;
    njvm.stack.stack_pointer += immediate;
}

void instruction_RSF() {
    njvm.stack.stack_pointer = njvm.stack.frame_pointer;
    njvm.stack.frame_pointer = stack_pop().u.number;
}

void instruction_PUSHL(int immediate) {
    stack_push(njvm.stack.stack[njvm.stack.frame_pointer + SIGN_EXTEND(immediate)]);
}

void instruction_POPL(int immediate) {
    njvm.stack.stack[njvm.stack.frame_pointer + SIGN_EXTEND(immediate)] = stack_pop();
}

void instruction_EQ() {
    bip.op2 = stack_pop_objref();
    bip.op1 = stack_pop_objref();
    bigFromInt(bigCmp() == 0 ? 1 : 0);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_NE() {
    bip.op2 = stack_pop_objref();
    bip.op1 = stack_pop_objref();
    bigFromInt(bigCmp() != 0 ? 1 : 0);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_LT() {
    bip.op2 = stack_pop_objref();
    bip.op1 = stack_pop_objref();
    bigFromInt(bigCmp() < 0 ? 1 : 0);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_LE() {
    bip.op2 = stack_pop_objref();
    bip.op1 = stack_pop_objref();
    bigFromInt(bigCmp() <= 0 ? 1 : 0);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_GT() {
    bip.op2 = stack_pop_objref();
    bip.op1 = stack_pop_objref();
    bigFromInt(bigCmp() > 0 ? 1 : 0);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_GE() {
    bip.op2 = stack_pop_objref();
    bip.op1 = stack_pop_objref();
    bigFromInt(bigCmp() >= 0 ? 1 : 0);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_JMP(int immediate) {
    njvm.program_memory.program_counter = immediate;
}

void instruction_BRF(int immediate) {
    bip.op1 = stack_pop_objref();
    if (!bigToInt()) njvm.program_memory.program_counter = immediate;
}

void instruction_BRT(int immediate) {
    bip.op1 = stack_pop_objref();
    if (bigToInt() == 1) njvm.program_memory.program_counter = immediate;
}

void instruction_CALL(int immediate) {
    stack_push(stackslot_new(njvm.program_memory.program_counter));
    njvm.program_memory.program_counter = immediate;
}

void instruction_RET() {
    njvm.program_memory.program_counter = stack_pop().u.number;
}

void instruction_DROP(int immediate) {
    for (int i = 0; i < immediate; i++)
        stack_pop();
}

void instruction_PUSHR() {
    if(njvm.rvr == NULL) {
        fatalError("rvr is null");
    }
    stack_push(stackslot_new_obj(njvm.rvr));
}

void instruction_POPR() {
    njvm.rvr = stack_pop_objref();
}

void instruction_DUP() {
    StackSlot s = stack_pop();
    stack_push(s);
    stack_push(s);
}

void instruction_NEW(int immediate) {
    ObjRef cmpObj = newCompositeObject(immediate);
    for(int i = 0; i < immediate; i++) {
        GET_REFS_PTR(cmpObj)[i] = malloc(8);
    }
    stack_push(stackslot_new_obj(cmpObj));
}

void instruction_GETF(int immediate) {
    ObjRef cmpObj = stack_pop_objref();
    if(IS_PRIMITIVE(cmpObj)) {
        fatalError("not a compound object");
    }
    if(GET_ELEMENT_COUNT(cmpObj) <= immediate) {
        fatalError("compound object out of bounds");
    }
    stack_push(stackslot_new_obj(GET_REFS_PTR(cmpObj)[immediate]));
}

void instruction_PUTF(int immediate) {
    ObjRef value = stack_pop_objref();
    ObjRef cmpObj = stack_pop_objref();
    if(IS_PRIMITIVE(cmpObj)) {
        fatalError("not a compound object");
    }
    if(GET_ELEMENT_COUNT(cmpObj) < immediate) {
        fatalError("compound object index out of bounds");
    }
    GET_REFS_PTR(cmpObj)[immediate] = value;
}

void instruction_NEWA() {
    ObjRef array;
    bip.op1 = stack_pop_objref();
    if(!IS_PRIMITIVE((ObjRef)bip.op1)) {
        fatalError("object is not primitive");
    }
    array = newCompositeObject(bigToInt());
    stack_push(stackslot_new_obj(array));
}

void instruction_GETFA() {
    bip.op1 = stack_pop_objref();
    ObjRef array = stack_pop_objref();
    if(!IS_PRIMITIVE((ObjRef)bip.op1)) {
        fatalError("object is not primitive");
    }
    int index = bigToInt();

    if(index >= GET_ELEMENT_COUNT(array) || index < 0) {
        fatalError("array index out of bounds");
    }

    stack_push(stackslot_new_obj(GET_REFS_PTR(array)[index]));
}

void instruction_PUTFA() {
    ObjRef value = stack_pop_objref();
    bip.op1 = stack_pop_objref();
    ObjRef array = stack_pop_objref();
    if(!IS_PRIMITIVE((ObjRef)bip.op1)) {
        fatalError("object is not primitive");
    }
    int index = bigToInt();

    if(IS_PRIMITIVE(array)) {
        fatalError("array is not compound object");
    }
    if(index >= GET_ELEMENT_COUNT(array) || index < 0) {
        fatalError("array index out of bounds");
    }

    GET_REFS_PTR(array)[index] = value;
}

void instruction_GETSZ() {
    bip.op1 = stack_pop_objref();
    if(IS_PRIMITIVE((ObjRef) bip.op1)) {
        bigFromInt(-1);
        stack_push(stackslot_new_obj(bip.res));
    } else {
        bigFromInt(GET_ELEMENT_COUNT((ObjRef) bip.op1));
        stack_push(stackslot_new_obj(bip.res));
    }
}

void instruction_PUSHN() {
    stack_push(stackslot_new_obj(NULL));
}

void instruction_REFEQ() {
    bip.op1 = stack_pop_objref();
    bip.op2 = stack_pop_objref();
    if(bip.op1 == bip.op2) {
        bigFromInt(1);
        stack_push(stackslot_new_obj(bip.res));
    } else {
        bigFromInt(0);
        stack_push(stackslot_new_obj(bip.res));
    }
}

void instruction_REFNE() {
    bip.op1 = stack_pop_objref();
    bip.op2 = stack_pop_objref();
    if(bip.op1 != bip.op2) {
        bigFromInt(1);
        stack_push(stackslot_new_obj(bip.res));
    } else {
        bigFromInt(0);
        stack_push(stackslot_new_obj(bip.res));
    }
}

void instruction_execute(int instruction) {
    unsigned int opcode = instruction >> 24;
    int immediate = IMMEDIATE(instruction);
    switch (opcode) {
        case HALT:
            instruction_HALT();
            break;
        case PUSHC:
            instruction_PUSHC(immediate);
            break;
        case ADD:
            instruction_ADD();
            break;
        case SUB:
            instruction_SUB();
            break;
        case MUL:
            instruction_MUL();
            break;
        case DIV:
            instruction_DIV();
            break;
        case MOD:
            instruction_MOD();
            break;
        case RDINT:
            instruction_RDINT();
            break;
        case WRINT:
            instruction_WRINT();
            break;
        case RDCHR:
            instruction_RDCHR();
            break;
        case WRCHR:
            instruction_WRCHR();
            break;
        case PUSHG:
            instruction_PUSHG(immediate);
            break;
        case POPG:
            instruction_POPG(immediate);
            break;
        case ASF:
            instruction_ASF(immediate);
            break;
        case RSF:
            instruction_RSF();
            break;
        case PUSHL:
            instruction_PUSHL(immediate);
            break;
        case POPL:
            instruction_POPL(immediate);
            break;
        case EQ:
            instruction_EQ();
            break;
        case NE:
            instruction_NE();
            break;
        case LT:
            instruction_LT();
            break;
        case LE:
            instruction_LE();
            break;
        case GT:
            instruction_GT();
            break;
        case GE:
            instruction_GE();
            break;
        case JMP:
            instruction_JMP(immediate);
            break;
        case BRF:
            instruction_BRF(immediate);
            break;
        case BRT:
            instruction_BRT(immediate);
            break;
        case CALL:
            instruction_CALL(immediate);
            break;
        case RET:
            instruction_RET();
            break;
        case DROP:
            instruction_DROP(immediate);
            break;
        case PUSHR:
            instruction_PUSHR();
            break;
        case POPR:
            instruction_POPR();
            break;
        case DUP:
            instruction_DUP();
            break;
        case NEW:
            instruction_NEW(immediate);
            break;
        case GETF:
            instruction_GETF(immediate);
            break;
        case PUTF:
            instruction_PUTF(immediate);
            break;
        case NEWA:
            instruction_NEWA();
            break;
        case GETFA:
            instruction_GETFA();
            break;
        case PUTFA:
            instruction_PUTFA();
            break;
        case GETSZ:
            instruction_GETSZ();
            break;
        case PUSHN:
            instruction_PUSHN();
            break;
        case REFEQ:
            instruction_REFEQ();
            break;
        case REFNE:
            instruction_REFNE();
            break;
        default:
            printf("unknown opcode\n");
    }
}

void instruction_print(int instruction) {
    unsigned int opcode = instruction >> 24;
    switch (opcode) {
        case HALT:
            printf("%03d:\thalt\n", njvm.program_memory.program_counter - 1);
            break;
        case PUSHC:
            printf("%03d:\tpushc\t%d\n", njvm.program_memory.program_counter - 1, SIGN_EXTEND(IMMEDIATE(instruction)));
            break;
        case ADD:
            printf("%03d:\tadd\n", njvm.program_memory.program_counter - 1);
            break;
        case SUB:
            printf("%03d:\tsub\n", njvm.program_memory.program_counter - 1);
            break;
        case MUL:
            printf("%03d:\tmul\n", njvm.program_memory.program_counter - 1);
            break;
        case DIV:
            printf("%03d:\tdiv\n", njvm.program_memory.program_counter - 1);
            break;
        case MOD:
            printf("%03d:\tmod\n", njvm.program_memory.program_counter - 1);
            break;
        case RDINT:
            printf("%03d:\trdint\n", njvm.program_memory.program_counter - 1);
            break;
        case WRINT:
            printf("%03d:\twrint\n", njvm.program_memory.program_counter - 1);
            break;
        case RDCHR:
            printf("%03d:\trdchr\n", njvm.program_memory.program_counter - 1);
            break;
        case WRCHR:
            printf("%03d:\twrchr\n", njvm.program_memory.program_counter - 1);
            break;
        case PUSHG:
            printf("%03d:\tpushg\t%d\n", njvm.program_memory.program_counter - 1, IMMEDIATE(instruction));
            break;
        case POPG:
            printf("%03d:\tpopg\t%d\n", njvm.program_memory.program_counter - 1, IMMEDIATE(instruction));
            break;
        case ASF:
            printf("%03d:\tasf\t%d\n", njvm.program_memory.program_counter - 1, IMMEDIATE(instruction));
            break;
        case RSF:
            printf("%03d:\trsf\n", njvm.program_memory.program_counter - 1);
            break;
        case PUSHL:
            printf("%03d:\tpushl\t%d\n", njvm.program_memory.program_counter - 1, SIGN_EXTEND(IMMEDIATE(instruction)));
            break;
        case POPL:
            printf("%03d:\tpopl\t%d\n", njvm.program_memory.program_counter - 1, IMMEDIATE(instruction));
            break;
        case EQ:
            printf("%03d:\teq\n", njvm.program_memory.program_counter - 1);
            break;
        case NE:
            printf("%03d:\tne\n", njvm.program_memory.program_counter - 1);
            break;
        case LT:
            printf("%03d:\tlt\n", njvm.program_memory.program_counter - 1);
            break;
        case LE:
            printf("%03d:\tle\n", njvm.program_memory.program_counter - 1);
            break;
        case GT:
            printf("%03d:\tgt\n", njvm.program_memory.program_counter - 1);
            break;
        case GE:
            printf("%03d:\tge\n", njvm.program_memory.program_counter - 1);
            break;
        case JMP:
            printf("%03d:\tjmp\t%d\n", njvm.program_memory.program_counter - 1, IMMEDIATE(instruction));
            break;
        case BRF:
            printf("%03d:\tbrf\t%d\n", njvm.program_memory.program_counter - 1, IMMEDIATE(instruction));
            break;
        case BRT:
            printf("%03d:\tbrt\t%d\n", njvm.program_memory.program_counter - 1, IMMEDIATE(instruction));
            break;
        case CALL:
            printf("%03d:\tcall\t%d\n", njvm.program_memory.program_counter - 1, IMMEDIATE(instruction));
            break;
        case RET:
            printf("%03d:\tret\n", njvm.program_memory.program_counter - 1);
            break;
        case DROP:
            printf("%03d:\tdrop\t%d\n", njvm.program_memory.program_counter - 1, IMMEDIATE(instruction));
            break;
        case PUSHR:
            printf("%03d:\tpushr\n", njvm.program_memory.program_counter - 1);
            break;
        case POPR:
            printf("%03d:\tpopr\n", njvm.program_memory.program_counter - 1);
            break;
        case DUP:
            printf("%03d:\tdup\n", njvm.program_memory.program_counter - 1);
            break;
        case NEW:
            printf("%03d:\tnew\t%d\n", njvm.program_memory.program_counter - 1, IMMEDIATE(instruction));
            break;
        case GETF:
            printf("%03d:\tgetf\t%d\n", njvm.program_memory.program_counter - 1, IMMEDIATE(instruction));
            break;
        case PUTF:
            printf("%03d:\tputf\t%d\n", njvm.program_memory.program_counter - 1, IMMEDIATE(instruction));
            break;
        case NEWA:
            printf("%03d:\tnewa\n", njvm.program_memory.program_counter - 1);
            break;
        case GETFA:
            printf("%03d:\tgetfa\n", njvm.program_memory.program_counter - 1);
            break;
        case PUTFA:
            printf("%03d:\tputfa\n", njvm.program_memory.program_counter - 1);
            break;
        case GETSZ:
            printf("%03d:\tgetsz\n", njvm.program_memory.program_counter - 1);
            break;
        case PUSHN:
            printf("%03d:\tpushn\n", njvm.program_memory.program_counter - 1);
            break;
        case REFEQ:
            printf("%03d:\trefeq\n", njvm.program_memory.program_counter - 1);
            break;
        case REFNE:
            printf("%03d:\trefne\n", njvm.program_memory.program_counter - 1);
            break;
        default:
            printf("unknown opcode\n");
    }
}

void instructions_run() {
    int instruction;
    char e[20];
    do {
        instruction = njvm.program_memory.instructions[njvm.program_memory.program_counter];
        njvm.program_memory.program_counter++;

        if(njvm.debugger.breakpoint_set == true && njvm.debugger.breakpoint == (njvm.program_memory.program_counter - 1)) {
            njvm.debugger.debug = true;
            njvm.debugger.breakpoint_set = false;
        }

        if (njvm.debugger.debug) {
            info:
            instruction_print(instruction);
            printf("DEBUG: inspect, list, breakpoint, step, run, quit, copyrootobj??\n");
            scanf("%s", e);

            switch (e[0]) {
                case 'i':
                    printf("DEBUG [inspect]: stack, data, object\n");
                    scanf("%s", e);
                    if (e[0] == 's') stack_print();
                    if (e[0] == 'd') sda_print();
                    if (e[0] == 'o') {
                        printf("Object reference:\n");
                        int *address;
                        scanf("%p", (void**)&address);

                        if((ObjRef)address == NULL) {
                            if (!IS_PRIMITIVE((ObjRef) address)) {
                                printf("size = %d\n", GET_ELEMENT_COUNT((ObjRef) address));
                                for (int i = 0; i < GET_ELEMENT_COUNT((ObjRef) address); i++) {
                                    printf("address: %p\n", (void *) GET_REFS_PTR((ObjRef) address)[i]);
                                }
                            } else {
                                bip.op1 = (ObjRef) address;
                                printf("value = ");
                                bigPrint(stdout);
                                printf("\n");
                            }
                        } else {
                            printf("value = NULL\n");
                        }
                    }
                    goto info;
                case 'l':
                    instructions_print();
                    printf("\t\t--- end of code ---\n");
                    goto info;
                case 'b':
                    printf("Breakpoint: (program_count)\n");
                    scanf("%u", &njvm.debugger.breakpoint);
                    njvm.debugger.breakpoint_set = true;
                    njvm.debugger.debug = false;
                    break;
                case 's':
                    break;
                case 'r':
                    njvm.debugger.debug = false;
                    break;
                case 'q':
                    goto end_while;
                case 'c':
                    copyRootObjects();
                    break;
            }
        }

        instruction_execute(instruction);
    } while (instruction >> 24 != HALT);
    end_while:
    njvm.program_memory.program_counter = 0;
}

void instructions_print() {
    int instruction;
    do {
        instruction = njvm.program_memory.instructions[njvm.program_memory.program_counter];
        instruction_print(instruction);
        njvm.program_memory.program_counter++;
    } while (instruction >> 24 != HALT);
    njvm.program_memory.program_counter = 0;
}