#include <iostream>
#include <cmath>
#include <cstdlib>
#include <mpi.h>

using namespace std;

void comp(double *x, double &y, int &n) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  double *buf=new double[n];
  y=0;
  if(rank==0) {
    for(int i=0;i<n;i++) x[i]=x[i]*x[i];
    MPI_Bsend(x,n,MPI_DOUBLE,1,0,MPI_COMM_WORLD);
    MPI_Recv(buf,n,MPI_DOUBLE,1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    MPI_Recv(buf,n,MPI_DOUBLE,1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    for(int i=0;i<n;i++) {
      y+=buf[i];
    }
  }
  if(rank==1) {
    for(int i=0;i<n;i++) x[i]=0;
    MPI_Send(x,n,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
    MPI_Recv(buf,n,MPI_DOUBLE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    for(int i=0;i<n;i++) { 
      buf[i]=sin(buf[i]);
    }
    MPI_Send(buf,n,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
  }
  delete [] buf;
}


int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  double h=1e-6;
  if(argc<2) {
    cout << "Not enough arguments. Missing problem size." << endl;
    MPI_Finalize();
    return 0;
  }
  int numprocs;
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  if(numprocs!=2) {
    cout << "Must be run with number of processes set to 2." << endl;
    MPI_Finalize();
    return 0;
  }
  int n=atoi(argv[1]);
  cout << "Problem size: " << n << endl;
  double *x=new double[n];
  double y=0;
  for(int i=0;i<n;i++) x[i]=(double) i;
  double *x_saved=new double[n];
  for(int i=0;i<n;i++) x_saved[i]=x[i];
  comp(x,y,n);
  cout << "Result:" << y << endl;
  cout << "Derivatives:" << endl;
  double y_saved=y;
  for(int i=0;i<n;i++) {
    for(int j=0;j<n;j++) x[j]=x_saved[j];
    x[i]=x[i]+h;
    comp(x,y,n);
    cout << (y-y_saved)/h << endl;
  }
  MPI_Finalize();
  delete [] x; delete [] x_saved;
  return 0;
}
