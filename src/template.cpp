// compile: mpic++ <filename>
// run: srun --mpi=pmi2 -n<number of processes> -w "node[3,4]" <absolute-path-to-file>

// srun --mpi=pmi2 -n6 -w "node[3,4]" /home/robert.fitzgerald/csc5593/project/csci5593-project/src/a.out myTest 100 500

#include <mpi.h>
#include <sys/time.h>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <list>

struct timeval start, end;
const int MAX_STRING_LENGTH = 50;

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
  unsigned iterations, messages;  
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
bool runTest(TestConfig conf, int proc, int numProcs, std::string nodeName, std::list<LogData>& logger)
{

  // time tracking example
  storeTime(start);
  
  // just demonstrating the config values we received
  std::cout << "runTest called with TestConfig:\n" 
            << "testName: " << conf.testName << ", " 
            << "iterations: " << conf.iterations << ", "
            << "messages: " << conf.messages << ".\n"
            << "for process " << proc << " of " << numProcs
            << " on node " << nodeName << "\n";

  //
  //
  // code goes here
  //
  //
  
  storeTime(end);
  float seconds = timeDelta(start, end);

  // logging example
  logger.push_back(LogData(conf.testName,nodeName,proc,98765,seconds,"demonstration"));

  // function returns true on success
  return true;
}

bool handleLogs(int proc, int numProcs, std::list<LogData>& logger)
{
  if (proc != 0)
  {
    // child node.  send logs
    //   std::string testName, thisNode, message;
    //   unsigned thisID, thatID;
    //   double timeDelta;
    int numLogs = logger.size();
    char name[MAX_STRING_LENGTH];
    strcpy(name, logger.begin()->testName.c_str());
    char node[MAX_STRING_LENGTH];
    strcpy(node, logger.begin()->thisNode.c_str());
    unsigned me = logger.begin()->thisID;
    unsigned you = logger.begin()->thatID;
    double time [numLogs];
    int i = 0;
    for (std::list<LogData>::iterator iter = logger.begin(); iter != logger.end(); ++iter)
    {
      time[i] = iter->timeDelta;
      ++i;
    }
    char messages[numLogs][MAX_STRING_LENGTH];
    i = 0;
    for (std::list<LogData>::iterator iter = logger.begin(); iter != logger.end(); ++iter)
    {
      strcpy(messages[i], iter->message.c_str());
      ++i;
    }

    printf("process %i send numLogs\n",proc);
    MPI_Send(&numLogs, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    printf("process %i send name\n",proc);
    MPI_Send(&name, MAX_STRING_LENGTH, MPI_CHAR, 0, 2, MPI_COMM_WORLD);
    printf("process %i send node\n",proc);
    MPI_Send(&node, MAX_STRING_LENGTH, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
    printf("process %i send me\n",proc);
    MPI_Send(&me, 1, MPI_UNSIGNED, 0, 4, MPI_COMM_WORLD);
    printf("process %i send you\n",proc);
    MPI_Send(&you, 1, MPI_UNSIGNED, 0, 5, MPI_COMM_WORLD);
    printf("process %i send time\n",proc);
    MPI_Send(&time, numLogs, MPI_DOUBLE, 0, 6, MPI_COMM_WORLD);
    printf("process %i send messages\n",proc);
    MPI_Send(&messages, numLogs * MAX_STRING_LENGTH, MPI_CHAR, 0, 7, MPI_COMM_WORLD);
    printf("process %i done sending\n",proc);
  } else {
    // master node.  receive logs
    for (int p = 1; p < numProcs; ++p)
    {
      int numLogs;
      char name[MAX_STRING_LENGTH];
      char node[MAX_STRING_LENGTH];
      unsigned me;
      unsigned you;
      double time [numLogs];
      char messages[numLogs][MAX_STRING_LENGTH];
      // printf("master, about to receive, has %i logs",logger.size());
      // printf("master, from process %i, receive numLogs",p);
      MPI_Recv(&numLogs, 1, MPI_INT, p, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      // printf("master, from process %i, receive name",p);
      MPI_Recv(&name, MAX_STRING_LENGTH, MPI_CHAR, p, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      // printf("master, from process %i, receive node",p);
      MPI_Recv(&node, MAX_STRING_LENGTH, MPI_CHAR, p, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      // printf("master, from process %i, receive me",p);
      MPI_Recv(&me, 1, MPI_UNSIGNED, p, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      // printf("master, from process %i, receive you",p);
      MPI_Recv(&you, 1, MPI_UNSIGNED, p, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      // printf("master, from process %i, receive time",p);
      MPI_Recv(&time, numLogs, MPI_DOUBLE, p, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      // printf("master, from process %i, receive messages",p);
      MPI_Recv(&messages, numLogs * MAX_STRING_LENGTH, MPI_CHAR, p, 7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      // printf("master, from process %i, done receiving",p);
      for (int i = 0; i < numLogs; ++i)
      {
        logger.push_back(LogData(name, node, me, you, time[i], messages[i]));
      }      
      // printf("master, done receiving, has %i logs",logger.size());
    }
  }

  return true;
}

int main (int argc, char** argv)
{
  TestConfig conf;
  
  std::list<LogData> log;

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
    if (!runTest(conf, world_rank, world_size, nodeName, log)) {
      MPI_Finalize();
      return EXIT_FAILURE;
    }
    else
    {
      handleLogs(world_rank, world_size, log);
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
  printf("%s,%s,%i,%i,%f,%s\n",l.testName.c_str(),l.thisNode.c_str(),l.thisID,l.thatID,l.timeDelta,l.message.c_str());
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
    result.messages = atoi(argv[3]);
    return true;
  }
}