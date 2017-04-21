// #include <mpi.h>
// #include <ctime>
#include <sys/time.h>
#include <string>
#include <cstdlib>
#include <iostream>

struct timeval start, end;
struct TestConfig
{
  std::string testName; 
  unsigned nodes, processes, iterations, messages;  
};

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

bool parseArgs(int, char**, TestConfig&);
bool runTest(TestConfig);
void log(LogData);
void storeTime(timeval&);
float timeDelta(timeval, timeval);


int main (int argc, char** argv)
{
  TestConfig conf;
  
  if (!parseArgs(argc, argv, conf))
    std::cout << "usage: <testName> <# nodes> <# processes> <# iterations> <# messages>\n";
  else 
    if (!runTest(conf)) 
      return EXIT_FAILURE;
    else
      return EXIT_SUCCESS;
}

/**
 * this is where we will inject our test
 * @param  conf : configuration called from command line
 * @return true on success
 */
bool runTest(TestConfig conf)
{
  bool somethingBadHappened = false;
  if (somethingBadHappened)
    return false;

  // time tracking example
  storeTime(start);
  
  // remove this
  std::cout << "runTest called with TestConfig:\n" 
            << "testName: " << conf.testName << ", " 
            << "nodes: " << conf.nodes << ", "
            << "processes: " << conf.processes << ", "
            << "iterations: " << conf.iterations << ", "
            << "messages: " << conf.messages << ".\n";

  //
  // code goes here
  //
  //
  
  storeTime(end);
  float seconds = timeDelta(start, end);

  // logging example
  log(LogData(conf.testName,"a node id",0,1,seconds,"demonstration"));

  return true;
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

void log(LogData l)
{
  std::cout << l.testName << "," 
            << l.thisNode << "," 
            << l.thisID << ","
            << l.thatID << ","
            << l.timeDelta << ","
            << l.message <<"\n";
}


bool parseArgs(int argc, char** argv, TestConfig& result)
{
  if (argc < 6)
  {
    return false;
  }
  else
  {
    result.testName = argv[1];
    result.nodes = atoi(argv[2]);
    result.processes = atoi(argv[3]);
    result.iterations = atoi(argv[4]);
    result.messages = atoi(argv[5]);
    return true;
  }
}