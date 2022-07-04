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

#include "tchecker/parsing/parsing.hh"
#include "tchecker/utils/log.hh"
#include "zg-couvscc.hh"
#include "zg-ndfs.hh"

/*!
 \file tck-liveness.cc
 \brief Liveness verification of timed automata
 */

static struct option long_options[] = {{"algorithm", required_argument, 0, 'a'},
                                       {"certificate", no_argument, 0, 'C'},
                                       {"help", no_argument, 0, 'h'},
                                       {"labels", required_argument, 0, 'l'},
                                       {"block-size", required_argument, 0, 0},
                                       {"table-size", required_argument, 0, 0},
                                       {0, 0, 0, 0}};

static char const * const options = (char *)"a:C:hl:";

/*!
  \brief Display usage
  \param progname : programme name
*/
void usage(char * progname)
{
  std::cerr << "Usage: " << progname << " [options] [file]" << std::endl;
  std::cerr << "   -a algorithm  liveness algorithm" << std::endl;
  std::cerr << "          couvscc    Couvreur's SCC-decomposition-based algorithm" << std::endl;
  std::cerr << "                     search an accepting cycle that visits all labels" << std::endl;
  std::cerr << "          ndfs       nested depth-first search algorithm over the zone graph" << std::endl;
  std::cerr << "                     search an accepting cycle with a state with all labels" << std::endl;
  std::cerr << "   -C out_file   output a certificate (as a graph) in out_file" << std::endl;
  std::cerr << "   -h            help" << std::endl;
  std::cerr << "   -l l1,l2,...  comma-separated list of accepting labels" << std::endl;
  std::cerr << "   --block-size  size of allocation blocks" << std::endl;
  std::cerr << "   --table-size  size of hash tables" << std::endl;
  std::cerr << "reads from standard input if file is not provided" << std::endl;
}

enum algorithm_t {
  ALGO_COUVSCC, /*!< Couvreur's SCC algorithm */
  ALGO_NDFS,    /*!< Nested DFS algorithm */
  ALGO_NONE,    /*!< No algorithm */
};

static enum algorithm_t algorithm = ALGO_NONE; /*!< Selected algorithm */
static bool help = false;                      /*!< Help flag */
static std::string output_file = "";           /*!< Output file name */
static std::string labels = "";                /*!< Searched labels */
static std::size_t block_size = 10000;         /*!< Size of allocated blocks */
static std::size_t table_size = 65536;         /*!< Size of hash tables */

/*!
 \brief Parse command-line arguments
 \param argc : number of arguments
 \param argv : array of arguments
 \pre argv[0] up to argv[argc-1] are valid accesses
 \post global variables algorithm, help, output_file and labels have been set
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
        if (strcmp(optarg, "ndfs") == 0)
          algorithm = ALGO_NDFS;
        else if (strcmp(optarg, "couvscc") == 0)
          algorithm = ALGO_COUVSCC;
        else
          throw std::runtime_error("Unknown algorithm: " + std::string(optarg));
        break;
      case 'C':
        output_file = optarg;
        break;
      case 'h':
        help = true;
        break;
      case 'l':
        labels = optarg;
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
tchecker::parsing::system_declaration_t * load_system_declaration(std::string const & filename)
{
  tchecker::parsing::system_declaration_t * sysdecl = nullptr;
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
 \brief Run nested DFS algorithm
 \param sysdecl : system declaration
 \post statistics on accepting run w.r.t. command-line specified labels in
 the system declared by sysdecl have been output to standard output.
 A certificate has been output if required.
*/
void ndfs(std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl)
{
  auto && [stats, graph] = tchecker::tck_liveness::zg_ndfs::run(sysdecl, labels, block_size, table_size);

  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;

  // graph
  if (output_file != "") {
    std::ofstream ofs{output_file};
    tchecker::tck_liveness::zg_ndfs::dot_output(ofs, *graph, sysdecl->name());
    ofs.close();
  }
}

/*!
 \brief Run Couvreur's algorithm
 \param sysdecl : system declaration
 \post statistics on accepting run w.r.t. command-line specified labels in
 the system declared by sysdecl have been output to standard output.
 A certificate has been output if required.
*/
void couvscc(std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl)
{
  auto && [stats, graph] = tchecker::tck_liveness::zg_couvscc::run(sysdecl, labels, block_size, table_size);

  // stats
  std::map<std::string, std::string> m;
  stats.attributes(m);
  for (auto && [key, value] : m)
    std::cout << key << " " << value << std::endl;

  // graph
  if (output_file != "") {
    std::ofstream ofs{output_file};
    tchecker::tck_liveness::zg_couvscc::dot_output(ofs, *graph, sysdecl->name());
    ofs.close();
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

    if (help) {
      usage(argv[0]);
      return EXIT_SUCCESS;
    }

    std::string input_file = (optindex == argc ? "" : argv[optindex]);

    std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{load_system_declaration(input_file)};

    if (tchecker::log_error_count() > 0)
      return EXIT_FAILURE;

    switch (algorithm) {
    case ALGO_NDFS:
      ndfs(sysdecl);
      break;
    case ALGO_COUVSCC:
      couvscc(sysdecl);
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
