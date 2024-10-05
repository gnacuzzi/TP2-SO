#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>
#include <vars.h>

#define CANT_FILE_DESCRIPTORS 5

typedef struct PCB{
    uint16_t pid;
    uint16_t parentPid;
    void *stackBase;
    void *stackPos;
    char **argv;
    char *name;
    uint8_t priority;
    Status status;
    int16_t fileDescriptors[BUILT_IN_DESCRIPTORS];
} PCB;

// Declaración de la función de inicialización del marco de pila
void * setupStackFrame(void *executeProcess, Function code, void *stackEnd, void *args);

#endif