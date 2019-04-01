/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/algorithms/covreach/run.hh"
#include "tchecker/algorithms/covreach/waiting.hh"

namespace tchecker {
  
  namespace covreach {
    
    void run(tchecker::parsing::system_declaration_t const & sysdecl,
             tchecker::covreach::options_t const & options,
             tchecker::log_t & log)
    {
      switch (options.search_order()) {
        case tchecker::covreach::options_t::BFS:
          tchecker::covreach::details::run<tchecker::covreach::fifo_waiting_t>(sysdecl, options, log);
          break;
        case tchecker::covreach::options_t::DFS:
          tchecker::covreach::details::run<tchecker::covreach::lifo_waiting_t>(sysdecl, options, log);
          break;
        default:
          log.error("Unsupported search order for covreach algorithm");
          break;
      }
    }
    
  } // end of namespace covreach
  
} // end of namespace tchecker
