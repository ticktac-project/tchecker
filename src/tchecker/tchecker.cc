/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cstring>
#include <getopt.h>
#include <string>
#include <unordered_map>

#include "tchecker/algorithms/covreach/options.hh"
#include "tchecker/algorithms/covreach/run.hh"
#include "tchecker/algorithms/explore/options.hh"
#include "tchecker/algorithms/explore/run.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/utils/log.hh"

/*!
 \file tchecker.cc
 \brief TChecker command-line tool
 */


/*!
 \brief Type of command
 */
enum command_t {
  COMMAND_UNKNOWN,
  COMMAND_EXPLORE,
  COMMAND_COVREACH,
};


/*!
 \brief Type of map of algorithm command-line options
 */
using command_line_options_map_t = std::unordered_map<std::string, std::string>;


/*!
 \brief Parse the command
 \param command : a command
 \return algorithm type corresponding to command, ALGO_UNKNOWN if command does
 not correspond to any implemented algorithm
 */
enum command_t parse_command(char * command)
{
  if (strcmp(command, "explore") == 0)
    return COMMAND_EXPLORE;
  if (strcmp(command, "covreach") == 0)
    return COMMAND_COVREACH;
  return COMMAND_UNKNOWN;
}


/*!
 \brief Parse command line options
 \param argc : number of command-line options
 \param argv : command-line options
 \param options : string of command-line options
 \param long_options : array of long names for command-line options
 \param log : logging facility
 \return A tuple <map, index> where map have entries (option, value) for every option with
 corresponding value from argv, and parsed according to options and long_options.
 value is the empty string if option has no associated value.
 index is the position of the first non-flag option in argv
 \pre options and long_options should follow getopt_long() requirements.
 Moreover, entries in long_options should either have format {long_name, arg, 0, short_name}
 or {long_name, arg, 0, 0} (no corresponding short name). Every short option (in options) must
 have a corresponding long option name in long_options
 argv[] is assumed to start with flags (of the form -s or --long), and then all non-flag
 options (input file names, etc)
 \post all errors and warnings have been reported to log
 */
std::tuple<command_line_options_map_t, int>
parse_options(int argc, char * argv[], char const * options, struct option const * long_options, tchecker::log_t & log)
{
  command_line_options_map_t map;
  int option = 0, option_index = 0;
  
  while (1) {
    option = getopt_long(argc, argv, options, long_options, &option_index);
    
    if (option == -1)
      break;        // All flags have been parsed
    
    if (option == ':')
      log.warning("Missing option parameter");
    else if (option == '?')
      log.warning("Unknown option");
    else if (option != 0) {
      char opt[] = {static_cast<char>(option), 0};
      map[opt] = (optarg == nullptr ? "" : optarg);
    }
    else
      map[long_options[option_index].name] = (optarg == nullptr ? "" : optarg);
  }
  
  return std::make_tuple(map, optind);
}


/*!
 \brief Print usage information
 \param exec_name : name of executable file
 \post Usage information has been output to std::cerr
 */
void usage(std::string const & exec_name)
{
  std::cerr << "Usage: " << exec_name << " command [options] [file]" << std::endl;
  std::cerr << "    with command:" << std::endl;
  std::cerr << "        covreach      run covering reachability algorithm" << std::endl;
  std::cerr << "        explore       run explore algorithm" << std::endl;
  std::cerr << "    options are command-specific (use -h to get help on the command)" << std::endl;
  std::cerr << "    reads from standard input if no file name is provided" << std::endl;
}


/*!
 \brief Command-line interface to TChecker
 */
int main(int argc, char * argv[])
{
  tchecker::log_t log(&std::cerr);
  
  // no command: list all available commands
  if (argc < 2) {
    usage(argv[0]);
    return 0;
  }
  
  // parse command
  enum command_t command = parse_command(argv[1]);
  
  // parse options
  command_line_options_map_t map;
  int index;
  switch (command) {
    case COMMAND_EXPLORE:
    {
      std::tie(map, index) = parse_options(argc-1, argv+1, tchecker::explore::options_t::getopt_long_options,
                                           tchecker::explore::options_t::getopt_long_options_long, log);
      ++index;    // accounts for argv+1 above
    }
      break;
    case COMMAND_COVREACH:
    {
      std::tie(map, index) = parse_options(argc-1, argv+1, tchecker::covreach::options_t::getopt_long_options,
                                           tchecker::covreach::options_t::getopt_long_options_long, log);
      ++index;    // accounts for argv+1 above
    }
      break;
    default:
      log.error("Unknown command: " + std::string(argv[1]));
      usage(argv[0]);
      return -1;
  }
  
  // describe command and exit if -h
  if (map.find("h") != map.end()) {
    switch (command) {
      case COMMAND_EXPLORE:
        tchecker::explore::options_t::describe(std::cerr);
        break;
      case COMMAND_COVREACH:
        tchecker::covreach::options_t::describe(std::cerr);
        break;
      default:
        break;
    }
    return -1;
  }
  
  // set input file ("" or "-" for std::cin, see system parser)
  std::string filename("");
  
  if (index < argc) {
    if (index != argc - 1) {
      log.error("more than 1 input file provided");
      usage(argv[0]);
      return -1;
    }
    
    filename = argv[index];
  }
  
  // run the command
  tchecker::parsing::system_declaration_t const * sysdecl = nullptr;
  
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(filename, log);
    if (sysdecl == nullptr)
      throw std::runtime_error("nullptr system declaration");
    
    switch (command) {
      case COMMAND_EXPLORE:
      {
        tchecker::explore::options_t options(tchecker::make_range(map.begin(), map.end()), log);
        tchecker::explore::run(*sysdecl, options, log);
      }
        break;
      case COMMAND_COVREACH:
      {
        tchecker::covreach::options_t options(tchecker::make_range(map.begin(), map.end()), log);
        tchecker::covreach::run(*sysdecl, options, log);
      }
        break;
      default:
        break;
    }
  }
  catch (std::exception const & e) {
    log.error(e.what());
  }
  
  log.display_counts();
  delete sysdecl;
  
  return EXIT_SUCCESS;
}
