/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/algorithms/explore/run.hh"
#include "tchecker/utils/waiting.hh"

namespace tchecker {
  
  namespace explore {
    
    namespace details {
      
      namespace fsm {
        
        std::tuple<tchecker::intvar_index_t const &>
        explored_model_t::node_outputter_args(tchecker::explore::details::fsm::explored_model_t::model_t const & model)
        {
          return std::tuple<tchecker::intvar_index_t const &>(model.flattened_integer_variables().index());
        }
        
        std::tuple<>
        explored_model_t::edge_outputter_args(tchecker::explore::details::fsm::explored_model_t::model_t const & model)
        {
          return std::make_tuple();
        }
        
      } // end of namespace fsm
      
      
      
      
      namespace ta {
        
        std::tuple<tchecker::intvar_index_t const &>
        explored_model_t::node_outputter_args(tchecker::explore::details::ta::explored_model_t::model_t const & model)
        {
          return std::tuple<tchecker::intvar_index_t const &>(model.flattened_integer_variables().index());
        }
        
        std::tuple<tchecker::clock_index_t const &>
        explored_model_t::edge_outputter_args(tchecker::explore::details::ta::explored_model_t::model_t const & model)
        {
          return std::tuple<tchecker::clock_index_t const &>(model.flattened_clock_variables().index());
        }
        
      } // end of namespace ta
      
    } // end of namespace details
    
    
    
    
    
    void run(tchecker::parsing::system_declaration_t const & sysdecl,
             tchecker::explore::options_t const & options,
             tchecker::log_t & log)
    {
      switch (options.search_order()) {
        case tchecker::explore::options_t::BFS:
          tchecker::explore::details::run<tchecker::fifo_waiting_t>(sysdecl, options, log);
          break;
        case tchecker::explore::options_t::DFS:
          tchecker::explore::details::run<tchecker::lifo_waiting_t>(sysdecl, options, log);
          break;
        default:
          log.error("Unsupported search order for explore algorithm");
          break;
      }
    }
    
  } // end of namespace explore
  
} // end of namespace tchecker
