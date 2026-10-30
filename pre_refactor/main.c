#include <stdio.h>
#include <stdlib.h>

unsigned char evolve_cell(const unsigned char *grid, int N, int row, int col){
        int neighbors = 0;

        for (int dr = -1; dr <= 1; dr++){
            for (int dc = -1; dc <= 1; dc++){
                if (dr == 0 && dc == 0) continue;

                int nr = (row + dr + N) % N;
                int nc = (col + dc + N) % N;

                neighbors += grid[nr * N + nc];
            }
        }

        unsigned char alive = grid[row * N + col];
        return (neighbors == 3 || (alive && neighbors == 2));
    }


int main(void){
    const int N = 10;
    const int steps = 15;

    unsigned char *current = malloc(N * N * sizeof(unsigned char));
    unsigned char *next = malloc(N * N * sizeof(unsigned char));

    if (current == NULL || next == NULL){
        free(current); free(next);
        return 1; 
    }

    for (int i = 0; i < N * N; i++){
        current[i] = 0;
    }

    current[5 * N + 4] = 1;
    current[5 * N + 5] = 1;
    current[5 * N + 6] = 1; 

    for (int step = 0; step < steps; step++){

        for(int row = 0; row < N; row++){
            for (int col = 0; col < N; col++){
                next[row * N + col] = evolve_cell(current, N, row, col);
            }
        }

        unsigned char *temp = current;
        current = next;
        next = temp;
    }

    for (int row = 0; row < N; row++){
        for(int col = 0; col < N; col++){
            printf("%c", current[row * N + col] ? '#' : '-');
        }
        printf("\n");
    }

    free(current); 
    free(next);

    return 0;
}