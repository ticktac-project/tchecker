/*
 * This file is part of the TChecker Project.
 * 
 * See files AUTHORS and LICENSE for copyright details.
 */

#include <cstdlib>
#include <sstream>
#include "memcheck-unittest.hh"

int main(int argc, char **argv)
{
  std::ostringstream oss;

  oss << VALGRIND_PROGRAM << " " << VALGRIND_OPTIONS << " "
      << TESTED_EXECUTABLE;
  for(int i = 1; i < argc; i++)
    oss << " " << argv[i];

  return system(oss.str().c_str ());
}