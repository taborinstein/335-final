#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __MAIN_H
#define __MAIN_H
typedef struct {
    int id;
    char target[256];
    int deps[256]; // this shouldnt be fixed but for this project its fine
    int depcount;
    char commands[256][256];
    int cmdcount;
    int exit; // -1 if still going, otherwise exit code
} task_t;
#endif