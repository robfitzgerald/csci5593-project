# csci5593-project

run command: `srun --mpi=pmi2 -n<procs> -w "node[<node list>]" <program>`

where

```
  <procs>      number of requested processes
  <node list>  id of node in cluster to use
               comma-delimited list is acceptable (i.e. [3,4])
  <program>    absolute path to compiled program
```

### agenda

1. template @robfitzgerald

  a. argv parameters

    * number of nodes
    * number of processes
    * number of messages to pass at a time
    * number of iterations of a test

2. tests

  a. baseline

    *send/recv
      
      for all combinations of nodes in heracles
        
      test 1000 or so messages between the combination to generate a baseline message cost between all nodes
    
    *broadcast

    *scatter/gather

  b. general topology experiments @mkalan

    * ring

    * star

    * complete graph

  c. broadcast

    * crank up payload size

  d. scatter/gather

    * crank up workload/payload size

3. rdma

  * run tests on messaging in rdma

4. analysis

  * excel? python?