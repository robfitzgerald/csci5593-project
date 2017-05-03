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
mpic++ src/topologyTests.cpp -o topologyTests
mpic++ src/topologyTests-baseline.cpp -o topoBaseline
mpic++ src/monteCarloPi.cpp -o mcpi
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
srun --mpi=pmi2 -n48 -w "node[3,4]" /home/<your.login>/<path>/<to>/csci5593-project/mcpi myPiTest 1000000 -b
srun --mpi=pmi2 -n12 -w "node[3,4,5]" /home/<your.login>/<path>/<to>/csci5593-project/topologyTests ring 1000 1
srun --mpi=pmi2 -n12 -w "node[3,4,5]" /home/<your.login>/<path>/<to>/csci5593-project/topoBaseline myTopoTest 1000 1
```

### Program Parameters

Topology Test takes several command line parameters.

1. Test Name: The first parameter can be a predefinded tests, any other string will default to a baseline test. The following strings will run a predefinded test

```
	ring
	star
	complete
	traffic
```

- note: if you want to explore increasing the "messages" parameter, run topoBaseline.

2. Iterations: The second parameter should be an integer. This will set the number of times the test will run in the execution. 

3. Message Volume: The third parameter should be an integer. This will set the number of messages that are passed from each node in each iteration. 

4. Center process: The fourth parameter is optional, and is only relevant in the star test. This parameter should be an integer and will represent which process will act as the center of the star topology. If this parameter is left blank the star test will iteratively test every process as the center process. 
