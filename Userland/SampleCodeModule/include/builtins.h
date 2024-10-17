#ifndef BUILTINS_H
#define BUILTINS_H

void memState(char *argv[], int argc);

/* Process Managing */
void killProcess(char *argv[], int argc);
void changePriority(char *argv[], int argc);
void blockProcess(char *argv[], int argc);


#endif