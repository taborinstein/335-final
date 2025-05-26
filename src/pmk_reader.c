#include "pmk_reader.h"
int get_num_objects(char *objects, int *num_objects) {
    int i = 0;
    while(objects[i] != 0) {
        i++;
        if(objects[i] == ' ') (*num_objects)++;
    }
}
int get_object_num(char *objects, char *value) {
    char *obj = objects;
    int index = 0;
    while (*obj != 0) {
        // printf("Comparing %c %c\n", value[0], *obj);
        while (*obj == ' ')
            obj++;
        int matches = 1;
        for (int i = 0; i < strlen(value); i++) {
            if (value[i] != *obj) {
                matches = 0;
                break;
            }
            obj++;
        }
        while (*obj != ' ' && *obj != '\0')
            obj++;
        if (matches)
            return index;
        index++;
        obj++;
    }

    return -1;
}
void read_pmk(task_t **tasks, int *num_tasks, char **objects, int *num_objects, int *errors) {
    int tasks_len = 4;
    *tasks = (task_t *)malloc(sizeof(task_t) * tasks_len);
    // read build.pmk
    FILE *bpmk = fopen("build.pmk", "r");
    if (bpmk == NULL) {
        printf("pmk: No build.pmk found. Stop\n");
        MPI_Finalize();
        exit(1);
    }

    // build.pmk and get the list of tasks to be done
    char line[2048];
    int lnum = 0;
    while (fgets(line, 2048, bpmk) != NULL) {
        // first line should start with @objects
        if (lnum == 0) {
            if (strncmp("@for", line, 4) == 0) {
                printf("pmk: Line 0 in build.pmk must start with @objects\n");
                errors++;
                break;
            }
            *objects = (char *)malloc(sizeof(char) * strlen(line));
            strcpy(*objects, line);
            get_num_objects(*objects, num_objects);
            lnum++;
            continue;
        }
        // if line starts with @for...
        else if (strncmp("@for", line, 4) == 0) {
            (*num_tasks)++;
            if (*num_tasks > tasks_len)
                *tasks = (task_t *)realloc(*tasks,
                                           sizeof(task_t) * (tasks_len *= 2));
            (*tasks)[*num_tasks - 1].exit = -1;
            // iterate through line
            int pos = 4;
            int setter = 0;
            while (line[pos] == ' ')
                pos++;
            while (line[pos] != ' ' && line[pos] != '\n') {
                (*tasks)[*num_tasks - 1].target[setter++] = line[pos];
                (*tasks)[*num_tasks - 1].target[setter] =
                    0; // cheap way to ensure null termination
                pos++;
            }
            (*tasks)[*num_tasks - 1].id =
                get_object_num(*objects, (*tasks)[*num_tasks - 1].target);
            while (line[pos] == ' ')
                pos++;
            (*tasks)[*num_tasks - 1].depcount = 0; // to be safe
            if (line[pos] == '\n')
                continue; // no dependencies
            if (line[pos] != '<') {
                printf("pmk: Invalid syntax on line %d position %d\n", lnum,
                       pos);
                errors++;
                break;
            }
            pos++;
            // char **deps = malloc(sizeof(char*) * 1); // will remalloc
            // this
            while (line[pos] != '\0') {
                while (line[pos] == ' ')
                    pos++;
                char dep[256];
                int i = 0;
                while (line[pos] != ' ' && line[pos] != '\n') {
                    dep[i++] = line[pos];
                    pos++;
                }
                dep[i] = 0;
                int dep_id = get_object_num(*objects, dep);
                if (dep_id < 0) {
                    printf("pmk: Invalid dependency on line %d position %d\n",
                           lnum, pos);
                    errors++;
                    break;
                }
                (*tasks)[*num_tasks - 1]
                    .deps[(*tasks)[*num_tasks - 1].depcount++] = dep_id;

                pos++;
            }
        }
        // otherwise append command to current target
        else {
            if (strncmp("    ", line, 4)) {
                printf("pmk: Indentation error on line %d\n", lnum);
                errors++;
                break;
            }
            char cmd[256]; // = malloc(sizeof(char) * (strlen(line) - 5));
            // if this were a char* and not a char[], we could do line += 4
            // :(
            int i = 0;
            int len = strlen(line);
                strncpy((*tasks)[*num_tasks - 1]
                            .commands[(*tasks)[*num_tasks - 1].cmdcount],
                        line + 4, len - (line[len - 1] == '\n' ? 5 : 4));
            (*tasks)[*num_tasks - 1].cmdcount++;
        }

        lnum++;
    }
}