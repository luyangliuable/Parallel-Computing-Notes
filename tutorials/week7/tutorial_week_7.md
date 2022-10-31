# Questions

## 1. Discuss the difference between logical clock and real clock synchronization.
* The ordering of events is can be known without time. Doing so would require a logical clock

* A real lock orders messages and events based on the time stamp lower than a time limit. Logic orders events based on a vector ordering.

## 2. Outline when and why you might need to use a centralised or distributed MUTEX algorithm.
* You need a centralised or distributed mutex algorithm when you multiple process wants to access a shared resource to prevent race condition, lost updates and inconsistent retrievals.

## 3. Discuss Averaging algorithm
* “Averaging algorithm” is a decentralized algorithm. 
* This algorithm divides time into resynchronization intervals with a fixed length R. 
  * All processes broadcast their time with all other processes. All process takes an average of the time and makes it their new time.
* If the previous time of the process is later than the new process, the process will halt or slow down its clock to adjust.


# 4. Discuss the relationship between the Chandy-Misra-Haas algorithm and the wait for graph in the context of deadlock identification.
* A wait for graph indicates a deadlock if there is a cycle.
* Chandy-Misra-Haas algorithm sends probe messages to identify a deadlock to see if there is a cycle.
* A cycle is detected in Chandy-Misra-Haas algorithm if the message returns to where it started.
