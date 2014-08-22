#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <strings.h>
#include <assert.h>

#define DF_MMU_DEV_PATH "/dev/df_mmu"

static int df_mmu_fd = -1;

static void
close_df_mmu(void)
{
	if (df_mmu_fd != -1) {
		(void) close(df_mmu_fd);	/* someone else may have closed it */
		df_mmu_fd = -1;
	}
}

static int
open_df_mmu(void)
{
	int ret = 0;

	if ((df_mmu_fd = open(DF_MMU_DEV_PATH, O_RDWR)) == -1)
		return -1;

        return ret;
}

int main(int argc, char *argv[]) {
  int i;
  int minsize=(1<<28), /* since we use doubles, this yields 2 MiB */
    maxsize=(1<<28);   /* since we use doubles, this yields 2 GiB */
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int ncpu;
  MPI_Comm_size(MPI_COMM_WORLD, &ncpu);
  size_t size = (maxsize)*sizeof(double);
  int iters = 20;
  // MPI_Alloc_mem or malloc?
  double *s_buf;
  double *r_buf;

 open_df_mmu();

#ifndef USE_MPI_ALLOC_MEM
  s_buf = (void*) malloc(size);
  r_buf = (void*) malloc(size);
#else
  int ierr;
  ierr = MPI_Alloc_mem(size, MPI_INFO_NULL, (void* ) &s_buf);
  assert(ierr==0);
  ierr = MPI_Alloc_mem(size, MPI_INFO_NULL, (void* ) &r_buf);
  assert(ierr==0);
#endif
  bzero(s_buf, size);
  memset(r_buf,'U',size);
  // start clock
  if (rank == 0) {
    printf("% 20s\t%12s\t%12s\n", "size [MiB]", "time [s]", "BW [MiB/s]");
  }
 
 // so the comms
  for (size = maxsize*sizeof(double); size >= minsize*sizeof(double); size = size >> 1) {
    double t_start = MPI_Wtime();
    for (i=0; i<iters; i++) {
#ifndef DONT_USE_MPI_ALLTOALL
      MPI_Alltoall(s_buf, size/sizeof(double)/ncpu, MPI_DOUBLE,
		   r_buf, size/sizeof(double)/ncpu, MPI_DOUBLE, MPI_COMM_WORLD);
#else
      // try the equivalent send/recv construct!
      int sendcount = size/sizeof(double)/ncpu;
      int tag = 0;
      MPI_Request MPI_sreq[ncpu], MPI_rreq[ncpu];
      MPI_Status status;
      for (int i = 0; i < ncpu; i++) 
	MPI_Isend(s_buf + i * sendcount ,// * sizeof(double),
		  sendcount, MPI_DOUBLE, i, tag, MPI_COMM_WORLD, &(MPI_sreq[i]));
      for (int i = 0; i < ncpu; i++)
	MPI_Irecv(r_buf + i * sendcount ,// * sizeof(recvtype),
		  sendcount, MPI_DOUBLE, i, tag, MPI_COMM_WORLD, &(MPI_rreq[i]));
      for (int i=0; i<ncpu;i++) {
	MPI_Wait(&(MPI_sreq[i]), &status);
	MPI_Wait(&(MPI_rreq[i]), &status);
      }
#endif
    }
    MPI_Barrier(MPI_COMM_WORLD);
    double t_end = MPI_Wtime();
    if (rank == 0) {
      double time = (t_end-t_start)/iters;
      double bw = size/time/(1<<20);
      printf("% 20g\t%2.10f\t%12.2f\n", size/((double)(1<<20)), time, bw);
    }
  }

  // disable this...
  if (ncpu<1) { 
    // for comparison, a point-to-point sendrecv
    MPI_Request MPI_req;
    MPI_Status status;
    double t_start = MPI_Wtime();
    size = (1<<26)*sizeof(double);
    for (i=0; i<iters; i++) {
      if (rank == 0) {
	MPI_Isend(s_buf, size/sizeof(double), MPI_DOUBLE, ncpu-1, 0, MPI_COMM_WORLD, &MPI_req);
	MPI_Wait(&MPI_req, &status);
      } else {
	if (rank == ncpu-1) {
	  MPI_Irecv(r_buf, size/sizeof(double), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &MPI_req);
	  MPI_Wait(&MPI_req, &status);
	}
      }
    }
   MPI_Barrier(MPI_COMM_WORLD);
    double t_end = MPI_Wtime();
    if (rank == 0) {
      double time = (t_end-t_start)/iters;
      double bw = size/time/(1<<20);
      printf("% 20g\t%2.10f\t%12.2f\n", size/((double)(1<<20)), time, bw);
    }
  }
  close_df_mmu();

  MPI_Finalize();
}
 
