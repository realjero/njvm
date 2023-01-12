#include "njvm.h"
#include "instruction.h"
#include "support.h"

/*
 * gcc -g -Wall -std=c99 -pedantic -I./include -L./lib njvm.c instruction.c stack.c support.c -lbigint -o vm
 */

NinjaVM njvm;

NinjaVM vm_check_binary_format(NinjaVM vm) {
    char binary_format[4];
    fread(binary_format, sizeof(char), 4, vm.file);

    if(strcmp(binary_format, BINARY_FORMAT) != 0) {
        printf("BINARY_FORMAT error");
        exit(0);
    }
    return vm;
}

NinjaVM vm_check_version(NinjaVM vm) {
    unsigned int version;
    fread(&version, sizeof(unsigned int), 1, vm.file);

    if(version != VERSION) {
        printf("VERSION error\n");
        exit(0);
    }
    return vm;
}

NinjaVM vm_init_program_memory(NinjaVM vm) {
    fread(&vm.program_memory.size, sizeof(unsigned int), 1, vm.file);

    if(vm.program_memory.size == 0)
        fatalError("vm_init_program_size() size");

    vm.program_memory.instructions = malloc(vm.program_memory.size * sizeof(unsigned int));
    if(vm.program_memory.instructions == NULL)
        fatalError("vm_init_program_size() malloc");

    vm.program_memory.program_counter = 0;
    return vm;
}

NinjaVM vm_init_sda(NinjaVM vm) {
    fread(&vm.sda.size, sizeof(unsigned int), 1, vm.file);
    vm.sda.sda = malloc(vm.sda.size * sizeof(ObjRef));
    if(vm.sda.sda == NULL)
        fatalError("vm_init_sda() malloc");
    return vm;
}

NinjaVM vm_init_stack(NinjaVM vm) {
    vm.stack.stack_pointer = 0;
    vm.stack.frame_pointer = 0;
    vm.stack.size = STACK_MAX_ITEMS;
    vm.stack.stack = malloc(vm.stack.size * sizeof(StackSlot));
    if(vm.stack.stack == NULL)
        fatalError("vm_init_stack() malloc");
    return vm;
}

NinjaVM vm_load_program(NinjaVM vm) {
    fread(vm.program_memory.instructions, sizeof(unsigned int), vm.program_memory.size, vm.file);
    return vm;
}

NinjaVM vm_init_debug(NinjaVM vm, bool debug) {
    vm.debugger.breakpoint_set = debug;
    vm.debugger.breakpoint = 0;
    return vm;
}

NinjaVM vm_init(NinjaVM vm, char file[], bool debug) {
    if((vm.file = fopen(file, "r")) == NULL) {
        printf("Error: cannot open code file '%s'", file);
        exit(0);
    }

    vm = vm_check_binary_format(vm);
    vm = vm_check_version(vm);
    vm = vm_init_program_memory(vm);
    vm = vm_init_sda(vm);
    vm = vm_init_stack(vm);
    vm = vm_load_program(vm);
    vm = vm_init_debug(vm, debug);

    fclose(vm.file);

    return vm;
}

void free_stack() {
    free(njvm.stack.stack);
}

void free_program_memory() {
    free(njvm.program_memory.instructions);
}

void free_sda() {
    free(njvm.sda.sda);
}

NinjaVM arguments(NinjaVM vm, int argc, char *argv[]) {
    bool debug = false;
    if(argc != 1) {
        for(int i = 1; i < argc; i++) {
            if(argv[i][0] == '-') {
                if (strcmp(argv[i], "--help") == 0) {
                    printf("usage: ./njvm_start [option] [option] ...\n");
                    printf("  --debug          start virtual machine in debug mode\n");
                    printf("  --version        show version and exit\n");
                    printf("  --help           show this help and exit\n");
                    exit(0);
                } else if (strcmp(argv[i], "--debug") == 0) {
                    debug = true;
                    i++;
                } else if (strcmp(argv[i], "--version") == 0) {
                    printf("Ninja Virtual Machine version 5 (compiled Sep 23 2015, 10:36:52)\n");
                    exit(0);
                } else {
                    printf("unknown command line argument '%s', try './njvm_start --help'\n", argv[i]);
                    exit(0);
                }
            }
        }
    } else {
        printf("Error: no code file specified");
        exit(0);
    }
    vm = vm_init(vm, argv[argc - 1], debug);
    return vm;
}


int njvm_start(int argc, char *argv[]) {
    njvm = arguments(njvm, argc, argv);
    printf("Ninja Virtual Machine started\n");

    instructions_run();

    printf("Ninja Virtual Machine stopped\n");
    return 0;
}

int main(int argc, char *argv[]) {
    obj_compound_new(2);
    return njvm_start(argc, argv);
}