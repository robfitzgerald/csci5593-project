// #include <mpi.h>
#include <ctime>
#include <string>
#include <cstdlib>
#include <iostream>


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


bool runTest(TestConfig conf)
{
  bool somethingBadHappened = false;
  if (somethingBadHappened)
    return false;

  time_t startTime, endTime;
  time(&startTime);
  
  std::cout << "runTest called with TestConfig:\n" 
            << "testName: " << conf.testName << ", " 
            << "nodes: " << conf.nodes << ", "
            << "processes: " << conf.processes << ", "
            << "iterations: " << conf.iterations << ", "
            << "messages: " << conf.messages << ".\n";
  
  time(&endTime);
  double seconds = difftime(startTime, endTime);

  log(LogData(conf.testName,"a node id",0,1,seconds,"demonstration"));

  return true;
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