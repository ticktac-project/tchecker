/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_EXPLORE_RUN_HH
#define TCHECKER_ALGORITHMS_EXPLORE_RUN_HH

#include "tchecker/algorithms/explore/algorithm.hh"
#include "tchecker/algorithms/explore/graph.hh"
#include "tchecker/algorithms/explore/options.hh"
#include "tchecker/async_zg/async_zg_ta.hh"
#include "tchecker/fsm/fsm.hh"
#include "tchecker/graph/allocators.hh"
#include "tchecker/graph/output.hh"
#include "tchecker/parsing/declaration.hh"
#include "tchecker/ta/ta.hh"
#include "tchecker/utils/gc.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/zg/zg_ta.hh"

/*!
 \file run.hh
 \brief Running explore algorithm
 */

namespace tchecker {
  
  namespace explore {
    
    namespace details {
      
      namespace fsm {
        
        /*!
         \class explored_model_t
         \brief Explored model for finite state machines
         */
        class explored_model_t {
        public:
          using model_t = tchecker::fsm::model_t;
          using ts_t = tchecker::fsm::ts_t;
          using node_t = tchecker::explore::node_t<ts_t::state_t>;
          using edge_t = tchecker::explore::edge_t<ts_t::transition_t>;
          using node_outputter_t = tchecker::fsm::state_outputter_t;
          using edge_outputter_t = tchecker::fsm::transition_outputter_t;
          using node_allocator_t = tchecker::fsm::state_pool_allocator_t<tchecker::make_shared_t<node_t>>;
          using edge_allocator_t = tchecker::fsm::transition_singleton_allocator_t<edge_t>;
          using graph_allocator_t = tchecker::graph::graph_allocator_t<node_allocator_t, edge_allocator_t>;
          
          static std::tuple<tchecker::intvar_index_t const &>
          node_outputter_args(tchecker::explore::details::fsm::explored_model_t::model_t const & model);
          static std::tuple<> edge_outputter_args(tchecker::explore::details::fsm::explored_model_t::model_t const & model);
        };
        
      } // end of namespace fsm
      
      
      
      
      namespace ta {
        
        /*!
         \class explored_model_t
         \brief Explored model for timed automata
         */
        class explored_model_t {
        public:
          using model_t = tchecker::ta::model_t;
          using ts_t = tchecker::ta::ts_t;
          using node_t = tchecker::explore::node_t<ts_t::state_t>;
          using edge_t = tchecker::explore::edge_t<ts_t::transition_t>;
          using node_outputter_t = tchecker::ta::state_outputter_t;
          using edge_outputter_t = tchecker::ta::transition_outputter_t;
          using node_allocator_t = tchecker::ta::state_pool_allocator_t<tchecker::make_shared_t<node_t>>;
          using edge_allocator_t = tchecker::ta::transition_singleton_allocator_t<edge_t>;
          using graph_allocator_t = tchecker::graph::graph_allocator_t<node_allocator_t, edge_allocator_t>;
          
          static std::tuple<tchecker::intvar_index_t const &>
          node_outputter_args(tchecker::explore::details::ta::explored_model_t::model_t const & model);
          static std::tuple<tchecker::clock_index_t const &>
          edge_outputter_args(tchecker::explore::details::ta::explored_model_t::model_t const & model);
        };
        
      } // end of namespace ta
      
      
      
      
      namespace zg {
        
        namespace ta {
          
          /*!
           \class explored_model_t
           \brief Explored model for zone graphs of timed automata
           */
          template <class ZONE_SEMANTICS>
          class explored_model_t {
          public:
            using zone_semantics_t = ZONE_SEMANTICS;
            using model_t = tchecker::zg::ta::model_t;
            using ts_t = typename zone_semantics_t::ts_t;
            using node_t = tchecker::explore::node_t<typename ts_t::state_t>;
            using edge_t = tchecker::explore::edge_t<typename ts_t::transition_t>;
            using node_outputter_t = tchecker::zg::ta::state_outputter_t;
            using edge_outputter_t = tchecker::zg::ta::transition_outputter_t;
            using node_allocator_t = typename zone_semantics_t::template state_pool_allocator_t<tchecker::make_shared_t<node_t>>;
            using edge_allocator_t = typename zone_semantics_t::template transition_singleton_allocator_t<edge_t>;
            using graph_allocator_t = tchecker::graph::graph_allocator_t<node_allocator_t, edge_allocator_t>;
            
            static std::tuple<tchecker::intvar_index_t const &, tchecker::clock_index_t const &>
            node_outputter_args(tchecker::explore::details::zg::ta::explored_model_t<ZONE_SEMANTICS>::model_t const & model)
            {
              return std::tuple<tchecker::intvar_index_t const &, tchecker::clock_index_t const &>
              (model.flattened_integer_variables().index(), model.flattened_clock_variables().index());
            }
            
            static std::tuple<tchecker::clock_index_t const &>
            edge_outputter_args(tchecker::explore::details::zg::ta::explored_model_t<ZONE_SEMANTICS>::model_t const & model)
            {
              return std::tuple<tchecker::clock_index_t const &>(model.flattened_clock_variables().index());
            }
          };
          
        } // end of namespace ta
        
      } // end of namespace zg
      
      
      
      
      namespace async_zg {
        
