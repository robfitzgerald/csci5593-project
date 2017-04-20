# csci5593-project

run command: `srun --mpi=pmi2 -n<procs> -w "node[<node list>]" <program>`

where

```
  <procs>      number of requested processes
  <node list>  id of node in cluster to use
               comma-delimited list is acceptable (i.e. [3,4])
  <program>    absolute path to compiled program
```