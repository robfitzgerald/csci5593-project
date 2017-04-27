package edu.colorado.cse.csci5593.team8

import java.io.PrintWriter

object MonteCarloScriptGenerator extends App {
  if (args.size < 2) {
    println("usage: <absolute/path/to/ExecutableBinary> <destFile>")
  } else {
    val binPath: String = args(0)
    val printWriter: PrintWriter = new PrintWriter(args(1))
    val iterations: Integer = if (args.size > 2) args(2).toInt else 1000
    
    (2 to 17).toSet[Int].subsets.foreach(subset => {
        // setup interpolated values
        val nodeList: String = subset.mkString(",")
        val testName: String = s"${subset.mkString("-")}"
        // generate shell script lines for this pair
        val script: String = s"""srun --mpi=pmi2 -n2 -w "node[$nodeList]" $binPath $testName $iterations -b"""
        printWriter.println(script)
      })

    printWriter.close ();  
  }
}
