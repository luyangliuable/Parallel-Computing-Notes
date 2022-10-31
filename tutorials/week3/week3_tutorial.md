# Questions
<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [Questions](#questions)
    - [1. Discuss the advantages and disadvantages of Threads.](#1-discuss-the-advantages-and-disadvantages-of-threads)
        - [Answers:](#answers)
    - [Question 2](#question-2)
    - [Consequently, what would be the outcome if the code above was compiled and executed? Briefly discuss the concept of lost and spurious signals when using condition variables for POSIX threads.](#consequently-what-would-be-the-outcome-if-the-code-above-was-compiled-and-executed-briefly-discuss-the-concept-of-lost-and-spurious-signals-when-using-condition-variables-for-posix-threads)
    - [If the sleep function in Thread1Func is to remain, modify Thread2Func to ensure that pthread_cond_signal is not lost (Note: Adding another sleep function in Thread2Func is not the appropriate solution here)](#if-the-sleep-function-in-thread1func-is-to-remain-modify-thread2func-to-ensure-that-pthread_cond_signal-is-not-lost-note-adding-another-sleep-function-in-thread2func-is-not-the-appropriate-solution-here)

<!-- markdown-toc end -->

## 1. Discuss the advantages and disadvantages of Threads.
Advantages:
1. Lightweight process. Context switching between threads is faster than switching between processes causing lower overhead. They are cheaper to create in resources. Producer/consumer problem is easier to solve by coordinating threads.
2. Shared memory meaning that threads does not need to pass data between each other and they can access a common memory.
3. Fork-join model meaning that threads can be easily created in a process and joined later.
4. Allows parallel programming.  Threads can be scheduled concurrently making software more efficient and flexible
5. Scheduling and load balancing.
6. Easy and widespread


### Answers:
7. Threads allow utilization of multiprocessor architectures to a greater scale and efficiency for task and data parallelism

8. Reduced number of required servers: Because one server can dispatch multiple service threads, the number of servers to start for your application is reduced. This capability for multiple dispatched threads is especially useful for conversational servers, which otherwise must be dedicated to one client for the entire duration of a conversation. E.g. threaded approach for client server ipc communication

9. Software portability

10. Latency hiding

11. Easier to code remote procedure calls on different threads than on the same thread.

Disadvantages:
1. Difficult to write code. Only programmers experienced can code these types of applications.
2. Susceptible to deadlocks, race conditions and lost updates due to shared memory variables and data.
3. Difficult to debug. Crashes on diff. processes are harder to detect because there is no lower bound on the amount of time for execution of task on each process.
4. Difficult to maintain concurrency.
5. Difficulty of testing
6. Difficulty of porting existing code.
* Remove static variables
* Replace any function calls that are not thread-safe
* Replace any other code that is not thread-safe
* Because the completed port must be tested and re-tested, the work required to port a multithreaded and/or multi-context application is substantial


## Question 2

Possible values
* Only outputs what each threads returns but does not print message1 and message2
* Prints out what each threads returns as well as message1 and message2 which each thread.

* The cause is there is a missing pthread_join(pthread_t id, void * ret) at the end of main function.

* Without this function the main function with exit/return without waiting for the thread to finish. The threads are terminated as well.
* The outcome depends on if the threads prints first or main returns first


(c) Analyze the code in parts (a) and (b) above. Both the code in (a) and (b) should compile and produce a correct output. Notice that for both code snippets, the threads are writing into global variables. However, the code snippet in (a) used a mutex but the code snippet in (b) did not use a mutex. Why is this the case?

* Mutex is required when there is a potential race condition on the same varible involved during parallel processing.
* Mutex allows only one process to have exclusive accessing on a variable.
* In part a, the global sum variable is added and updated by all processes so it does a mutex lock
* In part b, the global array index/pigeonhole is unique for each index so race condition does not exist, so mutex lock is not required.

## Consequently, what would be the outcome if the code above was compiled and executed? Briefly discuss the concept of lost and spurious signals when using condition variables for POSIX threads.

* Thread1Func gets blocked indefinitely because pthread_cond_wait(&g_Cond, &g_Mutex) does not receive a signal.

* When thread2Func finishes and signals the condition variable, thread 1 function is still blocked.

* The signal from thread2func is lost so thread1func gets blocked.

* The main thread also gets blocked at thread join for thread 1 hence the program will not end.

* This in turn would cause a program failure (i.e., the program will freeze
or hang)

## If the sleep function in Thread1Func is to remain, modify Thread2Func to ensure that pthread_cond_signal is not lost (Note: Adding another sleep function in Thread2Func is not the appropriate solution here)
