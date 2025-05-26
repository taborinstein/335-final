#include "main.h"
#include "pmk_reader.h"
#include "process.h"


void create_task_t_type(MPI_Datatype *task_type) {
    const int nitems = 7;
    int blocklengths[7] = {1, 256, 256, 1, 256 * 256, 1, 1};
    MPI_Datatype types[7] = {
        MPI_INT, 
        MPI_CHAR,
        MPI_INT, 
        MPI_INT, 
        MPI_CHAR,
        MPI_INT, 
        MPI_INT  
    };
    MPI_Aint offsets[7];

    offsets[0] = offsetof(task_t, id);
    offsets[1] = offsetof(task_t, target);
    offsets[2] = offsetof(task_t, deps);
    offsets[3] = offsetof(task_t, depcount);
    offsets[4] = offsetof(task_t, commands);
    offsets[5] = offsetof(task_t, cmdcount);
    offsets[6] = offsetof(task_t, exit);

    MPI_Type_create_struct(nitems, blocklengths, offsets, types, task_type);
    MPI_Type_commit(task_type);
}

void print_task(task_t task) {
    printf("[%s] {\n    id: %d,\n    deps: [", task.target, task.id);
    for (int i = 0; i < task.depcount; i++) {
        printf("%d", task.deps[i]);
        if (i != task.depcount - 1)
            printf(", ");
    }
    printf("],\n    cmds: [\n    ");
    for (int i = 0; i < task.cmdcount; i++) {
        printf("    \"%s\"", task.commands[i]);
        if (i != task.cmdcount - 1)
            printf(",\n    ");
    }
    printf("\n    ],\n    cmdcount: %d,\n    depcount: %d,\n    "
           "exit: %d\n}\n",
           task.cmdcount, task.depcount, task.exit);
}

void free_tasks(task_t *tasks, int num_tasks) {
    
    free(tasks);
}

int main(int argc, char **argv) {
    int rank;
    int comm_sz;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    MPI_Datatype MPI_PTASK;
    create_task_t_type(&MPI_PTASK);

    int num_tasks = 0;
    task_t *tasks;

    char *objects;
    int completions[256] = {[0 ... 255] = 1}; // neat
    int errors = 0;

    int num_objects = 0;

    // node 0 gets the list of tasks to distribute
    if (rank == 0) {

        read_pmk(&tasks, &num_tasks, &objects, &num_objects, &errors);
        // completions = malloc(sizeof(int) * num_objects);
        for(int i = 0; i < num_tasks; i++) completions[tasks[i].id] = 0;
    }

    MPI_Bcast(&errors, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(completions, 256, MPI_INT, 0, MPI_COMM_WORLD);
    if (errors) {
        printf("Node %d is exiting\n", rank);
        free_tasks(tasks, num_tasks);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // broadcast number of tasks
    MPI_Bcast(&num_tasks, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    int *sendcounts = malloc(sizeof(int) * comm_sz);
    int *displs = malloc(sizeof(int) * comm_sz);
    int base = num_tasks / comm_sz;
    int rem = num_tasks % comm_sz;
    
    
    for (int i = 0; i < comm_sz; i++) {
        sendcounts[i] = base + (i < rem ? 1 : 0);
        displs[i] = (i == 0) ? 0 : displs[i - 1] + sendcounts[i - 1];
    }

    int local_ntasks = sendcounts[rank];
    task_t *local_tasks = malloc(sizeof(task_t) * local_ntasks);

    MPI_Scatterv(
        tasks, sendcounts, displs, MPI_PTASK,
        local_tasks, local_ntasks, MPI_PTASK,
        0, MPI_COMM_WORLD
    );



    process_tasks(rank, local_tasks, local_ntasks, completions);
    // print_task(local_tasks[0]);
    // printf("\x1b[0m");

    // printf("Hello from Node %d\n", rank);

    // free tasks
    if (rank == 0) {
        for (int i = 0; i < num_tasks; i++) {
            // print_task(tasks[i]);
        }
        free_tasks(tasks, num_tasks);
    }
    // free(completions);
    MPI_Finalize();
    return 0;
}
