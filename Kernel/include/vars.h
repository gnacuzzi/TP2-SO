#ifndef _VARS_H
#define _VARS_H

typedef enum { BLOCKED = 0,
			   READY,
			   RUNNING } Status;

typedef int (*Function)(int argc, char **argv);

#define STDIN 0
#define STDOUT 1
#define STDERR 2


#endif