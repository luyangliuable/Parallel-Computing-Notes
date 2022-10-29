# Practice Exam

a) Explain the difference between the MPI Synchronous Send (i.e., MPI_Ssend()) and MPI Send (i.e., MPI_Send()) functions? What would be a possible benefit of using MPI_Ssend() instead of MPI_Send()?

MPI_Ssend() is a synchronous blocking send meaning that it will wait for the other process to reply.

MPI_Send() is will either a Ssend or Bsend depending on whether the buffer attached for buffered sends contains enough space for the message to send.

b) There isn't a specific synchronous MPI function for receive (e.g., MPI_Srecv()). Instead, we are required to use MPI_Recv() together with MPI_Ssend() for synchronous-based blocking communication. Why is this so?

There is no synchronous blocking receive because receiving a message cannot be asynchronous and it is immediate.

c) The streaming SIMD Extensions (SSE) represent an expanded set of SIMD instructions with 32-bit floating point support. These instructions are divided into four functional groups. List these groups.

1. Packed and scalar single-precision floating-point instructions
2. 64-bit SIMD integer instructions
3. State management instructions
3. Cacheability control, prefetch, and memory ordering instructions

d)
Large registers 256 bit registers
operation on 32 8-bit values instructions.

e)
ThreadIdx.x - The thread id index inside the block
BlockIdx.x - The block id index inside the grid
blockDim.x - The number of blocks in each grid.


## Question 2
a) Describe the three different types of scheduling clauses available in OpenMP, and when to use these scheduling clauses
* Dynamic - Use when you want the thread to take on any free tasks when it is done
* Static - Use when you want a round robin distribution of scheduling.
* Guide - When you want the chunk size to be different for each chunk with each successive chunk similar to the last.

b) OpenMP parallel sections enables independent sections of code to execute concurrently. What if there are more/less threads than tasks? How are tasks assigned to threads?

* If there are less threads than tasks openmp will run the tasks with all the avalaible threads and based on the schelduing clause when a the threads are avalaible they will take the on the next tasks.

* If there are more threads, often each thread will take on a task.


```
#include <omp.h>
#include <stdio.h>
#include <string.h>

int checkPN(int inNumber) {
  char strNumber[50];
  int len, i, rev;
  if (inNumber < 10) {
    return 1;
  } else {
    sprintf(strNumber, "%d", inNumber);
    len = strlen(strNumber);
    rev = len - 1;
    for (i = 0; i < len; i++) {
      if (strNumber[i] != strNumber[rev])
        return 0;
      rev--;
    }
    return 1;
  }
}

int main() {
  omp_set_num_threads(4);
  int i, upLimit, threadID;
  char fileName[32];
  FILE *pFile;
  printf("Enter the upper limit (> 0) of the search for palindromic numbers: ");
  scanf("%d", &upLimit);
  pFile = fopen("File_Palindromic.txt", "w");

#pragma omp parallel
  {
  int num_threads = omp_get_num_threads();
  int thread_no = omp_get_thread_num();
#pragma omp barrier

int test = -1;
#pragma omp for schedule(static) private(i)
    for (i = 0; i <= upLimit; i++) {
      /*
       * Shared Variables:
       * upLimit
       * thread_no
       * pFile
       *
       * Private Variables:
       * i
       */
      if (checkPN(i)) {
        fprintf(pFile, "Thread %i: %d\n", thread_no, i);
      }
    }
    fclose(pFile);
  }
  return 0;
}

```


## Question 3

(a)
The g_RobotEvtArr array is used to store the condition variables for each robot thread. This array is used in the ControlModuleFunc() and RobotFunc() functions. Why does each robot thread require a separate condition variable? Why can't both robot threads share a single condition variable?

* Each event signal array need their own thread/robot id because if you signal using only one variable all threads will run not knowing which robot's choice got updated for the control module.
* Have an array of unique condition variable add uniqueness to the signal from each thread so which robot's choice can be distinguished.

(b)
* Each Mutex signifies that which thread is entering a critical region
* The Mutex must not be shared amongst different threads. Otherwise, the threading entering the critical region to change a shared value would not be able to be distinguished.

(c)
* g_CMMutex stores the choices inputted by the user. The main process locks this value to indicate it is being changed by a thread.
* A single variable g_CMEvt is used to signal the start of ControlModuleFunc because the control module accepts only a choice and no shared values are changed.
* From here and until actually updating the on/off status of the robots, there is no global value that's change and g_CMEvt is only used to signal the control module to update the on/off status based on the choices.
* Hence an array of conditions and mutexs is not required because only main thread is updating the choices and used to signal control based to update the on/off status of the robot based on the choice.

(d) In the RobotFunc() function, what is the purpose of the pthread_cond_wait() function? Why does this function require both the condition variable and mutex variable to passed in as arguments (by reference)? Explain the importance of pairing the condition variable with a mutex.

* Condition variables allows blocked threads to be notified when a specific condition occurs.
* When a thread waits using pthread_cond_wait(pthread_cond_t &cond, pthread_mutex_t &mutex) it is blocked and automatically release the mutex.
* When the thread is woken up it automatically acquire the mutex lock.
* In this case when the thread for ControlModuleFunc() is woken up it will acquire the mutex lock so that the main function can't continue to make new choices.
* The pthread_cond_wait() function waits for a condition to be signaled before running the next lines of code in the thread. It will be triggered when pthread_cond_signal() is call with the same conditional variable.
* The pthread_cond_wait() function needs the mutex so that it can lock the condition variable

e) In the ControlModuleFunc() function, why doesn't the pthread_cond_signal() function accept a mutex as an argument? In the same ControlModuleFunc() function, What would happen to the simulator if the pthread_mutex_unlock() function is removed?
* In the control module, if the thread is woken up on thread_cond_wait(pthread_cond_t &cond, pthread_mutex_t &mutex), the thread will automatically acqure the mutex not allowing other process to acquire it.
* So ControlModuleFunc does not have to manually lock the process and it does not need the mutex as an argument,.
* If pthread_mutex_unlock() is removed then the main process will be blocked from updating user choice.


(f) If the code listing below is to be rewritten using C with only message passing interface (MPI), specify (with reasoning) the type MPI functions which could be used to substitute the POSIX functions.

* pthread_mutex_lock(&g_CMMutex) can be removed
* pthread_mutex_unlock(&g_CMMutex) and pthread_cond_signal(&g_CMEvt) can be replaced with MPI_Send(&choice, 1, MPI_INT, process, 0, MPI_COM_WORLD) to siginal another process to start
<!-- * Immediate after place a MPI_Recv() to block until a response is received from the ControlModuleFunc() process. -->
* pthread_cond_wait(&g_CMEvt, &g_CMMutex) can be replaced with MPI_Recv(&choice, 1, MPI_INT, root, 0, MPI_COMM_WORLD) to block until receive the signal and perform changes to the on/off states of the robots.
* The main function will not be able to update the choices until ControlModuleFunc() is done because the next round of MPI_Send() is blocked until .


