/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <fstream>
#include <getopt.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "concur19.hh"
#include "tchecker/algorithms/reach/algorithm.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/utils/log.hh"
#include "zg-covreach.hh"
#include "zg-reach.hh"

/*!
 \file tck-reach.cc
 \brief Reachability analysis of timed automata
 */

static struct option long_options[] = {{"algorithm", required_argument, 0, 'a'},
                                       {"certificate", required_argument, 0, 'C'},
                                       {"output", required_argument, 0, 'o'},
                                       {"help", no_argument, 0, 'h'},
                                       {"labels", required_argument, 0, 'l'},
                                       {"search-order", no_argument, 0, 's'},
                                       {"block-size", required_argument, 0, 0},
                                       {"table-size", required_argument, 0, 0},
                                       {0, 0, 0, 0}};

static char const * const options = (char *)"a:C:hl:o:s:";

/*!
  \brief Display usage
  \param progname : programme name
*/
void usage(char * progname)
{
  std::cerr << "Usage: " << progname << " [options] [file]" << std::endl;
  std::cerr << "   -a algorithm  reachability algorithm" << std::endl;
  std::cerr << "          reach      standard reachability algorithm over the zone graph" << std::endl;
  std::cerr << "          concur19   reachability algorithm with covering over the local-time zone graph" << std::endl;
  std::cerr << "          covreach   reachability algorithm with covering over the zone graph" << std::endl;
  std::cerr << "   -C type       type of certificate" << std::endl;
  std::cerr << "          none       no certificate (default)" << std::endl;
  std::cerr << "          graph      graph of explored state-space" << std::endl;
  std::cerr << "          symbolic   symbolic run to a state with searched labels if any" << std::endl;
  std::cerr << "          concrete   concrete run to a state with searched labels if any (only for reach and covreach)"
            << std::endl;
  std::cerr << "   -h            help" << std::endl;
  std::cerr << "   -l l1,l2,...  comma-separated list of searched labels" << std::endl;
  std::cerr << "   -o out_file   output file for certificate (default is standard output)" << std::endl;
  std::cerr << "   -s bfs|dfs    search order" << std::endl;
  std::cerr << "   --block-size  size of allocation blocks" << std::endl;
  std::cerr << "   --table-size  size of hash tables" << std::endl;
  std::cerr << "reads from standard input if file is not provided" << std::endl;
}

enum algorithm_t {
  ALGO_REACH,    /*!< Reachability algorithm */
  ALGO_CONCUR19, /*!< Covering reachability algorithm over the local-time zone graph */
  ALGO_COVREACH, /*!< Covering reachability algorithm */
  ALGO_NONE,     /*!< No algorithm */
};

enum certificate_t {
  CERTIFICATE_GRAPH,    /*!< Graph of state-space */
  CERTIFICATE_SYMBOLIC, /*!< Symbolic counter-example */
  CERTIFICATE_CONCRETE, /*!< Concrete counter-example */
  CERTIFICATE_NONE,     /*!< No certificate */
};

static enum algorithm_t algorithm = ALGO_NONE;            /*!< Selected algorithm */
static bool help = false;                                 /*!< Help flag */
static enum certificate_t certificate = CERTIFICATE_NONE; /*!< Type of certificate */
static std::string search_order = "bfs";                  /*!< Search order */
static std::string labels = "";                           /*!< Searched labels */
static std::string output_file = "";                      /*!< Output file name (empty means standard output) */
static std::ostream * os = &std::cout;                    /*!< Default output stream */
static std::size_t block_size = 10000;                    /*!< Size of allocated blocks */
static std::size_t table_size = 65536;                    /*!< Size of hash tables */

/*!
 \brief Check if expected certificate is a path
 \param ctype : certificate type
 \return true if ctype is a path, false otherwise
 */
static bool is_certificate_path(enum certificate_t ctype)
{
  return (ctype == CERTIFICATE_SYMBOLIC || ctype == CERTIFICATE_CONCRETE);
}

