#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char target[256];
    int deps[256]; // this shouldnt be fixed but for this project its fine
    int depcount;
    char **commands;
    int cmdcount;
    int exit; // -1 if still going, otherwise exit code
} task_t;

void print_task(task_t task) {
    printf("[%s] {\n    id: %d,\n    deps: [", task.target,
           task.id);
    for (int i = 0; i < task.depcount; i++) {
        printf("%d", task.deps[i]);
        if (i != task.depcount - 1)
            printf(", ");
    }
    printf("],\n    depcount: %d,\n    "
           "exit: %d\n}\n",
           task.depcount, task.exit);
}

void free_tasks(task_t *tasks, int num_tasks) {
    for (int i = 0; i < num_tasks; i++) {
        // free(tasks[i].)
    }
}
int get_object_num(char *objects, char *value) {
    char *obj = objects;
    int index = 0;
    while (*obj != 0) {
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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int main(int argc, char **argv) {
    int rank;
    int comm_sz;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int num_tasks = 0;
    int tasks_len = 0;
    task_t *tasks;

    char *objects;

    int errors = 0;

    // node 0 gets the list of tasks to distribute
    if (rank == 0) {
        tasks_len = 4;
        tasks = malloc(sizeof(task_t) * tasks_len);
        // read build.pmk
        FILE *bpmk = fopen("build.pmk", "r");
        if (bpmk == NULL) {
            printf("pmk: No build.pmk found. Stop\n");
            MPI_Finalize();
            exit(1);
        }

        // build.pmk and get the list of tasks to be done
        char line[256];
        int lnum = 0;
        while (fgets(line, 256, bpmk) != NULL) {
            if (lnum == 0) {
                // first line should start with @objects
                if (strncmp("@for", line, 4) == 0) {
                    printf(
                        "pmk: Line 0 in build.pmk must start with @objects\n");
                    errors++;
                    break;
                }
                objects = malloc(sizeof(char) * strlen(line));
                strcpy(objects, line);
                lnum++;
                continue;
            }
            // if line starts with @for...
            if (strncmp("@for", line, 4) == 0) {
                num_tasks++;
                if (num_tasks > tasks_len)
                    tasks = realloc(tasks, sizeof(task_t) * (tasks_len *= 2));

                // iterate through line
                int pos = 4;
                int setter = 0;
                while (line[pos] == ' ')
                    pos++;
                while (line[pos] != ' ' && line[pos] != '\n') {
                    tasks[num_tasks - 1].target[setter++] = line[pos];
                    tasks[num_tasks - 1].target[setter] =
                        0; // cheap way to ensure null termination
                    pos++;
                }
                tasks[num_tasks - 1].id =
                    get_object_num(objects, tasks[num_tasks - 1].target);
                while (line[pos] == ' ')
                    pos++;
                tasks[num_tasks - 1].depcount = 0; // to be safe
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
                    printf("%d\n", pos);
                    char dep[256];
                    int i = 0;
                    while(line[pos] != ' ' && line[pos] != '\n') {
                        dep[i++] = line[pos];
                        pos++;
                    }
                    dep[i] = 0;
                    int dep_id = get_object_num(objects, dep);
                    if(dep_id < 0) {
                        printf("pmk: Invalid dependency on line %d position %d\n",
                               lnum, pos);
                        errors++;
                        break;
                    }
                    tasks[num_tasks - 1].deps[tasks[num_tasks - 1].depcount++] = dep_id;

                        pos++;
                }
            }
            lnum++;
        }
    }

    MPI_Bcast(&errors, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (errors) {
        printf("Node %d is exiting\n", rank);
        free_tasks(tasks, num_tasks);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // printf("Hello from Node %d\n", rank);

    // free tasks
    if (rank == 0) {
        for (int i = 0; i < num_tasks; i++) {
            print_task(tasks[i]);
        }
        free_tasks(tasks, num_tasks);
    }
    MPI_Finalize();
    return 0;
}