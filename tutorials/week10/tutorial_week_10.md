# Tutorial Week 10

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [Tutorial Week 10](#tutorial-week-10)
    - [How is MPI Scatter different from MPI Broadcast? In addition, how is MPI Gather different from MPI Reduce?](#how-is-mpi-scatter-different-from-mpi-broadcast-in-addition-how-is-mpi-gather-different-from-mpi-reduce)
    - [Explain the concept of MPI virtual topology and its benefits](#explain-the-concept-of-mpi-virtual-topology-and-its-benefits)
    - [Question 4](#question-4)
        - [A.](#a)
        - [B. Advantages of each option](#b-advantages-of-each-option)
        - [C. Why should the MPI_Cart_create() function be invoked by all of the MPI processes simulating the sensor nodes? What happens if any one of the MPI processes simulating the sensor nodes does not invoke the MPI_Cart_create() function?](#c-why-should-the-mpi_cart_create-function-be-invoked-by-all-of-the-mpi-processes-simulating-the-sensor-nodes-what-happens-if-any-one-of-the-mpi-processes-simulating-the-sensor-nodes-does-not-invoke-the-mpi_cart_create-function)

<!-- markdown-toc end -->

## How is MPI Scatter different from MPI Broadcast? In addition, how is MPI Gather different from MPI Reduce?

* MPI scatter scatters a buffer of element and evenly distribute the elements to each process of a certain size. Not all processes need to call the same scatter line.

* MPI Broadcast shares a variable with all processes that calls the same line.

* MPI Reduce performs a certain operatation on all processes' elements and save to a buffer. While MPI gather only gathers the array element from all process into a buffer.


## Explain the concept of MPI virtual topology and its benefits
* Easier to get neighboring nodes and work with specific communication patterns
* Improve communication efficiency by communicating with nodes that are nearby. Minimize hardware penalties.
* A particular implementation may optimize process mapping based upon the physical characteristics of a given parallel machine.


## Question 4
### A.
* Option B represents the master-slave architecture (centralized) as every node compares sends their reading to the server for analysis. This incurs a lot of communication overhead as every node have to periodically send a message to the server.

* Option A represents the decentralized architecture as every node compares their reading with their neighbors and only send one or less alert when there is a fire instead all the nodes sending the alert. This is more efficient.

### B. Advantages of each option
* Option B: Each node does not have to worry about comparing reading and detect alert. Easier to implement.

* Option A: Less likely to bottleneck at the server because less computation. Less number of communications and communication overhead between nodes so it is more efficient.

### C. Why should the MPI_Cart_create() function be invoked by all of the MPI processes simulating the sensor nodes? What happens if any one of the MPI processes simulating the sensor nodes does not invoke the MPI_Cart_create() function?

Every sensor in the Cartesian grid must invoke it in order to get their rank number in the 2d communicator as well as the coordinates by calling MPI_Cart_rank() and MPI_Cart_coord(). Without calling MPI_Cart_create() will not allow the sensor to know their position in the grid and will not create a 2d communicator.


### D. When passing in the first argument into the MPI_Cart_create() function, why doesn't this function use the default MPI_COMM_WORLD communicator?

* When the communicated is split into two colors, the first color contains the processes belonging to the grid topology while the second is the master process. Passing in MPI_COMM_WORLD would cause all the processes to be part of the cartesian topology so comm which is the communicator containing processes other than the master should be used.


### E.
k(depth) = x / (row_width * column_width)
i(row) = x % row_width
j(column) = ( x / row_width )


### F.
rank = k*(row_width*column_width) + i + j*row_width;

### g.
    
