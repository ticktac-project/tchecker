/*
 * This file is part of the TChecker Project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 */

#include "memcheck-unittest.hh"
#include <cstdlib>
#include <sstream>
#include <iostream>

int main(int argc, char ** argv)
{
  std::ostringstream oss;

  oss << VALGRIND_PROGRAM << " " << VALGRIND_OPTIONS << " " << TESTED_EXECUTABLE;
  if (argc > 1) {
    for (int i = 1; i < argc; i++)
      oss << " \"" << argv[i] << "\"";
  }
  int val = system(oss.str().c_str());
  return (val ? 1 : 0);
}
