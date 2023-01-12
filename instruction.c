#include "instruction.h"
#include "stack.h"
#include "makro.h"
#include <bigint.h>

void instruction_HALT() {
    free_program_memory();
    free_sda();
    free_stack();
}

void instruction_PUSHC(unsigned int immediate) {
    bigFromInt(SIGN_EXTEND(immediate));
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_ADD() {
    bip.op2 = stack_pop().u.objRef;
    bip.op1 = stack_pop().u.objRef;
    bigAdd();
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_SUB() {
    bip.op2 = stack_pop().u.objRef;
    bip.op1 = stack_pop().u.objRef;
    bigSub();
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_MUL() {
    bip.op2 = stack_pop().u.objRef;
    bip.op1 = stack_pop().u.objRef;
    bigMul();
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_DIV() {
    bip.op2 = stack_pop().u.objRef;
    bip.op1 = stack_pop().u.objRef;
    bigDiv();
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_MOD() {
    bip.op2 = stack_pop().u.objRef;
    bip.op1 = stack_pop().u.objRef;
    bigDiv();
    stack_push(stackslot_new_obj(bip.rem));
}

void instruction_RDINT() {
    bigRead(stdin);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_WRINT() {
    bip.op1 = stack_pop().u.objRef;
    bigPrint(stdout);
}

void instruction_RDCHR() {
    char input;
    scanf("%c", &input);
    bigFromInt((int) input);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_WRCHR() {
    bip.op1 = stack_pop().u.objRef;
    printf("%c", (char) bigToInt());
}

void instruction_PUSHG(unsigned int immediate) {
    stack_push(stackslot_new_obj(
            njvm.sda.sda[immediate]));
}

void instruction_POPG(unsigned int immediate) {
    ObjRef ob = stack_pop().u.objRef;
    njvm.sda.sda[immediate] = ob;
}

void instruction_ASF(unsigned int immediate) {
    stack_push(stackslot_new(njvm.stack.frame_pointer));
    njvm.stack.frame_pointer = njvm.stack.stack_pointer;
    njvm.stack.stack_pointer += immediate;
}

void instruction_RSF() {
    njvm.stack.stack_pointer = njvm.stack.frame_pointer;
    njvm.stack.frame_pointer = stack_pop().u.number;
}

void instruction_PUSHL(unsigned int immediate) {
    stack_push(njvm.stack.stack[njvm.stack.frame_pointer + SIGN_EXTEND(immediate)]);
}

void instruction_POPL(unsigned int immediate) {
    njvm.stack.stack[njvm.stack.frame_pointer + SIGN_EXTEND(immediate)] = stack_pop();
}

void instruction_EQ() {
    bip.op2 = stack_pop().u.objRef;
    bip.op1 = stack_pop().u.objRef;
    bigFromInt(bigCmp() == 0 ? 1 : 0);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_NE() {
    bip.op2 = stack_pop().u.objRef;
    bip.op1 = stack_pop().u.objRef;
    bigFromInt(bigCmp() != 0 ? 1 : 0);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_LT() {
    bip.op2 = stack_pop().u.objRef;
    bip.op1 = stack_pop().u.objRef;
    bigFromInt(bigCmp() < 0 ? 1 : 0);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_LE() {
    bip.op2 = stack_pop().u.objRef;
    bip.op1 = stack_pop().u.objRef;
    bigFromInt(bigCmp() <= 0 ? 1 : 0);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_GT() {
    bip.op2 = stack_pop().u.objRef;
    bip.op1 = stack_pop().u.objRef;
    bigFromInt(bigCmp() > 0 ? 1 : 0);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_GE() {
    bip.op2 = stack_pop().u.objRef;
    bip.op1 = stack_pop().u.objRef;
    bigFromInt(bigCmp() >= 0 ? 1 : 0);
    stack_push(stackslot_new_obj(bip.res));
}

void instruction_JMP(unsigned int immediate) {
    njvm.program_memory.program_counter = immediate;
}

void instruction_BRF(unsigned int immediate) {
    bip.op1 = stack_pop().u.objRef;
    if (!bigToInt()) njvm.program_memory.program_counter = immediate;
}

void instruction_BRT(unsigned int immediate) {
    bip.op1 = stack_pop().u.objRef;
    if (bigToInt() == 1) njvm.program_memory.program_counter = immediate;
}

void instruction_CALL(unsigned int immediate) {
    stack_push(stackslot_new(njvm.program_memory.program_counter));
    njvm.program_memory.program_counter = immediate;
}

void instruction_RET() {
    njvm.program_memory.program_counter = stack_pop().u.number;
}

void instruction_DROP(unsigned int immediate) {
    for (int i = 0; i < immediate; i++)
        stack_pop();
}

void instruction_PUSHR() {
    stack_push(stackslot_new_obj(njvm.rvr));
}

void instruction_POPR() {
    njvm.rvr = stack_pop().u.objRef;
}

void instruction_DUP() {
    StackSlot s = stack_pop();
    stack_push(s);
    stack_push(s);
}

void instruction_execute(unsigned int instruction) {
    unsigned int opcode = instruction >> 24;
    unsigned int immediate = IMMEDIATE(instruction);
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
        default:
            printf("unknown opcode\n");
    }
}

void instruction_print(unsigned int instruction) {
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
            printf("%03d:\tpopl\t%d\n", njvm.program_memory.program_counter - 1, SIGN_EXTEND(IMMEDIATE(instruction)));
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
        default:
            printf("unknown opcode\n");
    }
}

void instructions_run() {
    unsigned int instruction;
    char e[20];
    do {
        instruction = njvm.program_memory.instructions[njvm.program_memory.program_counter];
        njvm.program_memory.program_counter++;

        if(njvm.breakpoint != NULL & njvm.breakpoint == (njvm.program_memory.program_counter - 1)) {
            njvm.debug = true;
            njvm.breakpoint = NULL;
        }

        if (njvm.debug) {
            info:
            instruction_print(instruction);
            printf("DEBUG: inspect, list, breakpoint, step, run, quit?\n");
            scanf("%s", e);

            switch (e[0]) {
                case 'i':
                    printf("DEBUG [inspect]: stack, data, object?\n");
                    scanf("%s", e);
                    if (e[0] == 's') stack_print();
                    if (e[0] == 'd') sda_print();
                    if (e[0] == 'o') {
                        printf("Object reference:\n");
                        int *address;
                        scanf("%p", (void**)&address);
                        bip.op1 = (ObjRef) address;

                        printf("value = ");
                        bigPrint(stdout);
                        printf("\n");
                    }
                    goto info;
                case 'l':
                    instructions_print();
                    printf("\t\t--- end of code ---\n");
                    goto info;
                case 'b':
                    printf("Breakpoint: (program_count)\n");
                    scanf("%d", &njvm.breakpoint);
                    njvm.debug = false;
                    break;
                case 's':
                    break;
                case 'r':
                    njvm.debug = false;
                    break;
                case 'q':
                    goto end_while;
            }
        }

        instruction_execute(instruction);
    } while (instruction >> 24 != HALT);
    end_while:
    njvm.program_memory.program_counter = 0;
}

void instructions_print() {
    unsigned int instruction;
    do {
        instruction = njvm.program_memory.instructions[njvm.program_memory.program_counter];
        instruction_print(instruction);
        njvm.program_memory.program_counter++;
    } while (instruction >> 24 != HALT);
    njvm.program_memory.program_counter = 0;
}