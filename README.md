# csci5593-project

### about

This is the project repository for **Team 8**'s project, _Measuring Heracles_. 

#### Team 8 is:

- Rob Fitzgerald
- Matthew Kalan
- Nathan Justice

The project provides testing applications for reporting the time for passing MPI messages across an organized compute cluster. In our case, we attempted to profile the CSE Heracles Xeon cluster and Infiniband network. This cluster has 16 compute nodes, each with 24 cores. We wrote our programs in C++ using the Open MPI library.

### install

1. connect to Heracles via ssh:

```bash
$ ssh <your.login>@heracles.ucdenver.pvt
<your.login>@heracles.ucdenver.pvt's password: 
Last login: ...
[your.login@heracles ~]$ 
```

2. download our code

```bash
$ git clone https://github.com/robfitzgerald/csci5593-project.git
Cloning into 'csci5593-project'...
remote: Counting objects: 251, done.
remote: Compressing objects: 100% (69/69), done.
remote: Total 251 (delta 32), reused 0 (delta 0), pack-reused 182
Receiving objects: 100% (251/251), 1.03 MiB | 0 bytes/s, done.
Resolving deltas: 100% (102/102), done.
$ cd csci5593-project
$ ls
build.sh README.md  src  test
```

3. compile a test application

```
$ chmod 777 build.sh
$ ./build.sh
$ 
```

4. run 

You will be using Slurm to run this MPI program across multiple nodes. Slurm's `srun` has the following usage:

```
srun --mpi=pmi2 -n<procs> -w "node[<node list>]" <program>

where
  <procs>      number of requested processes
  <node list>  id of node in cluster to use
               comma-delimited list is acceptable (i.e. [3,4])
  <program>    absolute path to compiled program, and the program's arguments
```

here are examples for both applications:

```
srun --mpi=pmi2 -n48 -w "node[3,4]" /home/<your.login>/<path>/<to>/csci5593-project/monteCarloPi testName 1000000 -b
srun --mpi=pmi2 -n12 -w "node[3,4,5]" /home/<your.login>/<path>/<to>/csci5593-project/topologyTests testName 1000000 1
```
