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
#include <tuple>
#include <unordered_set>

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

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

static struct option long_options[] = {{"asynchronous-events", no_argument, 0, 0},
                                       {"check", no_argument, 0, 'c'},
                                       {"product", no_argument, 0, 'p'},
                                       {"output", required_argument, 0, 'o'},
                                       {"delimiter", required_argument, 0, 'd'},
                                       {"process-name", required_argument, 0, 'n'},
                                       {"transform", no_argument, 0, 't'},
                                       {"json", no_argument, 0, 'j'},
                                       {"help", no_argument, 0, 'h'},
                                       {0, 0, 0, 0}};

static char * const options = (char *)"cd:hn:o:ptj";

void usage(char * progname)
{
  std::cerr << "Usage: " << progname << " [options] [file]" << std::endl;
  std::cerr << "   --asynchronous-events  reports all asynchronous events in the model" << std::endl;
  std::cerr << "   -c                     syntax check (timed automaton)" << std::endl;
  std::cerr << "   -p                     synchronized product" << std::endl;
  std::cerr << "   -t                     transform a system into dot graphviz file format" << std::endl;
  std::cerr << "   -j                     transform a system into json file format" << std::endl;
  std::cerr << "   -o file                output file" << std::endl;
  std::cerr << "   -d delim               delimiter string (default: _)" << std::endl;
  std::cerr << "   -n name                name of synchronized process (default: P)" << std::endl;
  std::cerr << "   -h                     help" << std::endl;
  std::cerr << "reads from standard input if file is not provided" << std::endl;
}

static bool report_asynchronous_events = false;
static bool check_syntax = false;
static bool synchronized_product = false;
static bool transform = false;
static bool json = false;
static bool help = false;
static std::string delimiter = "_";
static std::string process_name = "P";
static std::string output_file = "";

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
      case 'j':
        json = true;
        break;
      default:
        throw std::runtime_error("This should never be executed");
        break;
      }
    }
    else {
      if (strcmp(long_options[long_option_index].name, "asynchronous-events") == 0)
        report_asynchronous_events = true;
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

/*!
 \brief Type of pair (process identifier, event identifier)
*/
using process_event_t = std::tuple<tchecker::process_id_t, tchecker::event_id_t>;

/*!
 \brief Hash functor on process events
*/
class hash_process_event_t {
public:
  /*!
   \brief Hash function
   \param process_event : process event
   \return hash code for process_event
  */
  std::size_t operator()(process_event_t const & process_event) const
  {
    std::size_t h = boost::hash_value(std::get<0>(process_event));
    boost::hash_combine(h, std::get<1>(process_event));
    return h;
  }
};

/*!
 \brief Report asynchronous events from a declaration
 \param sysdecl : system declaration
 \post all asynchronous events in sysdecl have been reported to std::cout
*/
void do_report_asynchronous_events(tchecker::parsing::system_declaration_t const & sysdecl)
{
  std::unordered_set<process_event_t, hash_process_event_t> reported_asynchronous_events;

  try {
    tchecker::syncprod::system_t system(sysdecl);

    std::cout << "Asynchronous events in model " << system.name() << std::endl;
    for (tchecker::system::edge_const_shared_ptr_t const & edge : system.edges())
      if (system.is_asynchronous(*edge)) {
        process_event_t process_event = std::make_tuple(edge->pid(), edge->event_id());
        if (reported_asynchronous_events.find(process_event) == reported_asynchronous_events.end()) {
          std::cout << "    event " << system.event_name(edge->event_id()) << " in process " << system.process_name(edge->pid())
                    << std::endl;
          reported_asynchronous_events.insert(process_event);
        }
      }
    std::cout << "Found " << reported_asynchronous_events.size() << " asynchronous event(s)" << std::endl;
  }
  catch (...) {
    std::cerr << tchecker::log_error << "Syntax error in TChecker file (run tck-syntax with option -c)" << std::endl;
  }
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
 \brief Output a system of processes following the JSON format
 \param sysdecl : system declaration
 \param delimiter : delimiter used in node names
 \param os : output stream
 \post The system of processes in sysdecl has been output to os following the
 JSON format, using delimiter as a separator between process name and
 location name for node names.
*/
void do_output_json(tchecker::parsing::system_declaration_t const & sysdecl, std::string const & delimiter, std::ostream & os)
{
  std::shared_ptr<tchecker::system::system_t> system(new tchecker::system::system_t(sysdecl));
  tchecker::system::output_json(os, *system, delimiter);
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

    if (report_asynchronous_events)
      do_report_asynchronous_events(*sysdecl);

    if (synchronized_product)
      do_synchronized_product(*sysdecl, process_name, delimiter, *os);

    if (transform)
      do_output_dot(*sysdecl, delimiter, *os);
    else if (json)
      do_output_json(*sysdecl, delimiter, *os);
  }
  catch (std::exception & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
