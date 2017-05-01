// compile: mpic++ <filename>
// run: srun --mpi=pmi2 -n<number of processes> -w "node[3,4]" <absolute-path-to-file>

// srun --mpi=pmi2 -n6 -w "node[3,4]" /home/robert.fitzgerald/csc5593/project/csci5593-project/src/a.out myTest 100 500

#include <mpi.h>
#include <sys/time.h>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string.h>
#include <sstream>
#include <sys/time.h>

struct timeval start, end;

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
  int center;  
};

bool parseArgs(int, char**, TestConfig&);
bool ring(TestConfig, int, int, std::string);
bool traffic(TestConfig, int, int, std::string);
bool complete(TestConfig, int, int, std::string);
bool star(TestConfig, int, int, int, std::string);
bool averagetime(TestConfig, int, int, int, std::string);
void logger(LogData);
void storeTime(timeval&);
float timeDelta(timeval, timeval);
std::string itoa(int);


/**
 * this is where we will inject our test
 * @param  conf : configuration called from command line
 * @return true on success
 */
bool runTest(TestConfig conf, int proc, int numProcs, std::string nodeName){

  if(proc == 0){
     printf("%s,%s,%s,%s,%s\n","thisNode","thisID","thatID","timeDelta","message");
  }  

  storeTime(start);
  // just demonstrating the config values we received
  if(strcmp(conf.testName.c_str(), "ring") == 0){
    for(int i = 0; i < conf.iterations; ++i ){
      ring(conf,proc,numProcs,nodeName);
    }
  } else if(strcmp(conf.testName.c_str(), "traffic") == 0){
    for(int i = 0; i < conf.iterations; ++i ){
      traffic(conf,proc,numProcs,nodeName);
    }
  } else if(strcmp(conf.testName.c_str(), "complete") == 0){
    for(int i = 0; i < conf.iterations; ++i ){
      complete(conf,proc,numProcs,nodeName);
    }
  } else if(strcmp(conf.testName.c_str(), "star") == 0 && conf.center == -1){
    for(int i = 0; i < conf.iterations; ++i ){
      for(int j = 0; j < numProcs; ++j){
        star(conf,j,proc,numProcs,nodeName);
      }
    }
  } else if(strcmp(conf.testName.c_str(), "star") == 0 && conf.center > -1){
    for(int i = 0; i < conf.iterations; ++i ){
        star(conf,conf.center,proc,numProcs,nodeName);
    }
  } else { //if(strcmp(conf.testName.c_str(), "average_time") == 0}){
    for(int i = 0; i < conf.iterations; ++i ){
      for(int j = 0; j < numProcs; ++j){
        averagetime(conf,j,proc,numProcs,nodeName);
      }
    }
  } 
  // function returns true on success
  return true;
}

