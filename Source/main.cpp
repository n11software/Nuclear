#include <iostream>
#include <bits/stdc++.h>
#include "Nuclear.hpp"

int main(int argc, char **argv) {
  clock_t start, end;
  start = clock();
  Arguments args = Arguments(argc, argv);
  if (args.getInput() == "" || args.getOutput() == "") return 0;
  Nuclear nuclear = Nuclear(&args);
  end = clock();
  double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
  std::cout << "Took " << std::fixed << time_taken << std::setprecision(5) << "s to compile!" << std::endl;
  return 0;
}