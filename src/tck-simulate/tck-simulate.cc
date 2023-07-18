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

#include <boost/json.hpp>

#include "simulate.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/utils/log.hh"

/*!
 \file tck-simulate.cc
 \brief Command-line simulator for TChecker timed automata models
 */

static struct option long_options[] = {{"interactive", no_argument, 0, 'i'},  {"json", no_argument, 0, 0},
                                       {"random", required_argument, 0, 'r'}, {"output", required_argument, 0, 'o'},
                                       {"state", required_argument, 0, 's'},  {"trace", no_argument, 0, 't'},
                                       {"help", no_argument, 0, 'h'},         {0, 0, 0, 0}};

static char * const options = (char *)"ir:ho:s:t";

/*!
\brief Print usage message for program progname
*/
void usage(char * progname)
{
  std::cerr << "Usage: " << progname << " [options] [file]" << std::endl;
  std::cerr << "   -i          interactive simulation" << std::endl;
  std::cerr << "   --json      display states/transitions in JSON format" << std::endl;
  std::cerr << "   -r N        randomized simulation, N steps" << std::endl;
  std::cerr << "   -o file     output file for simulation trace" << std::endl;
  std::cerr << "   -s state    starting state, specified as a JSON object with keys vloc, intval and zone" << std::endl;
  std::cerr << "               vloc: comma-separated list of location names (one per process), in-between < and >" << std::endl;
  std::cerr << "               intval: comma-separated list of assignments (one per integer variable)" << std::endl;
  std::cerr << "               zone: conjunction of clock-constraints (following TChecker expression syntax)" << std::endl;
  std::cerr << "   -t          output simulation trace (default: stdout)" << std::endl;
  std::cerr << "   -h          help" << std::endl;
  std::cerr << "reads from standard input if file is not provided" << std::endl;
}

static bool interactive_simulation = false;
static enum tchecker::tck_simulate::display_type_t display_type = tchecker::tck_simulate::HUMAN_READABLE_DISPLAY;
static bool randomized_simulation = false;
static bool help = false;
static std::size_t nsteps = 0;
static std::string output_filename = "";
static std::string starting_state_json = "";
static bool output_trace = false;

/*!
\brief Parse command line arguments
\param argc : number of command line arguments
\param argv : array of command line arguments
\post Global variables have been set according to argv
*/
int parse_command_line(int argc, char * argv[])
{
  while (true) {
    int long_option_index = -1;
    int c = getopt_long(argc, argv, options, long_options, &long_option_index);

    if (c == -1)
      break;

    if (c == ':')
      throw std::runtime_error("Missing option parameter");
    else if (c == '?')
      throw std::runtime_error("Unknown command-line option");
    else if (c != 0) {
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
      case 's':
        starting_state_json = optarg;
        break;
      case 't':
        output_trace = true;
        break;
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
    else {
      if (strcmp(long_options[long_option_index].name, "json") == 0)
        display_type = tchecker::tck_simulate::JSON_DISPLAY;
      else
        throw std::runtime_error("This also should never be executed");
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
    std::cerr << tchecker::log_error << e.what() << std::endl;
  }

  if (sysdecl == nullptr)
    tchecker::log_output_count(std::cout);

  return std::shared_ptr<tchecker::parsing::system_declaration_t>(sysdecl);
}

/*!
 \brief Parse JSON description of state as a map of attributes
 \param state_json : JSON description of state
 \pre state_json is a JSON object with keys: vloc, intval and zone
 \note all other keys in state_json are ignored
 \return state_json vloc, intval and zone as a map of attributes
 \throw std::invalid_argument : if state_json cannot be parsed, or if the precondition is not
 satisfied
*/
std::map<std::string, std::string> parse_state_json(std::string const & state_json)
{
  boost::json::error_code ec;
  boost::json::value json_value = boost::json::parse(state_json, ec);
  if (ec)
    throw std::invalid_argument("Syntax error in JSON state description: " + state_json);

  if (json_value.kind() != boost::json::kind::object)
    throw std::invalid_argument("State description is not a JSON object: " + state_json);

  boost::json::object const & json_obj = json_value.get_object();

  auto value_as_string = [&](boost::json::object const & obj, std::string const & key) {
    boost::json::value const & v = obj.at(key);
    if (v.kind() != boost::json::kind::string)
      throw std::invalid_argument("Unexpected value for key " + key + ", expecting a string");
    return v.get_string();
  };

  std::map<std::string, std::string> attributes;
  attributes["vloc"] = value_as_string(json_obj, "vloc");
  attributes["intval"] = value_as_string(json_obj, "intval");
  attributes["zone"] = value_as_string(json_obj, "zone");
  return attributes;
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
    if (input_file == "" || input_file == "-")
      std::cerr << "Reading model from standard input" << std::endl;
    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{load_system(input_file)};
    if (sysdecl.get() == nullptr)
      return EXIT_FAILURE;

    if (output_filename != "")
      os = new std::ofstream(output_filename, std::ios::out);

    std::map<std::string, std::string> starting_state_attributes;
    if (starting_state_json != "")
      starting_state_attributes = parse_state_json(starting_state_json);

    std::shared_ptr<tchecker::tck_simulate::graph_t> g{nullptr};
    if (interactive_simulation)
      g = tchecker::tck_simulate::interactive_simulation(*sysdecl, display_type, starting_state_attributes);
    else if (randomized_simulation)
      g = tchecker::tck_simulate::randomized_simulation(*sysdecl, nsteps, starting_state_attributes);
    else
      throw std::runtime_error("Select one of interactive or randomized simulation");

    if (output_trace)
      tchecker::tck_simulate::dot_output(*os, *g, sysdecl->name());

    if (os != &std::cout)
      delete os;
  }
  catch (std::exception & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
    if (os != &std::cout)
      delete os;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
