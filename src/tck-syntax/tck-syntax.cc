/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cstring>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "tchecker/parsing/parsing.hh"
#include "tchecker/syncprod/system.hh"
#include "tchecker/system/output.hh"
#include "tchecker/system/system.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/utils/log.hh"

/*!
 \file tck-syntax.cc
 \brief Syntax checking and translation of systems
 */

static struct option long_options[] = {{"check", no_argument, 0, 'c'},
                                       {"product", no_argument, 0, 'p'},
                                       {"output", required_argument, 0, 'o'},
                                       {"delimiter", required_argument, 0, 'd'},
                                       {"process-name", required_argument, 0, 'n'},
                                       {"help", no_argument, 0, 'h'},
                                       {0, 0, 0, 0}};

static char * const options = (char *)"cd:hn:o:p";

void usage(char * progname)
{
  std::cerr << "Usage: " << progname << " [options] [file]" << std::endl;
  std::cerr << "   -c          syntax check (timed automaton)" << std::endl;
  std::cerr << "   -p          synchronized product" << std::endl;
  std::cerr << "   -o file     output file" << std::endl;
  std::cerr << "   -d delim    delimiter string (default: _)" << std::endl;
  std::cerr << "   -n name     name of synchronized process (default: P)" << std::endl;
  std::cerr << "   -h          help" << std::endl;
  std::cerr << "reads from standard input if file is not provided" << std::endl;
}

static bool check_syntax = false;
static bool synchronized_product = false;
static bool help = false;
static std::string delimiter = "_";
static std::string process_name = "P";
static std::string output_file = "";

int parse_command_line(int argc, char * argv[])
{
  while (true) {
    int c = getopt_long(argc, argv, options, long_options, nullptr);

    if (c == -1)
      break;

    if (c == ':')
      throw std::runtime_error("Missing option parameter");
    else if (c == '?')
      throw std::runtime_error("Unknown command-line option");

    switch (c) {
    case 'c':
      check_syntax = true;
      break;
    case 'd':
      delimiter = optarg;
      break;
    case 'h':
      help = true;
      break;
    case 'n':
      process_name = optarg;
      break;
    case 'o':
      if (strcmp(optarg, "") == 0)
        throw std::invalid_argument("Invalid empty output file name");
      output_file = optarg;
      break;
    case 'p':
      synchronized_product = true;
      break;
    default:
      throw std::runtime_error("I should never be executed");
      break;
    }
  }

  return optind;
}

/*!
 \brief Load system from a file
 \param filename : file name
 \return The system declaration loaded from filename, nullptr if parsing error occurred
*/
std::shared_ptr<tchecker::parsing::system_declaration_t> load_system(std::string const & filename)
{
  tchecker::parsing::system_declaration_t * sysdecl = nullptr;
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(filename);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << " " << e.what() << std::endl;
  }

  if (sysdecl == nullptr)
    tchecker::log_output_count(std::cout);

  return std::shared_ptr<tchecker::parsing::system_declaration_t>(sysdecl);
}

/*!
 \brief Check timed automaton syntax from a declaration
 \param sysdecl : system declaration
 \return true if sydecl contains a syntactically correct declaration of a
 system of timed automata, false otherwise
*/
bool do_check_syntax_ta(tchecker::parsing::system_declaration_t const & sysdecl)
{
  try {
    tchecker::ta::system_t system(sysdecl);
  }
  catch (std::exception & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
  }

  if (tchecker::log_error_count() > 0) {
    tchecker::log_output_count(std::cout);
    return false;
  }

  std::cout << "Syntax OK" << std::endl;
  return true;
}

/*!
 \brief Flatten a system of processes into a single process
 \param sysdecl : system declaration
 \param process_name : name of the resulting process
 \param delimiter : delimiter used in names of the resulting process
 \param os : output stream
 \post The synchronized product of the system in sysdecl has been output to os.
 The resulting process has name process_name. The names of locations and events
 are list of location/event names in the original process, seperated by
 delimiter
 */
void do_synchronized_product(tchecker::parsing::system_declaration_t const & sysdecl, std::string const & process_name,
                             std::string const & delimiter, std::ostream & os)
{
  std::shared_ptr<tchecker::syncprod::system_t> system(new tchecker::syncprod::system_t(sysdecl));
  tchecker::system::system_t product = tchecker::syncprod::synchronized_product(system, process_name, delimiter);
  os << product << std::endl;
}

/*!
 \brief Main function
*/
int main(int argc, char * argv[])
{
  try {
    int optindex = parse_command_line(argc, argv);

    if (argc - optindex > 1) {
      std::cerr << "Too many input files" << std::endl;
      usage(argv[0]);
      return EXIT_FAILURE;
    }

    if (help) {
      usage(argv[0]);
      return EXIT_SUCCESS;
    }

    std::string input_file = (optindex == argc ? "" : argv[optindex]);

    std::ostream * os = nullptr;
    if (output_file != "") {
      os = new std::ofstream(output_file, std::ios::out);
    }
    else
      os = &std::cout;

    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{load_system(input_file)};
    if (sysdecl.get() == nullptr)
      return EXIT_FAILURE;

    if (check_syntax)
      do_check_syntax_ta(*sysdecl);

    if (synchronized_product)
      do_synchronized_product(*sysdecl, process_name, delimiter, *os);
  }
  catch (std::exception & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
