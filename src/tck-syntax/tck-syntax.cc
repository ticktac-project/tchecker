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

#include "syntax-check.hh"
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
                                       {"transform", no_argument, 0, 't'},
                                       {"help", no_argument, 0, 'h'},
                                       {0, 0, 0, 0}};

static char * const options = (char *)"cd:hn:o:pt";

void usage(char * progname)
{
  std::cerr << "Usage: " << progname << " [options] [file]" << std::endl;
  std::cerr << "   -c          syntax check (timed automaton)" << std::endl;
  std::cerr << "   -p          synchronized product" << std::endl;
  std::cerr << "   -t          transform a system into dot graphviz file format" << std::endl;
  std::cerr << "   -o file     output file" << std::endl;
  std::cerr << "   -d delim    delimiter string (default: _)" << std::endl;
  std::cerr << "   -n name     name of synchronized process (default: P)" << std::endl;
  std::cerr << "   -h          help" << std::endl;
  std::cerr << "reads from standard input if file is not provided" << std::endl;
}

static bool check_syntax = false;
static bool synchronized_product = false;
static bool transform = false;
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
    case 't':
      transform = true;
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
 \post error and warnings have been reported to std::cerr
*/
void do_syntax_check_ta(tchecker::parsing::system_declaration_t const & sysdecl)
{
  if (tchecker::tck_syntax::syntax_check_ta(std::cerr, sysdecl))
    std::cout << "Syntax OK" << std::endl;
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
  tchecker::system::output_tck(os, product);
  os << std::endl;
}

/*!
 \brief Output a system of processes following the dot graphviz format
 \param sysdecl : system declaration
 \param delimiter : delimiter used in node names
 \param os : output stream
 \post The system of processes in sysdecl has been output to os following the
 dot graphviz format, using delimiter as a separator between process name and
 location name for node names.
*/
void do_output_dot(tchecker::parsing::system_declaration_t const & sysdecl, std::string const & delimiter, std::ostream & os)
{
  std::shared_ptr<tchecker::system::system_t> system(new tchecker::system::system_t(sysdecl));
  tchecker::system::output_dot(os, *system, delimiter);
  os << std::endl;
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

    if (synchronized_product && transform) {
      std::cerr << "Command line options -p and -t are incompatible" << std::endl;
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
      do_syntax_check_ta(*sysdecl);

    if (synchronized_product)
      do_synchronized_product(*sysdecl, process_name, delimiter, *os);

    if (transform)
      do_output_dot(*sysdecl, delimiter, *os);
  }
  catch (std::exception & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
