# Questions

## 1.
This case study content contains the description of the failure, which is software related. However, human error is also contributed to this failure due to improper testing prior to launch.


## 2. On July 4, 1997, the Mars pathfinder mission landed flawlessly on the Martial surface. However, later its communication failed due to a design flaw in the real-time embedded software kernel VxWorks. The problem was later diagnosed to be caused due to priority inversion. Explain the concept of priority inversion and the type of faulty which would be associated with this condition. Suggest a solution to address this fault.

* Example:
* There is a High priority task HPT, low priority task LPT and medium priority task MPT.
* The LPT locks resource T.
* The HPT needs resource T.
* MPT runs and takes up all the CPU, LPT gets no CPU.
* Hence Priority of HPT < Priority MPT, priority inversion occurs.

* Semaphores likely used and lacks the ability to elevate the priority of the lower task currently holding the semaphore.

* The type of error leading to this is software design error.
* Solution would be to use mutex instead of semaphores because mutex elevates the low priority task to high priority.

## 4. Explain the behavior of the following source code. Specifically, describe the behavior
of the workload distribution, MPI send and receive functions under non-uniformed
data distribution conditions, MPI reduce and the parallel computation section of the
code.
Note: Use the provided line numbers when describing the code:


* Line 24: The workload is distributed evenly amongst the processes. The last process gets the remainder of all the tasks on top of the workload distribution.
* Line 26-27: allocate memory equal to the workload distribution for receive buffer and perfect square.
* Line 31 - 35: Generate a random value between 0 and 1501 for finding perfect square for a total of buffer elements in heap array.
* Line 53 - Line 59: Each process then continues to calculate the perfect square (including root). The chunkDistSize is fixed so only the size of each chunk does not have to be sent.

* Line 88: The work processes first sends in the count which is the number of perfect squares found for each process.
* Line 89: Then it sends the actual buffer containing the perfect squares back to the root.
* Line 71: Root receives the number of perfects squares as count.
* Line 72: Root receives the perfects squares from message buffer where offset is the current position containing no values.
* Line 62: Get the total number of perfect squares from after every slave process calculated them.
* Line 66: Root process allocates memory to store perfect squares.
* Line 70-76: Root process receives the perfect square count and perfect squares from slave processes
* Line 78-81: Root process save the perfect squares into perfer_square_numbers file
* Line 82: Close the file pointer
* Line 83-84: Free the store buffer and send buffer
* Line 94 - 95: Free buffers
* Line 96 - 97: finalist MPI and return
