# CS632 Assignment 2 (Shivansh Rai)

# Instructions
To compile the source file, execute `make`.

To run wrt an input file `sample_input` -
```
cat sample_input | mpirun -n <num_procs> -stdin all ./parallel
```
Here `num_procs` is the number of processes (> 2).

# Design
Let the total number of processes to be `k`.
Out of these processes, one process is chosen as master and the rest `k-1`
processes are chosen as slaves.
The responsibility of a slave is to compute the convoluted value corresponding
to a specific matrix entry.
The responsibility of master is to visit all the slaves in round-robin manner
and collect their computed results.

Since the processes aren't running in a shared address space, the master process
is used to save all the individually retrieved results in its own address space,
which is finally printed to stdout.

The scheduling of master and slaves is done in a round-robin manner.

    master -> slave_1 -> master -> slave_2 -> ...

## Advantages
* The load gets equally distributed amongst all the processes.
* For machines with more than one core, the results will be computed parallely,
  thus resulting in lesser execution time than the sequential counterpart.

## Disadvantages
* If the number of processes is sufficiently large, the amount of time taken by 
  master process to revisit a slave can exceed the time required for a slave to 
  finish its computation. This will result in an increase in idle time.
* If the machine is single-core, the overall execution time will exceed that of
  the sequential counterpart due to the context-switch overhead.

## Pseudocode

```
# A master can only collect result from a slave when it is revisited. As a
# result one last iteration has to be made over all the slaves to get their
# computed result (and hence the extra k-1 in variable below).
num_iterations = m*n + k-1
slave_rank = 1
result = NULL

for index in 1...num_iterations:
    slave_rank = (slave_rank) % k + 1

    if world_rank == slave_rank:
        if index >= k:
            # The slave is revisited
            send result to master

        if index lies in {1, m*n}:
            result = compute(index)

    if world_rank == 0 && index >= k:
        # the slave was revisited, and hence sent its computation
        master receives result from slave (rank = slave_rank)

```