        namespace ta {
          
          /*!
           \class explored_model_t
           \brief Explored model for asynchronous zone graphs of timed automata
           */
          template <class ZONE_SEMANTICS>
          class explored_model_t {
          public:
            using zone_semantics_t = ZONE_SEMANTICS;
            using model_t = tchecker::async_zg::ta::model_t;
            using ts_t = typename zone_semantics_t::ts_t;
            using node_t = tchecker::explore::node_t<typename ts_t::state_t>;
            using edge_t = tchecker::explore::edge_t<typename ts_t::transition_t>;
            using node_outputter_t = tchecker::async_zg::ta::state_outputter_t;
            using edge_outputter_t = tchecker::async_zg::ta::transition_outputter_t;
            using node_allocator_t = typename zone_semantics_t::template state_pool_allocator_t<tchecker::make_shared_t<node_t>>;
            using edge_allocator_t = typename zone_semantics_t::template transition_singleton_allocator_t<edge_t>;
            using graph_allocator_t = tchecker::graph::graph_allocator_t<node_allocator_t, edge_allocator_t>;
            
            static std::tuple<tchecker::intvar_index_t const &, tchecker::clock_index_t const &, tchecker::clock_index_t const &>
            node_outputter_args(tchecker::explore::details::async_zg::ta::explored_model_t<ZONE_SEMANTICS>::model_t const & model)
            {
              return std::tuple<tchecker::intvar_index_t const &, tchecker::clock_index_t const &, tchecker::clock_index_t const &>
              (model.flattened_integer_variables().index(),
               model.flattened_offset_clock_variables().index(),
               model.flattened_clock_variables().index());
            }
            
