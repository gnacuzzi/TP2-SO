#ifndef BUILTINS_H
#define BUILTINS_H

void memState(int argc, char *argv[]);

/* Process Managing */
void killProcess(int argc, char *argv[]);
void changePriority(int argc, char *argv[]);
void blockProcess(int argc, char *argv[]);


#endif