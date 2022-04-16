/*
 * main.cpp - Start here!
 *
 *  This file will attempt to read the arguments
 *  passed into the binary and parse them and
 *  send the parsed versions to the lexer.
 *
 * It is recommended to read Nuclear.cpp before reading this.
 */

#include <iostream>
#include <bits/stdc++.h>
#include "Nuclear.hpp"

int main(int argc, char **argv) {
  // Now that you've gone to Nuclear.cpp you should understand the rest of this...
  // I hope
  Arguments args = Arguments(argc, argv);
  clock_t start, end;
  if (args.getIsShowingTime()) start = clock();
  if (args.getInput() == "" || args.getOutput() == "") {
    fprintf(stderr, "Error: No input or output file specified.\n");
    return 1;
  }
  Nuclear nuclear = Nuclear(&args);
  if (args.getIsShowingTime()) {
    end = clock();
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    std::cout << "Took " << std::fixed << time_taken << std::setprecision(5) << "s to compile!" << std::endl;
  }
  if (args.getRunAfter()) {
    if (args.getOutputAssembly()) {
      fprintf(stderr, "Error: Cannot run assembly file directly!\n");
      exit(1);
    }
    system(("./"+args.getOutput()).c_str());
  }
  return 0;
}