            static std::tuple<tchecker::clock_index_t const &>
            edge_outputter_args(tchecker::explore::details::async_zg::ta::explored_model_t<ZONE_SEMANTICS>::model_t const & model)
            {
              // display invariants, guards and resets w.r.t. system clocks
              return std::tuple<tchecker::clock_index_t const &>(model.flattened_clock_variables().index());
            }
          };
          
        } // end of namespace ta
        
      } // end of namespace async_zg
      
      
      
      
      /*!
       \brief Run explore algorithm
       \tparam EXPLORED_MODEL : type of explored model
       \tparam GRAPH_OUTPUTTER : type of graph outputter
       \tparam WAITING : type of waiting container
       \param sysdecl : a system declaration
       \param options : explore algorithm options
       \param log : logging facility
       \post explore algorithm has been run on a model of sysdecl as defined by EXPLORED_MODEL
       and following options and the exploreation policy implented by WAITING. The graph has
       been output using GRAPH_OUPUTTER
       Every error and warning has been reported to log.
       */
      template
      <class EXPLORED_MODEL,
      template <class N, class E, class NO, class EO> class GRAPH_OUTPUTTER,
      template <class NPTR> class WAITING
      >
      void run(tchecker::parsing::system_declaration_t const & sysdecl,
               tchecker::explore::options_t const & options,
               tchecker::log_t & log)
      {
        using model_t = typename EXPLORED_MODEL::model_t;
        using ts_t = typename EXPLORED_MODEL::ts_t;
        using node_t = typename EXPLORED_MODEL::node_t;
        using edge_t = typename EXPLORED_MODEL::edge_t;
        using hash_t = tchecker::intrusive_shared_ptr_delegate_hash_t;
        using equal_to_t = tchecker::intrusive_shared_ptr_delegate_equal_to_t;
        using graph_allocator_t = typename EXPLORED_MODEL::graph_allocator_t;
        using node_outputter_t = typename EXPLORED_MODEL::node_outputter_t;
        using edge_outputter_t = typename EXPLORED_MODEL::edge_outputter_t;
        using graph_outputter_t = GRAPH_OUTPUTTER<node_t, edge_t, node_outputter_t, edge_outputter_t>;
        using graph_t = tchecker::explore::graph_t<graph_allocator_t, hash_t, equal_to_t, graph_outputter_t>;
        
        model_t model(sysdecl, log);
        ts_t ts(model);
        
        tchecker::gc_t gc;
        
        graph_t graph(model.system().name(),
                      std::tuple<tchecker::gc_t &, std::tuple<model_t &, std::size_t>, std::tuple<>>
                      (gc, std::tuple<model_t &, std::size_t>(model, options.block_size()), std::make_tuple()),
                      options.output_stream(),
                      EXPLORED_MODEL::node_outputter_args(model),
                      EXPLORED_MODEL::edge_outputter_args(model));
        
        gc.start();
        
        tchecker::explore::algorithm_t<ts_t, graph_t, WAITING> algorithm;
        
        try {
          algorithm.run(ts, graph);
        }
        catch (...) {
          gc.stop();
          graph.free_all();
          throw;
        }
        
        gc.stop();
        graph.free_all();
      }
      
      
      /*!
       \brief Run explore algorithm
       \tparam GRAPH_OUTPUTTER : type of graph outputter
       \tparam WAITING : type of waiting container
       \param sysdecl : a system declaration
       \param log : logging facility
       \param options : explore algorithm options
       \post explore algorithm has been run on a model of sysdecl following options and
       the exploration policy implemented by WAITING. The graph has been output using
       GRAPH_OUPUTTER
       Every error and warning has been reported to log.
       */
      template <template <class N, class E, class NO, class EO> class GRAPH_OUTPUTTER, template <class NPTR> class WAITING>
      void run(tchecker::parsing::system_declaration_t const & sysdecl,
               tchecker::explore::options_t const & options,
               tchecker::log_t & log)
      {
        switch (options.explored_model()) {
          case tchecker::explore::options_t::FSM:
            tchecker::explore::details::run<tchecker::explore::details::fsm::explored_model_t, GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::TA:
            tchecker::explore::details::run<tchecker::explore::details::ta::explored_model_t, GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_ELAPSED_NOEXTRA:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::elapsed_no_extrapolation_t>, GRAPH_OUTPUTTER,
            WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_ELAPSED_EXTRAM_G:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::elapsed_extraM_global_t>, GRAPH_OUTPUTTER,
            WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_ELAPSED_EXTRAM_L:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::elapsed_extraM_local_t>, GRAPH_OUTPUTTER,
            WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_ELAPSED_EXTRAM_PLUS_G:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::elapsed_extraMplus_global_t>, GRAPH_OUTPUTTER,
            WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_ELAPSED_EXTRAM_PLUS_L:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::elapsed_extraMplus_local_t>, GRAPH_OUTPUTTER,
            WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_ELAPSED_EXTRALU_G:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::elapsed_extraLU_global_t>, GRAPH_OUTPUTTER,
            WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_ELAPSED_EXTRALU_L:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::elapsed_extraLU_local_t>, GRAPH_OUTPUTTER,
            WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_ELAPSED_EXTRALU_PLUS_G:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::elapsed_extraLUplus_global_t>, GRAPH_OUTPUTTER,
            WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_ELAPSED_EXTRALU_PLUS_L:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::elapsed_extraLUplus_local_t>, GRAPH_OUTPUTTER,
            WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_NON_ELAPSED_NOEXTRA:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::non_elapsed_no_extrapolation_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRAM_G:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::non_elapsed_extraM_global_t>, GRAPH_OUTPUTTER,
            WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRAM_L:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::non_elapsed_extraM_local_t>, GRAPH_OUTPUTTER,
            WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRAM_PLUS_G:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::non_elapsed_extraMplus_global_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRAM_PLUS_L:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::non_elapsed_extraMplus_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRALU_G:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::non_elapsed_extraLU_global_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRALU_L:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::non_elapsed_extraLU_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRALU_PLUS_G:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::non_elapsed_extraLUplus_global_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRALU_PLUS_L:
            tchecker::explore::details::run
            <tchecker::explore::details::zg::ta::explored_model_t<tchecker::zg::ta::non_elapsed_extraLU_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ASYNC_ZG_ELAPSED_EXTRALU_PLUS_L:
            tchecker::explore::details::run
            <tchecker::explore::details::async_zg::ta::explored_model_t<tchecker::async_zg::ta::elapsed_extraLUplus_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::explore::options_t::ASYNC_ZG_NON_ELAPSED_EXTRALU_PLUS_L:
            tchecker::explore::details::run
            <tchecker::explore::details::async_zg::ta::explored_model_t<tchecker::async_zg::ta::non_elapsed_extraLUplus_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          default:
            log.error("unsupported explored model");
        }
      }
      
      
      
      
      
      /*!
       \brief Run explore algorithm
       \tparam WAITING : type of waiting container
       \param sysdecl : a system declaration
       \param options : explore algorithm options
       \param log : logging facility
       \post explore algorithm has been run on a model of sysdecl following options and
       the exploration policy implemented by WAITING
       Every error and warning has been reported to log.
       */
      template <template <class NPTR> class WAITING>
      void run(tchecker::parsing::system_declaration_t const & sysdecl,
               tchecker::explore::options_t const & options,
               tchecker::log_t & log)
      {
        switch (options.output_format()) {
          case tchecker::explore::options_t::DOT:
            tchecker::explore::details::run<tchecker::graph::dot_outputter_t, WAITING>(sysdecl, options, log);
            break;
          case tchecker::explore::options_t::RAW:
            tchecker::explore::details::run<tchecker::graph::raw_outputter_t, WAITING>(sysdecl, options, log);
            break;
          default:
            log.error("unsupported output format");
        }
      }
      
    } // end of namespace details
    
    
    
    
    /*!
     \brief Run explore algorithm
     \param sysdecl : a system declaration
     \param options : explore algorithm options
     \param log : logging facility
     \post explore algorithm has been run on a model of sysdecl following options.
     Every error and warning has been reported to log.
     */
    void run(tchecker::parsing::system_declaration_t const & sysdecl,
             tchecker::explore::options_t const & options,
             tchecker::log_t & log);
    
  } // end of namespace explore
  
} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_EXPLORE_RUN_HH
