#include <ampi_tape.h>

inline int AMPI_Allreduce_stub (void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm ) {
    if(datatype == MPI_DOUBLE) 
	AMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
    else
	MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
}

