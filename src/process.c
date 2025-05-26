#include "process.h"
#include "main.h"
int process_tasks(int rank, task_t *tasks, int num_tasks, int *completions) {
    while (1) {
        MPI_Allreduce(MPI_IN_PLACE, completions, 256, MPI_INT, MPI_LOR,
                      MPI_COMM_WORLD);
        
        for (int i = 0; i < num_tasks; i++) {
            if (tasks[i].exit != -1)
                continue;
            int deps_satisfied = 0;
            for (int d = 0; d < tasks[i].depcount; d++) {
                if (completions[tasks[i].deps[d]])
                    deps_satisfied++;
                // printf("[%d] Task %d depends on %d (status %d)\n", rank,

            }
            // printf("[%d] Task %d has %d/%d deps satisfied\n", rank, tasks[i].id,
                //    deps_satisfied, tasks[i].depcount);
            if (deps_satisfied == tasks[i].depcount) {
                for(int c = 0; c < tasks[i].cmdcount; c++) {
                    printf("\x1b[3%dm[%d] %s\x1b[0m\n", (rank % 6) + 2 , rank,
                           tasks[i].commands[c]);
                    system(tasks[i].commands[c]);
                }
                completions[tasks[i].id] = 1;
                tasks[i].exit = 0;
                // printf("[%d] Task %d completed\n", rank, tasks[i].id);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
        int local_done = 1;
        for (int i = 0; i < num_tasks; i++) {
            if (tasks[i].exit == -1) {
                local_done = 0;
                break;
            }
        }
        int global_done = 0;
        MPI_Allreduce(&local_done, &global_done, 1, MPI_INT, MPI_LAND,
                      MPI_COMM_WORLD);

        if (global_done)
            break;
        
    }
}