bool averagetime(TestConfig conf, int center, int proc, int numProcs, std::string nodeName)
{
  int token;
  storeTime(end);
  float delta;
  delta = timeDelta(start, end);
  logger(LogData("initialize",nodeName,proc,-1,delta,"initialize"));
  //Receive all messages if you are not center
  if (proc != center) {
    for(int i = 0; i < conf.messages; ++i ){
      MPI_Recv(&token, 1, MPI_INT, center, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      storeTime(end);
      delta = timeDelta(start, end);
      logger(LogData(conf.testName,nodeName,proc,center,delta,"message received " + itoa(i)));
      //Send a response
      storeTime(start);
      MPI_Send(&token, 1, MPI_INT, center % numProcs, 0, MPI_COMM_WORLD);
      storeTime(end);
      delta = timeDelta(start, end);
      logger(LogData(conf.testName,nodeName,proc,center,delta,"message sent " + itoa(i)));
    }
  } else {
  // Set the token's value if you are center
    token = 0;
    for(int j = 0; j < numProcs; ++j){
      if(j != proc){
        for(int i = 0; i < conf.messages; ++i ){
          //Center sends message
          storeTime(start);
          MPI_Send(&token, 1, MPI_INT, j % numProcs, 0, MPI_COMM_WORLD);
          storeTime(end);
          delta = timeDelta(start, end);
          logger(LogData(conf.testName,nodeName,proc,j,delta,"message sent " + itoa(i)));
          //Receive responses
          MPI_Recv(&token, 1, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          storeTime(end);
          delta = timeDelta(start, end);
          logger(LogData(conf.testName,nodeName,proc,j,delta,"message received " + itoa(i)));
      }
    }
    }
  }
  //receive all messages after your turn to send
  return true;
}

bool star(TestConfig conf, int center, int proc, int numProcs, std::string nodeName)
{
  struct timeval token;
  float delta;
  storeTime(token); //This is dumb but it prevents negative times
  delta = timeDelta(start, token);
  logger(LogData("initialize",nodeName,proc,-1,delta,"initialize"));
  //Receive all messages if you are not center
  if (proc != center) {
    for(int i = 0; i < conf.messages; ++i ){
      MPI_Recv(&token, 1, MPI_INT, center, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      storeTime(end);
      delta = timeDelta(token, end);
      logger(LogData(conf.testName,nodeName,proc,center,delta,"message received " + itoa(i)));
      //Send a response
      storeTime(token);
      MPI_Send(&token, 1, MPI_INT, center % numProcs, 0, MPI_COMM_WORLD);
      storeTime(end);
      delta = timeDelta(start, end);
      logger(LogData(conf.testName,nodeName,proc,center,delta,"message sent " + itoa(i)));
    }
  } else {
  // Set the token's value if you are center
      for(int j = 0; j < numProcs; ++j){
        if(j != proc){
          for(int i = 0; i < conf.messages; ++i ){
            //Center sends message
            storeTime(token);
            MPI_Send(&token, 1, MPI_INT, j % numProcs, 0, MPI_COMM_WORLD);
            storeTime(end);
            delta = timeDelta(start, end);
            logger(LogData(conf.testName,nodeName,proc,j,delta,"message sent " + itoa(i)));
            //Receive responses
            MPI_Recv(&token, 1, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            storeTime(end);
            delta = timeDelta(token, end);
            logger(LogData(conf.testName,nodeName,proc,j,delta,"message received " + itoa(i)));
        }
      }
    }
  }
  //receive all messages after your turn to send
  return true;
}

bool complete(TestConfig conf, int proc, int numProcs, std::string nodeName)
{
  struct timeval token;
  float delta;
  storeTime(token); //This is dumb but it prevents negative times
  delta = timeDelta(start, token);
  logger(LogData("initialize",nodeName,proc,-1,delta,"initialize"));
  //Receive all messages before your turn to send
  if (proc != 0) {
    for(int j = 0; j < proc; ++j){
      for(int i = 0; i < conf.messages; ++i ){
        MPI_Recv(&token, 1, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        storeTime(end);
        delta = timeDelta(token, end);
        logger(LogData(conf.testName,nodeName,proc,j,delta,"message received " + itoa(i)));
      }
    }
  }

  //Send all of your messages
  for(int j = 0; j < numProcs; ++j){
    if(j != proc){
      for(int i = 0; i < conf.messages; ++i ){
        storeTime(token);
        MPI_Send(&token, 1, MPI_INT, j % numProcs, 0, MPI_COMM_WORLD);
        storeTime(end);
        delta = timeDelta(start, end);
        logger(LogData(conf.testName,nodeName,proc,j,delta,"message sent " + itoa(i)));
      } 
    }
  }

  //receive all messages after your turn to send
  for(int j = proc + 1; j < numProcs; ++j){
    for(int i = 0; i < conf.messages; ++i ){
      MPI_Recv(&token, 1, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      storeTime(end);
      delta = timeDelta(token, end);
      logger(LogData(conf.testName,nodeName,proc,j,delta,"message received " + itoa(i)));
    }
  }
  return true;
}

bool traffic(TestConfig conf, int proc, int numProcs, std::string nodeName)
{
  int token;
  float delta;  
  bool separate = false;
  storeTime(end); //This is dumb but it prevents negative times
  delta = timeDelta(start, end);
  logger(LogData("initialize",nodeName,proc,-1,delta,"initialize"));
  //Set up processes to recieve
  if (proc != 0 && proc != numProcs - 1) {
    for(int i = 0; i < conf.messages; ++i ){
      for(int j = 0; j < numProcs; ++j){
        if((j != 0 && j != numProcs - 1) || separate ){
          token = proc;
          MPI_Send(&token, 1, MPI_INT, j, 0, MPI_COMM_WORLD);
        }
      }
    } 
    for(int i = 0; i < conf.messages; ++i ){
       for(int j = 0; j < numProcs; ++j){
        if((j != 0 && j != numProcs - 1) || separate ){
          MPI_Recv(&token, 1, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
      }
    }
  } else if (proc == 0) {
    // Now process 0 can receive from the last process.
    token = 0;
    for(int i = 0; i < conf.messages; ++i ){
      storeTime(start);
      MPI_Send(&token, 1, MPI_INT, (numProcs -1), 0, MPI_COMM_WORLD);
      storeTime(end);
      delta = timeDelta(start, end);
      logger(LogData(conf.testName,nodeName,proc,(numProcs -1),delta,"message sent " + itoa(i)));
      MPI_Recv(&token, 1, MPI_INT, (numProcs -1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      storeTime(end);
      delta = timeDelta(start, end);
      logger(LogData(conf.testName,nodeName,proc,(numProcs -1),delta,"message received " + itoa(i)));
    }
  } else if (proc == (numProcs - 1)) {
    // Now process n -1 can receive from the last process.
    token = 0;
    storeTime(start);
    for(int i = 0; i < conf.messages; ++i ){
      MPI_Recv(&token, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      storeTime(end);
      delta = timeDelta(start, end);
      logger(LogData(conf.testName,nodeName,proc,0,delta,"message received " + itoa(i)));
      storeTime(start);
      MPI_Send(&token, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
      storeTime(end);
      delta = timeDelta(start, end);
      logger(LogData(conf.testName,nodeName,proc,0,delta,"message sent " + itoa(i)));
    }
  }
  return true;
}

bool ring(TestConfig conf, int proc, int numProcs, std::string nodeName)
{
  struct timeval token;
  float delta;  
  storeTime(token); //This is dumb but it prevents negative times
  delta = timeDelta(start, token);
  logger(LogData("initialize",nodeName,proc,-1,delta,"initialize"));
  //Set up processes to recieve
  if (proc != 0) {
    for(int i = 0; i < conf.messages; ++i ){
      MPI_Recv(&token, 1, MPI_INT, proc - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      storeTime(end);
      delta = timeDelta(token, end);
      logger(LogData(conf.testName,nodeName,proc,(proc - 1),delta,"message received " + itoa(i)));
    }
  }

  // Send all messages
  for(int i = 0; i < conf.messages; ++i ){
    storeTime(token);
    MPI_Send(&token, 1, MPI_INT, (proc + 1) % numProcs, 0, MPI_COMM_WORLD);
    storeTime(end);
    delta = timeDelta(start, end);
    logger(LogData(conf.testName,nodeName,proc,(proc + 1) % numProcs,delta,"message sent " + itoa(i)));
  } 

  // Now process 0 can receive from the last process.
  if (proc == 0) {
    for(int i = 0; i < conf.messages; ++i ){
      MPI_Recv(&token, 1, MPI_INT, numProcs - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      storeTime(end);
      delta = timeDelta(token, end);
      logger(LogData(conf.testName,nodeName,proc,numProcs - 1,delta,"message received " + itoa(i)));
    }
  }
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

  if (!parseArgs(argc, argv, conf)) {
    std::cout << "usage: <testName (ring/complete/star/average_time)> <# iterations> <# messages> <(opt) star center process>\n";
  }
  else {
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
  printf("%s,%s,%i,%i,%f,%s\n", l.testName.c_str(),l.thisNode.c_str(),l.thisID,l.thatID,l.timeDelta,l.message.c_str());
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
    if(argc > 4){
      result.center = atoi(argv[4]);
    } else {
      result.center = -1;
    }
    return true;
  }
}

std::string itoa(int val){
  std::ostringstream ss;
  ss << val;
  return ss.str();
}