
#ifndef NJVM_H
#define NJVM_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define VERSION 8
#define BINARY_FORMAT "NJBF"

typedef struct {
    void *forward_pointer;
    unsigned int size;
    bool broken_heart;
    unsigned char data[1];
} *ObjRef;

typedef struct {
    bool isObjRef;
    union {
        ObjRef objRef;
        unsigned int number;
    } u;
} StackSlot;

typedef struct {
    unsigned int program_counter;
    unsigned int size;
    unsigned int *instructions;
} ProgramMemory;

typedef struct {
    unsigned int size;
    bool purge;
    char *active;
    char *passive;
    char *next;
    char *end;
} Heap;

typedef struct {
    unsigned int stack_pointer;
    unsigned int frame_pointer;
    unsigned int size;
    StackSlot *stack;
} Stack;

typedef struct {
    unsigned int size;
    ObjRef *sda;
} StaticDataArea;

typedef struct {
    bool debug;
    bool breakpoint_set;
    unsigned int breakpoint;
} Debugger;

typedef struct {
    ProgramMemory program_memory;
    Heap heap;
    Stack stack;
    StaticDataArea sda;
    ObjRef rvr;
    Debugger debugger;
    FILE *file;
} NinjaVM;


extern NinjaVM njvm;

int njvm_start(int argc, char *argv[]);

void free_heap();

void free_stack();

void free_program_memory();

void free_sda();

#endif //NJVM_H
