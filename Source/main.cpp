#include "iostream"
#include "Nuclear.hpp"

int main(int argc, char **argv) {
  Arguments args = Arguments(argc, argv);
  if (args.getInput() == "" || args.getOutput() == "") return 0;
  return 0;
}