/*!
 \brief Parse command-line arguments
 \param argc : number of arguments
 \param argv : array of arguments
 \pre argv[0] up to argv[argc-1] are valid accesses
 \post global variables help, output_file, search_order and labels have been set
 from argv
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
      case 'a':
        if (strcmp(optarg, "reach") == 0)
          algorithm = ALGO_REACH;
        else if (strcmp(optarg, "concur19") == 0)
          algorithm = ALGO_CONCUR19;
        else if (strcmp(optarg, "covreach") == 0)
          algorithm = ALGO_COVREACH;
        else
          throw std::runtime_error("Unknown algorithm: " + std::string(optarg));
        break;
      case 'C':
        if (strcmp(optarg, "none") == 0)
          certificate = CERTIFICATE_NONE;
        else if (strcmp(optarg, "graph") == 0)
          certificate = CERTIFICATE_GRAPH;
        else if (strcmp(optarg, "concrete") == 0)
          certificate = CERTIFICATE_CONCRETE;
        else if (strcmp(optarg, "symbolic") == 0)
          certificate = CERTIFICATE_SYMBOLIC;
        else
          throw std::runtime_error("Unknown type of certificate: " + std::string(optarg));
        break;
      case 'o':
        output_file = optarg;
        break;
      case 'h':
        help = true;
        break;
      case 'l':
        labels = optarg;
        break;
      case 's':
        search_order = optarg;
        break;
      default:
        throw std::runtime_error("This should never be executed");
        break;
      }
    }
    else {
      if (strcmp(long_options[long_option_index].name, "block-size") == 0)
        block_size = std::strtoull(optarg, nullptr, 10);
      else if (strcmp(long_options[long_option_index].name, "table-size") == 0)
        table_size = std::strtoull(optarg, nullptr, 10);
      else
        throw std::runtime_error("This also should never be executed");
    }
  }

  return optind;
}

/*!
 \brief Load a system declaration from a file
 \param filename : file name
 \return pointer to a system declaration loaded from filename, nullptr in case
 of errors
 \post all errors have been reported to std::cerr
*/
std::shared_ptr<tchecker::parsing::system_declaration_t> load_system_declaration(std::string const & filename)
{
  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{nullptr};
  try {
    sysdecl = tchecker::parsing::parse_system_declaration(filename);
    if (sysdecl == nullptr)
      throw std::runtime_error("nullptr system declaration");
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
  }
  return sysdecl;
}

/*!
 \brief Perform reachability analysis
 \param sysdecl : system declaration
 \post statistics on reachability analysis of command-line specified labels in
 the system declared by sysdecl have been output to standard output.
 A certification has been output if required.
*/
void reach(tchecker::parsing::system_declaration_t const & sysdecl)
{
  auto && [stats, graph] = tchecker::tck_reach::zg_reach::run(sysdecl, labels, search_order, block_size, table_size);

  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;

  // certificate
  if (certificate == CERTIFICATE_GRAPH)
    tchecker::tck_reach::zg_reach::dot_output(*os, *graph, sysdecl.name());
  else if ((certificate == CERTIFICATE_CONCRETE) && stats.reachable()) {
    std::unique_ptr<tchecker::tck_reach::zg_reach::cex::concrete_cex_t> cex{
        tchecker::tck_reach::zg_reach::cex::concrete_counter_example(*graph)};
    if (cex->empty())
      throw std::runtime_error("Unable to compute a concrete counter example");
    tchecker::tck_reach::zg_reach::cex::dot_output(*os, *cex, sysdecl.name());
  }
  else if ((certificate == CERTIFICATE_SYMBOLIC) && stats.reachable()) {
    std::unique_ptr<tchecker::tck_reach::zg_reach::cex::symbolic_cex_t> cex{
        tchecker::tck_reach::zg_reach::cex::symbolic_counter_example(*graph)};
    if (cex->empty())
      throw std::runtime_error("Unable to compute a symbolic counter example");
    tchecker::tck_reach::zg_reach::cex::dot_output(*os, *cex, sysdecl.name());
  }
}

