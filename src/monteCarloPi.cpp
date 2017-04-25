// compile: mpic++ <filename>
// run: srun --mpi=pmi2 -n<number of processes> -w "node[3,4]" <absolute-path-to-file>

// srun --mpi=pmi2 -n6 -w "node[3,4]" /home/robert.fitzgerald/csc5593/project/csci5593-project/src/a.out myTest 100 -b

#include <mpi.h>
#include <sys/time.h>
#include <cmath>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>

struct timeval start, end;
const int DEFAULT_TAG = 0;

/**
 * object used to call the logging service
 */
struct LogData
{
  LogData (std::string name, std::string node, unsigned me, unsigned you, double t, std::string msg = ""):
    testName(name),
    thisNode(node),
    thisID(me),
    thatID(you),
    timeDelta(t),
    message(msg) {}
  std::string testName, thisNode, message;
  unsigned thisID, thatID;
  double timeDelta;
};

/**
 * object used to hold command line arguments
 */
struct TestConfig
{
  std::string testName; 
  unsigned iterations;
  bool bundled;  
};

bool parseArgs(int, char**, TestConfig&);
void logger(LogData);
void storeTime(timeval&);
float timeDelta(timeval, timeval);


/**
 * this is where we will inject our test
 * @param  conf : configuration called from command line
 * @return true on success
 */
bool runTest(TestConfig conf, int proc, int numProcs, std::string nodeName)
{
  srand(time(NULL));
  double x, y, dart;
  int i, j;
  float seconds;
  unsigned myCount = 0;  
  if (true /*conf.bundled*/) 
  {
    storeTime(start);
    for (i = 0; i < conf.iterations; ++i)
    {
      x = ((double) rand() / RAND_MAX);
      y = ((double) rand() / RAND_MAX);
      dart = x * x + y * y;
      if (dart < 1.0)
        ++myCount;
    }
    if (proc != 0)
    {
      MPI_Send(&myCount, 1, MPI_UNSIGNED, 0, DEFAULT_TAG, MPI_COMM_WORLD);
      storeTime(end);
      seconds = timeDelta(start, end);
      logger(LogData(conf.testName,nodeName,proc,0,seconds,"sending reduce step"));
    } else {        // -- MASTER
      storeTime(end);
      // not sure we want to log the master node work load w/o associated send/recv
      // seconds = timeDelta(start, end);
      // logger(LogData(conf.testName,nodeName,proc,0,seconds,"master process ending work"));
      unsigned theirCount;
      for (j = 1; j < numProcs; ++j)
      {
        storeTime(start);
        MPI_Recv(&theirCount, 1, MPI_UNSIGNED, j, DEFAULT_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        myCount += theirCount;
        storeTime(end);
        seconds = timeDelta(start, end);
        logger(LogData(conf.testName,nodeName,j,proc,seconds,"receiving reduce step"));
      }
      std::string prefix = "pi = ";
      double piValue = ((double) myCount / (numProcs + conf.iterations)) * 4;
      std::ostringstream pi;
      pi << prefix << piValue;
      printf("%s by way of (%i / (%i + %i)) * 4", pi.str(), myCount, numProcs, conf.iterations);
      logger(LogData(conf.testName,nodeName,0,0,0,pi.str()));
    }
    
  } else {

    // for (int i = 0; i < conf.iterations; ++i)
    // {
    //   storeTime(start);
    //   x = ((double) rand() / RAND_MAX);
    //   y = ((double) rand() / RAND_MAX);
    //   dart = x * x + y * y;
    //   if (dart < 1.0)
    //     ++myCount;
    //   MPI_Send();
    //   storeTime(end);
    //   seconds = timeDelta(start, end);
    //   logger(LogData(conf.testName,nodeName,proc,0,seconds,"returning single result"));
    //   myCount = 0;
    // }
  }

  // function returns true on success
  return true;
}






int main (int argc, char** argv)
{
  TestConfig conf;
  
  MPI_Init(NULL, NULL);

  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // Get the name of the processor
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  int name_len;
  MPI_Get_processor_name(processor_name, &name_len);
  std::string nodeName = processor_name;

  if (!parseArgs(argc, argv, conf))
    std::cout << "usage: <testName> <# iterations> <# messages>\n";
  else 
    if (!runTest(conf, world_rank, world_size, nodeName)) {
      MPI_Finalize();
      return EXIT_FAILURE;
    }
    else
    {
      MPI_Finalize();
      return EXIT_SUCCESS;
    }
}

void storeTime(timeval &t)
{
  gettimeofday( &t, NULL );
}

/**
 * taken from stats_s.c homework assignment
 */
float timeDelta(timeval start, timeval end)
{
  return ( ( end.tv_sec  - start.tv_sec ) * 1000.0 ) + ( ( end.tv_usec - start.tv_usec ) / 1000.0 );
}

void logger(LogData l)
{
  printf("%s,%s,%i,%i,%f,%s\n",l.testName,l.thisNode,l.thisID,l.thatID,l.timeDelta,l.message);
  // std::cout << l.testName << "," 
  //           << l.thisNode << "," 
  //           << l.thisID << ","
  //           << l.thatID << ","
  //           << l.timeDelta << ","
  //           << l.message <<"\n";
}


bool parseArgs(int argc, char** argv, TestConfig& result)
{
  if (argc < 4)
  {
    return false;
  }
  else
  {
    result.testName = argv[1];
    result.iterations = atoi(argv[2]);
    result.bundled = (argv[3] == std::string("-b") ? true : false);
    return true;
  }
}