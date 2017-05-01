// compile: scalac src/edu/colorado/cse/csci5593/team8/MonteCarloScriptGenerator.scala 
// run: scala edu.colorado.cse.csci5593.team8.MonteCarloScriptGenerator
// usage: <absolute/path/to/ExecutableBinary> <destFile> <lowNode> <highNode> <procs> [<iterations>]
// example: scala edu.colorado.cse.csci5593.team8.MonteCarloScriptGenerator /path/to/bin mcpi.sh 2 4 8

package edu.colorado.cse.csci5593.team8

import java.io.PrintWriter

object MonteCarloScriptGenerator extends App {
  val procSteps: Integer = 4

  if (args.size < 6) {
    println("usage: <absolute/path/to/ExecutableBinary> <destFile> <lowNode> <highNode> <lowProcs> <highProcs> [<iterations>]")
  } else {
    val completeNodeSet: Range = (2 to 17)
    val binPath: String = args(0)
    val printWriter: PrintWriter = new PrintWriter(args(1))
    val (lowNode, highNode): (Int, Int) = (args(2).toInt, args(3).toInt)
    val (lowProcs, highProcs): (Int, Int) = (args(4).toInt, args(5).toInt)
    val iterations: Int = if (args.size > 6) args(6).toInt else 1000
    

    (completeNodeSet).map(n=>(2 to n)).foreach(rangeOfNodes => {
        // setup interpolated values
        val procs: Integer = rangeOfNodes.size * lowProcs
        val nodeList: String = rangeOfNodes.mkString(",")
        val testName: String = s"${rangeOfNodes.mkString("-")}"
        // generate shell script lines for this pair
        val script: String = s"""srun --mpi=pmi2 -n$procs -w "node[$nodeList]" $binPath $testName $iterations -b"""
        printWriter.println(script)
      })

    printWriter.println("")

    (lowProcs to highProcs by ((highProcs - lowProcs) / procSteps)).foreach(procs => {
        // setup interpolated values
        val nodeList: String = (lowNode to highNode).mkString(",")
        val testName: String = s"${nodeList.size}n-${procs}p"
        // generate shell script lines for this pair
        val script: String = s"""srun --mpi=pmi2 -n$procs -w "node[$nodeList]" $binPath $testName $iterations -b"""
        printWriter.println(script)
      })

    printWriter.close ();  
  }
}
