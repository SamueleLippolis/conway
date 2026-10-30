CC = mpicc
CFLAGS = -Wall -Wextra -O2
conway_mpi: main_mpi.c conway.c conway.h config.h
	$(CC) $(CFLAGS) main_mpi.c conway.c -o conway_mpi

.PHONY = clean
clean: 
	rm -f conway_mpi conway