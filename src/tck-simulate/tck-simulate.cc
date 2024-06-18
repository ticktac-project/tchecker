/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/config.hh"
#include <cstring>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#if USE_BOOST_JSON
#include <boost/json.hpp>
#endif
#include "simulate.hh"

/*!
 \file tck-simulate.cc
 \brief Command-line simulator for TChecker timed automata models
 */

static struct option long_options[] = {{"interactive", no_argument, 0, 'i'},
                                       {"random", required_argument, 0, 'r'},
                                       {"onestep", no_argument, 0, '1'},
                                       {"output", required_argument, 0, 'o'},
                                       {"trace", no_argument, 0, 't'},
                                       {"help", no_argument, 0, 'h'},
#if USE_BOOST_JSON
                                       {"state", required_argument, 0, 's'},
                                       {"json", no_argument, 0, 0},
#endif
                                       {0, 0, 0, 0}};

static char * const options = (char *)"1ir:ho:s:t";

/*!
\brief Print usage message for program progname
*/
void usage(char * progname)
{
  std::cerr << "Usage: " << progname << " [options] [file]" << std::endl;
  std::cerr << "   -1          one-step simulation (output initial or next states if combined with -s)" << std::endl;
  std::cerr << "   -i          interactive simulation (default)" << std::endl;
  std::cerr << "   -r N        randomized simulation, N steps" << std::endl;
  std::cerr << "   -o file     output file for simulation trace (default: stdout)" << std::endl;
#if USE_BOOST_JSON
  std::cerr << "   --json      display states/transitions in JSON format" << std::endl;
  std::cerr << "   -s state    starting state, specified as a JSON object with keys vloc, intval and zone" << std::endl;
  std::cerr << "               vloc: comma-separated list of location names (one per process), in-between < and >" << std::endl;
  std::cerr << "               intval: comma-separated list of assignments (one per integer variable)" << std::endl;
  std::cerr << "               zone: conjunction of clock-constraints (following TChecker expression syntax)" << std::endl;
#endif
  std::cerr << "   -t          output simulation trace, incompatible with -1" << std::endl;
  std::cerr << "   -h          help" << std::endl;
  std::cerr << "reads from standard input if file is not provided" << std::endl;
}

/*!
 \brief Type of simulation
*/
enum simulation_type_t {
  INTERACTIVE_SIMULATION, /*!< Interactive simulation */
  ONESTEP_SIMULATION,     /*!< One-step simulation */
  RANDOMIZED_SIMULATION,  /*!< Randomized simulation */
};

static enum simulation_type_t simulation_type = INTERACTIVE_SIMULATION;
static enum tchecker::tck_simulate::display_type_t display_type = tchecker::tck_simulate::HUMAN_READABLE_DISPLAY;
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
      case '1':
        simulation_type = ONESTEP_SIMULATION;
        break;
      case 'i':
        simulation_type = INTERACTIVE_SIMULATION;
        break;
      case 'r': {
        simulation_type = RANDOMIZED_SIMULATION;
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
#if USE_BOOST_JSON
      if (strcmp(long_options[long_option_index].name, "json") == 0)
        display_type = tchecker::tck_simulate::JSON_DISPLAY;
      else
#endif
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
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(filename);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
  }

  if (sysdecl == nullptr)
    tchecker::log_output_count(std::cout);

  return sysdecl;
}

#if USE_BOOST_JSON
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
#endif

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

    if (output_trace && (simulation_type == ONESTEP_SIMULATION)) {
      std::cerr << "Cannot output trace in one-step simulation" << std::endl;
      return EXIT_FAILURE;
    }

    std::string input_file = (optindex == argc ? "" : argv[optindex]);
    if (input_file == "" || input_file == "-")
      std::cerr << "Reading model from standard input" << std::endl;
    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{load_system(input_file)};
    if (sysdecl == nullptr)
      return EXIT_FAILURE;

    if (output_filename != "")
      os = new std::ofstream(output_filename, std::ios::out);

    std::map<std::string, std::string> starting_state_attributes;
#if USE_BOOST_JSON
    if (starting_state_json != "")
      starting_state_attributes = parse_state_json(starting_state_json);
#endif
    std::shared_ptr<tchecker::tck_simulate::graph_t> g{nullptr};
    if (simulation_type == INTERACTIVE_SIMULATION)
      g = tchecker::tck_simulate::interactive_simulation(*sysdecl, display_type, starting_state_attributes);
    else if (simulation_type == RANDOMIZED_SIMULATION)
      g = tchecker::tck_simulate::randomized_simulation(*sysdecl, nsteps, starting_state_attributes);
    else if (simulation_type == ONESTEP_SIMULATION)
      tchecker::tck_simulate::onestep_simulation(*sysdecl, display_type, starting_state_attributes);
    else
      throw std::runtime_error("Select one of interactive, one-step or randomized simulation");

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
