#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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


int main(void){
    const int N = 12;
    const int steps = 15;

    MPI_Init(NULL, NULL);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

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

    // (local_rows * rank) is the first row of the process ranked rank
    if ((5 >= local_rows * rank) &&  (5 < local_rows * (rank + 1))){ 
        int local_row = 5 - local_rows * rank + 1;
        current[local_row * N + 4] = 1;
        current[local_row * N + 5] = 1;
        current[local_row * N + 6] = 1;
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

    if (rank == 0){
        for (int row = 0; row < N; row++){
            for (int col = 0; col < N; col++){
                printf("%c", global[row * N + col] ? '#' : '-');
            }
            printf("\n");
        }
    }

    free(global);
    free(current); 
    free(next);

    MPI_Finalize();
    
    return 0;
}