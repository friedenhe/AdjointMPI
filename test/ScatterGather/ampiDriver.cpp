#include <iostream>
#include <cmath>
#include <cstdlib>
#include "dco.hpp"
#include "ampi_tape.hpp"

using namespace std;

typedef dco::ga1s<double>::type active;



void comp(active *x, active &y, int n) {
  int numprocs, rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  int n_local=n/numprocs;
  active *buf=new active[n_local];
  y=0;
  for(int i=0;i<n;i++) x[i]=x[i]*x[i];
  AMPI_Scatter(x,n_local,MPI_DOUBLE,buf,n_local,MPI_DOUBLE,0,MPI_COMM_WORLD);
  for(int i=0;i<n_local;i++) { 
    buf[i]=sin(buf[i]);
  }
  AMPI_Gather(buf,n_local,MPI_DOUBLE,x,n_local,MPI_DOUBLE,0,MPI_COMM_WORLD);
  if(rank==0) {
    for(int i=0;i<n;i++) {
      y+=x[i];
    }
  }
  delete [] buf;
}


int main(int argc, char *argv[]) {
  AMPI_Init(&argc, &argv);
  dco::ga1s<double>::global_tape = dco::ga1s<double>::tape_t::create(1e4);
  active h=1e-6;
  if(argc<2) {
    cout << "Not enough arguments. Missing problem size." << endl;
    AMPI_Finalize();
    return 0;
  }
  int numprocs,rank;
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  int n=atoi(argv[1]);
  cout << "Problem size: " << n << endl;
  active *x=new active[n*numprocs];
  active y=0;
  if(rank==0) {
    for(int i=0;i<numprocs;i++) {
      for(int j=0;j<n;j++) x[i*n+j]=(double) j;
    }
  }
  else {
    for(int i=0;i<n*numprocs;i++) x[i]=0;
  }
  if(rank==0) {
    for(int i=0;i<n*numprocs;i++)
      dco::ga1s<double>::global_tape->register_variable(x[i]);
  }
  active *x_saved=new active[n*numprocs];
  for(int i=0;i<n*numprocs;i++) x_saved[i]=x[i];
  comp(x,y,n*numprocs);
  cout << "Result:" << y << endl;
  cout << "Derivatives:" << endl;
  if(rank == 0) dco::ga1s<double>::set(y, 1., -1);
  if(rank == 1) dco::ga1s<double>::set(y, 0., -1);
  dco::ga1s<double>::global_tape->interpret_adjoint();
  double g=0;
  for(int i=0;i<n*numprocs;i++) {
    dco::ga1s<double>::get(x_saved[i], g, -1);
    cout << g << endl;
  }
    
  AMPI_Finalize();
  delete [] x; delete [] x_saved;
  return 0;
}
