# Basic-Concurrent-Programming
## Problems Solved
* **Diner Philosopher Problem** - This problem states that, there are 'n' philosophers who think and eat for each of
the 'm' number of days. Provided there are 'n' forks to eat, each philosopher must have two forks in order to be able
to eat. All the philosophers are thinking and eating concurrently and they also make sure that they do not end up in
a deadlock situation (where each philosopher grabs single fork, hence no one can eat).
* **Reader/Writer Problem** - This problem states that, given a global buffer (which is initially empty), a writer
fills the empty slots in the buffer with meaningful data, and the reader reads the filled slots in the buffer. However
as the global buffer is accessible to both the reader and the writer at the same time, it is to be made sure that,
a reader may never read a slot ahead of time before the writer fills it with meaningful data and writer must not
overwrite the slot in the buffer before that slot is read by a reader. (This problem discusses only 1 reader and 1
writer).
* **Rendezvous Semaphore** - This is not a problem but an illustration of two threads which in order to progess
further, communicate with each other (using semaphores). This problem discusses a client requesting a server to add
two number which are initialized by the client.
