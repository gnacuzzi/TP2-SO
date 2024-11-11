#ifndef BUILTINS_H
#define BUILTINS_H

#include <stddef.h>

void memState(int argc, char *argv[]);

/* Process Managing */
void killProcess(int argc, char *argv[]);
void changePriority(int argc, char *argv[]);
void blockProcess(int argc, char *argv[]);
void unblockProcess(int argc, char *argv[]);
void listProcesses(int argc, char *argv[]);


#endif