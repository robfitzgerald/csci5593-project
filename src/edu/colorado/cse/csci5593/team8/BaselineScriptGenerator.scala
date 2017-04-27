package edu.colorado.cse.csci5593.team8

import java.io.PrintWriter

object BaselineScriptGenerator extends App {
  if (args.size < 2) {
    println("usage: <absolute/path/to/ExecutableBinary> <destFile>")
  } else {
    val binPath: String = args(0)
    val printWriter: PrintWriter = new PrintWriter(args(1))
    val messages: Integer = if (args.size > 2) args(2).toInt else 1000
    
    (2 to 17).combinations(2).foreach(pair => {
        // setup interpolated values
        val fwdNodeList: String = pair.mkString(",")
        val fwdTestName: String = s"${pair(0)}-${pair(1)}"
        val revNodeList: String = pair.reverse.mkString(",")
        val revTestName: String = s"${pair(1)}-${pair(0)}"
        // generate shell script lines for this pair
        val fwdScript: String = s"""srun --mpi=pmi2 -n2 -w "node[$fwdNodeList]" $binPath $fwdTestName 1 $messages"""
        val revScript: String = s"""srun --mpi=pmi2 -n2 -w "node[$revNodeList]" $binPath $revTestName 1 $messages"""
        printWriter.println(fwdScript)
        printWriter.println(revScript)
      })

    printWriter.close ();  
  }
}