/*!
 \brief Perform covering reachability analysis over the local-time zone graph
 \param sysdecl : system declaration
 \post statistics on covering reachability analysis of command-line specified
 labels in the system declared by sysdecl have been output to standard output.
 A certification has been output if required.
 \note This is the algorithm presented in R. Govind, Frédéric Herbreteau, B.
 Srivathsan, Igor Walukiewicz: "Revisiting Local Time Semantics for Networks of
 Timed Automata". CONCUR 2019: 16:1-16:15
*/
void concur19(tchecker::parsing::system_declaration_t const & sysdecl)
{
  tchecker::algorithms::covreach::stats_t stats;
  std::shared_ptr<tchecker::tck_reach::concur19::graph_t> graph;

  if (certificate == CERTIFICATE_CONCRETE)
    throw std::runtime_error("Concrete counter-example is not available for concur19 algorithm");

  tchecker::algorithms::covreach::covering_t covering =
      (is_certificate_path(certificate) ? tchecker::algorithms::covreach::COVERING_LEAF_NODES
                                        : tchecker::algorithms::covreach::COVERING_FULL);

  std::tie(stats, graph) = tchecker::tck_reach::concur19::run(sysdecl, labels, search_order, covering, block_size, table_size);

  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;

  // certificate
  if (certificate == CERTIFICATE_GRAPH)
    tchecker::tck_reach::concur19::dot_output(*os, *graph, sysdecl.name());
  else if ((certificate == CERTIFICATE_SYMBOLIC) && stats.reachable()) {
    std::unique_ptr<tchecker::tck_reach::concur19::cex::symbolic::cex_t> cex{
        tchecker::tck_reach::concur19::cex::symbolic::counter_example(*graph)};
    if (cex->empty())
      throw std::runtime_error("Unable to compute a symbolic counter example");
    tchecker::tck_reach::concur19::cex::symbolic::dot_output(*os, *cex, sysdecl.name());
  }
}

/*!
 \brief Perform covering reachability analysis
 \param sysdecl : system declaration
 \post statistics on covering reachability analysis of command-line specified
 labels in the system declared by sysdecl have been output to standard output.
 A certification has been output if required.
*/
void covreach(tchecker::parsing::system_declaration_t const & sysdecl)
{
  tchecker::algorithms::covreach::stats_t stats;
  std::shared_ptr<tchecker::tck_reach::zg_covreach::graph_t> graph;

  tchecker::algorithms::covreach::covering_t covering =
      (is_certificate_path(certificate) ? tchecker::algorithms::covreach::COVERING_LEAF_NODES
                                        : tchecker::algorithms::covreach::COVERING_FULL);

  std::tie(stats, graph) =
      tchecker::tck_reach::zg_covreach::run(sysdecl, labels, search_order, covering, block_size, table_size);

  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;

  // certificate
  if (certificate == CERTIFICATE_GRAPH)
    tchecker::tck_reach::zg_covreach::dot_output(*os, *graph, sysdecl.name());
  else if ((certificate == CERTIFICATE_CONCRETE) && stats.reachable()) {
    std::unique_ptr<tchecker::tck_reach::zg_covreach::cex::concrete_cex_t> cex{
        tchecker::tck_reach::zg_covreach::cex::concrete_counter_example(*graph)};
    if (cex->empty())
      throw std::runtime_error("Unable to compute a concrete counter example");
    tchecker::tck_reach::zg_covreach::cex::dot_output(*os, *cex, sysdecl.name());
  }
  else if ((certificate == CERTIFICATE_SYMBOLIC) && stats.reachable()) {
    std::unique_ptr<tchecker::tck_reach::zg_covreach::cex::symbolic_cex_t> cex{
        tchecker::tck_reach::zg_covreach::cex::symbolic_counter_example(*graph)};
    if (cex->empty())
      throw std::runtime_error("Unable to compute a symbolic counter example");
    tchecker::tck_reach::zg_covreach::cex::dot_output(*os, *cex, sysdecl.name());
  }
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

    if ((certificate == CERTIFICATE_CONCRETE) && (algorithm != ALGO_COVREACH) && (algorithm != ALGO_REACH)) {
      std::cerr << "Concrete counter-example is only available for algorithms covreach and reach" << std::endl;
      return EXIT_FAILURE;
    }

    if (help) {
      usage(argv[0]);
      return EXIT_SUCCESS;
    }

    std::string input_file = (optindex == argc ? "" : argv[optindex]);

    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{load_system_declaration(input_file)};

    if (tchecker::log_error_count() > 0)
      return EXIT_FAILURE;

    std::shared_ptr<std::ofstream> os_ptr{nullptr};

    if (certificate != CERTIFICATE_NONE && output_file != "") {
      try {
        os_ptr = std::make_shared<std::ofstream>(output_file);
        os = os_ptr.get();
      }
      catch (std::exception & e) {
        std::cerr << tchecker::log_error << e.what() << std::endl;
        return EXIT_FAILURE;
      }
    }

    switch (algorithm) {
    case ALGO_REACH:
      reach(*sysdecl);
      break;
    case ALGO_CONCUR19:
      concur19(*sysdecl);
      break;
    case ALGO_COVREACH:
      covreach(*sysdecl);
      break;
    default:
      throw std::runtime_error("No algorithm specified");
    }
  }
  catch (std::exception & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
