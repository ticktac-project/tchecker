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

#include "simulate.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/utils/log.hh"

/*!
 \file tck-simulate.cc
 \brief Command-line simulator for TChecker timed automata models
 */

static struct option long_options[] = {{"interactive", no_argument, 0, 'i'},
                                       {"random", required_argument, 0, 'r'},
                                       {"output", required_argument, 0, 'o'},
                                       {"help", no_argument, 0, 'h'},
                                       {0, 0, 0, 0}};

static char * const options = (char *)"ir:ho:";

/*!
\brief Print usage message for program progname
*/
void usage(char * progname)
{
  std::cerr << "Usage: " << progname << " [options] [file]" << std::endl;
  std::cerr << "   -i          interactive simulation" << std::endl;
  std::cerr << "   -r N        randomized simulation, N steps" << std::endl;
  std::cerr << "   -o file     output file for simulation tarce" << std::endl;
  std::cerr << "   -h          help" << std::endl;
  std::cerr << "reads from standard input if file is not provided" << std::endl;
}

static bool interactive_simulation = false;
static bool randomized_simulation = false;
static bool help = false;
static std::size_t nsteps = 0;
static std::string output_filename = "";

/*!
\brief Parse command line arguments
\param argc : number of command line arguments
\param argv : array of command line arguments
\post Global variables have been set according to argv
*/
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
    case 'i':
      interactive_simulation = true;
      break;
    case 'r': {
      randomized_simulation = true;
      long long int l = std::strtoll(optarg, nullptr, 10);
      if (l < 0)
        throw std::runtime_error("Invalid trace length (must be positive)");
      nsteps = l;
      break;
    }
    case 'h':
      help = true;
      break;
    case 'o':
      if (strcmp(optarg, "") == 0)
        throw std::invalid_argument("Invalid empty output file name");
      output_filename = optarg;
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
 \brief Main function
*/
int main(int argc, char * argv[])
{
  std::ostream * os = &std::cout;

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

    if (randomized_simulation && interactive_simulation) {
      std::cerr << "Interactive and randomized simulations are mutually exclusive" << std::endl;
      return EXIT_FAILURE;
    }

    std::string input_file = (optindex == argc ? "" : argv[optindex]);
    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{load_system(input_file)};
    if (sysdecl.get() == nullptr)
      return EXIT_FAILURE;

    if (output_filename != "")
      os = new std::ofstream(output_filename, std::ios::out);

    std::shared_ptr<tchecker::tck_simulate::graph_t> g{nullptr};
    if (interactive_simulation)
      g = tchecker::tck_simulate::interactive_simulation(*sysdecl);
    else if (randomized_simulation)
      g = tchecker::tck_simulate::randomized_simulation(*sysdecl, nsteps);
    else
      throw std::runtime_error("Select one of interactive or randomized simulation");

    tchecker::tck_simulate::dot_output(*os, *g, sysdecl->name());

    if (os != &std::cout)
      delete os;
  }
  catch (std::exception & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
    if (os != &std::cout)
      delete os;
  }

  return EXIT_SUCCESS;
}
