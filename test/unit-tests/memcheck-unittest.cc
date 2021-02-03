/*
 * This file is part of the TChecker Project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 */

#include "memcheck-unittest.hh"
#include <cstdlib>
#include <sstream>

int main(int argc, char ** argv)
{
  std::ostringstream oss;

  oss << VALGRIND_PROGRAM << " " << VALGRIND_OPTIONS << " " << TESTED_EXECUTABLE;
  for (int i = 1; i < argc; i++)
    oss << " " << argv[i];

  return system(oss.str().c_str());
}