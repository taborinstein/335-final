#include "main.h"

#ifndef __PMK_READER_H
#define __PMK_READER_H

int get_object_num(char *objects, char *value);
void read_pmk(task_t **tasks, int *num_tasks, char **objects, int *num_objects, int *errors);
#endif