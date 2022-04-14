#include <iostream>
#include "Nuclear.hpp"

int main(int argc, char **argv) {
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