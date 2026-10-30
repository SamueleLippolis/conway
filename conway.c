#include "conway.h"

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