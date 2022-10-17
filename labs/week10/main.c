#include "file_reader.c"
#include "quadratic_root.c"
#include <mpi.h>
#include <stdio.h>


MPI_Datatype create_root_datatype(root the_root);

int main(int argc, char *argv[]) {
  int rank, size;
  MPI_Comm new_comm;
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  char *file_name = "test1.txt";

  MPI_Status status[3];
  MPI_Request request[3];
  int number_of_rows = fetch_number_of_rows(file_name);
  int *lines = init_process_instruction_pointers(file_name, number_of_rows);

  root roots[3];
  MPI_Datatype mpi_root = create_root_datatype(roots[0]);

  if (rank == 0) {
    /*
     * Fetch the row and get coefficients for quadratic equation
     * Calculate the d value
     * Send to node 1
     */

    /*
     * Observations:
     *
     * Need to create a new mpi data type to send struct in openmpi between
     * processes. Otherwise need to extract the values out of the struct and
     * send as array of floats Float is a primitive data structure.
     *
     */

    float send[4];
    write_number_of_rows_to_output(number_of_rows);

    for (int i = 0; i < number_of_rows; i++) {
      float *coef = fetch_next_instruction(lines[i], file_name);
      float d = calculate_d(coef[0], coef[1], coef[2]);
      send[0] = d;
      send[1] = coef[0];
      send[2] = coef[1];
      printf("Row %i, Value d is %.2f.\n", i, d);
      MPI_Send(&send[0], 3, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
    }

  } else if (rank == 1) {
    /*
     * Receive the d value and coefficients from process rank 1,
     * Calculate the roots.
     * Send to node 2
     */

    /*
     * Observations:
     *
     * Need to convert integer into float in order for the arithmetic to work in c.
     *
     */
    float d;

    for (int i = 0; i < number_of_rows; i++) {
      float recv[3];
      MPI_Recv(&recv[0], 3, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status[rank]);
      printf("Row %i: Coffients are d %.2f, a %.2f, b %.2f\n", i, recv[0],
             recv[1], recv[2]);

      root *roots = quadratic_root(recv[1], recv[2], recv[0]);
      printf("Row %i: Roots are %.2f+%.2fi, %.2f-%.2fi.\n", i, roots[0].real,
             roots[0].img, roots[1].real, roots[0].img);
      float send[4] = {roots[0].real, roots[0].img, roots[1].real,
                       roots[1].img};

      MPI_Send(&send[0], 4, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
    }

  } else if (rank == 2) {
    float receive_vals[4];

    for (int i = 0; i < number_of_rows; i++) {
      MPI_Recv(&receive_vals[0], 4, MPI_FLOAT, 1, 0, MPI_COMM_WORLD,
               &status[rank]);
      output_result(receive_vals);
    }
  }

  MPI_Finalize();
  return 0;
}

MPI_Datatype create_root_datatype(root the_root) {
  // The number data type is 1 and just float
  MPI_Datatype array_of_types[1];
  array_of_types[0] = MPI_FLOAT;

  // The number of blocks length are 1 and it is 2 because 2 blocks.
  int array_of_blocklengths[1];
  array_of_blocklengths[0] = 2;

  // The distance between the first and second block
  MPI_Aint array_of_displaysments[1];
  MPI_Aint address1, address2;
  MPI_Get_address(&the_root.real, &address1);
  MPI_Get_address(&the_root.img, &address2);
  array_of_displaysments[0] = address2 - address1;
  MPI_Datatype mpi_root;

  MPI_Type_create_struct(1, array_of_blocklengths, array_of_displaysments,
                         array_of_types, &mpi_root);

  // Need to commit structure type in MPI
  MPI_Type_commit(&mpi_root);
  return mpi_root;
}
