#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "config.h"

unsigned char evolve_cell(const unsigned char *grid, int N, int row, int col){
        int neighbors = 0;

        for (int dr = -1; dr <= 1; dr++){
            for (int dc = -1; dc <= 1; dc++){
                if (dr == 0 && dc == 0) continue;

                int nr = row + dr;
                int nc = (col + dc + N) % N;

                neighbors += grid[nr * N + nc];
            }
        }

        unsigned char alive = grid[row * N + col];
        return (neighbors == 3 || (alive && neighbors == 2));
    }


int main(int argc, char *argv[]){
    const int N = (argc > 1) ? atoi(argv[1]) : DEFAULT_N;
    const int steps = (argc > 2) ? atoi(argv[2]) : DEFAULT_STEPS;
    const int mode = (argc > 3) ? atoi(argv[3]) : DEFAULT_MODE;

    MPI_Init(NULL, NULL);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N < 7 || steps < 1 || (mode != 0 && mode != 1)){
        if (rank == 0) fprintf(stderr, "Invalid N, steps or mode (must: N > 6, mode = 0 or 1)");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (N % size != 0) MPI_Abort(MPI_COMM_WORLD, 1);
    const int local_rows = N / size; 

    int up = (rank - 1 + size) % size;
    int down = (rank + 1 + size) % size;  

    unsigned char *current = malloc((local_rows + 2) * N * sizeof(unsigned char));
    unsigned char *next = malloc((local_rows + 2) * N * sizeof(unsigned char));

    if (current == NULL || next == NULL){
        free(current); free(next);
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1; 
    }

    for (int i = 0; i < (local_rows + 2) * N; i++){
        current[i] = 0;
    }

    if (mode == 0){
        // (local_rows * rank) rows-count of the previous processes  
        if ((5 >= local_rows * rank) &&  (5 < local_rows * (rank + 1))){ 
            int local_row = 5 - local_rows * rank + 1;
            current[local_row * N + 4] = 1;
            current[local_row * N + 5] = 1;
            current[local_row * N + 6] = 1;
        }
    } 
    else {
        for (int row = 1; row <= local_rows; row++) {
            for (int col = 0; col < N; col++){
                // row is the row of the single process (e.g., row in [0,1,2,3,4])
                // where 0 and 5 are ghosts 
                unsigned int id = (rank * local_rows + row - 1) * N + col;
                unsigned int x = id * 2654435761u + 1013904223u;
                x ^= x >> 16;
                current[row * N + col] = ((x % 100) < 30);
            }
        }
    } 


    for (int step = 0; step < steps; step++){

        MPI_Sendrecv(
            &current[N], N, MPI_UNSIGNED_CHAR, up, 0,
            &current[(local_rows + 1) * N], N, MPI_UNSIGNED_CHAR, down, 0,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE
        );

        MPI_Sendrecv(
            &current[local_rows * N], N, MPI_UNSIGNED_CHAR, down, 1,
            &current[0], N, MPI_UNSIGNED_CHAR, up, 1,
            MPI_COMM_WORLD, MPI_STATUS_IGNORE
        );

        for(int row = 1; row <= local_rows; row++){
            for (int col = 0; col < N; col++){
                next[row * N + col] = evolve_cell(current, N, row, col);
            }
        }

        unsigned char *temp = current;
        current = next;
        next = temp;
    }

    unsigned char *global = NULL;
    if (rank == 0){
        global = malloc(N * N * sizeof(unsigned char));
    }
    if (rank == 0 && global == NULL){
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Gather(
        &current[N], local_rows * N, MPI_UNSIGNED_CHAR, 
        global, local_rows * N, MPI_UNSIGNED_CHAR,
        0, MPI_COMM_WORLD
    );

    char filename[128];
    snprintf(filename, sizeof(filename), "reports/output_%d_%d_%d_%d.txt", N, steps, mode, size);
    FILE *out = fopen(filename, "w");
    if (out == NULL) {
        fprintf(stderr, "Error opening output file\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    if (rank == 0){
        for (int row = 0; row < N; row++){
            for (int col = 0; col < N; col++){
                fprintf(out, "%c", global[row * N + col] ? '#' : '-');
            }
            fprintf(out, "\n");
        }
    }
    fclose(out);

    free(global);
    free(current); 
    free(next);

    MPI_Finalize();
    
    return 0;
}