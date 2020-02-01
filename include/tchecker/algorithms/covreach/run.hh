/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COVREACH_RUN_HH
#define TCHECKER_ALGORITHMS_COVREACH_RUN_HH

#include "tchecker/algorithms/covreach/accepting.hh"
#include "tchecker/algorithms/covreach/algorithm.hh"
#include "tchecker/algorithms/covreach/cover.hh"
#include "tchecker/algorithms/covreach/graph.hh"
#include "tchecker/algorithms/covreach/options.hh"
#include "tchecker/async_zg/async_zg_ta.hh"
#include "tchecker/algorithms/covreach/output.hh"
#include "tchecker/algorithms/covreach/stats.hh"
#include "tchecker/graph/allocators.hh"
#include "tchecker/graph/output.hh"
#include "tchecker/parsing/declaration.hh"
#include "tchecker/ts/allocators.hh"
#include "tchecker/utils/gc.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/zg/zg_ta.hh"

/*!
 \file run.hh
 \brief Running explore algorithm
 */

namespace tchecker {
  
  namespace covreach {
    
    namespace details {
      
      namespace zg {
        
        namespace ta {
          
          /*!
           \class algorithm_model_t
           \brief Model for covering reachability over zone graphs of timed automata
           */
          template <class ZONE_SEMANTICS>
          class algorithm_model_t {
          public:
            using zone_semantics_t = ZONE_SEMANTICS;
            using model_t = tchecker::zg::ta::model_t;
            
            using ts_t = typename zone_semantics_t::ts_t;
            using state_t = typename ts_t::state_t;
            using transition_t = typename ts_t::transition_t;
            
            using key_t = std::size_t;
            
            using node_t = typename tchecker::covreach::details::graph_types_t<ts_t>::node_t;
            using node_ptr_t = typename tchecker::covreach::details::graph_types_t<ts_t>::node_ptr_t;
            
            using node_allocator_t = typename zone_semantics_t::template state_pool_allocator_t<node_t>;
            using transition_allocator_t = typename zone_semantics_t::template transition_singleton_allocator_t<transition_t>;
            using ts_allocator_t = tchecker::ts::allocator_t<node_allocator_t, transition_allocator_t>;
            
            using graph_t = tchecker::covreach::graph_t<key_t, ts_t, ts_allocator_t>;
            
            static inline key_t node_to_key(node_ptr_t const & node)
            {
              return tchecker::ta::hash_value(*node);
            }
            
            class state_predicate_t {
            public:
              using node_ptr_t
              = typename tchecker::covreach::details::zg::ta::algorithm_model_t<ZONE_SEMANTICS>::node_ptr_t;
              
              bool operator() (node_ptr_t const & n1, node_ptr_t const & n2)
              {
                return (static_cast<tchecker::ta::state_t const &>(*n1)
                        == static_cast<tchecker::ta::state_t const &>(*n2));
              }
            };
            
            class node_lt_t {
            public:
              bool operator() (node_ptr_t const & n1, node_ptr_t const & n2) const
              {
                return tchecker::zg::lexical_cmp(*n1, *n2) < 0;
              }
            };
            
            static std::tuple<> state_predicate_args(model_t const & model)
            {
              return std::tuple<>();
            }
            
            static std::tuple<model_t const &> zone_predicate_args(model_t const & model)
            {
              return std::tuple<model_t const &>(model);
            }
            
            using node_outputter_t = tchecker::zg::ta::state_outputter_t;
            
            static std::tuple<tchecker::intvar_index_t const &, tchecker::clock_index_t const &>
            node_outputter_args(model_t const & model)
            {
              return std::tuple<tchecker::intvar_index_t const &, tchecker::clock_index_t const &>
              (model.flattened_integer_variables().index(), model.flattened_clock_variables().index());
            }
          };
          
        } // end of namespace ta
        
      } // end of namespace zg
      
      
      
      
      namespace async_zg {
        
        namespace ta {
          
          /*!
           \class algorithm_model_t
           \brief Model for covering reachability over asynchronous zone graphs of timed automata
           */
          template <class ZONE_SEMANTICS>
          class algorithm_model_t {
          public:
            using zone_semantics_t = ZONE_SEMANTICS;
            using model_t = tchecker::async_zg::ta::model_t;
            
            using ts_t = typename zone_semantics_t::ts_t;
            using state_t = typename ts_t::state_t;
            using transition_t = typename ts_t::transition_t;
            
            using key_t = std::size_t;
            
            using node_t = typename tchecker::covreach::details::graph_types_t<ts_t>::node_t;
            using node_ptr_t = typename tchecker::covreach::details::graph_types_t<ts_t>::node_ptr_t;
            
            using node_allocator_t = typename zone_semantics_t::template state_pool_allocator_t<node_t>;
            using transition_allocator_t
            = typename zone_semantics_t::template transition_singleton_allocator_t<transition_t>;
            using ts_allocator_t = tchecker::ts::allocator_t<node_allocator_t, transition_allocator_t>;
            
            using graph_t = tchecker::covreach::graph_t<key_t, ts_t, ts_allocator_t>;
            
            static inline key_t node_to_key(node_ptr_t const & node)
            {
              return tchecker::ta::hash_value(*node);
            }
            
            class state_predicate_t {
            public:
              using node_ptr_t
              = typename tchecker::covreach::details::async_zg::ta::algorithm_model_t<ZONE_SEMANTICS>::node_ptr_t;
              
              bool operator() (node_ptr_t const & n1, node_ptr_t const & n2)
              {
                return (static_cast<tchecker::ta::state_t const &>(*n1)
                        == static_cast<tchecker::ta::state_t const &>(*n2));
              }
            };
            
            class node_lt_t {
            public:
              bool operator() (node_ptr_t const & n1, node_ptr_t const & n2) const
              {
                return tchecker::async_zg::lexical_cmp(*n1, *n2) < 0;
              }
            };
            
            static std::tuple<> state_predicate_args(model_t const & model)
            {
              return std::tuple<>();
            }
            
            static std::tuple<model_t const &> zone_predicate_args(model_t const & model)
            {
              return std::tuple<model_t const &>(model);
            }
            
            using node_outputter_t = tchecker::async_zg::ta::state_outputter_t;
            
            static std::tuple<tchecker::intvar_index_t const &, tchecker::clock_index_t const &, tchecker::clock_index_t const &>
            node_outputter_args(model_t const & model)
            {
              return
              std::tuple<tchecker::intvar_index_t const &, tchecker::clock_index_t const &, tchecker::clock_index_t const &>
              (model.flattened_integer_variables().index(),
               model.flattened_offset_clock_variables().index(),
               model.flattened_clock_variables().index());
            }
          };
          
        } // end of namespace ta
        
      } // end of namespace async_zg
      
      
      
      
      /*!
       \brief Run covering reachability algorithm
       \tparam ALGORITHM_MODEL : type of algorithm model
       \tparam GRAPH_OUTPUTTER : type of graph outputter
       \tparam WAITING : type of waiting container
       \param sysdecl : a system declaration
       \param options : covering reachability algorithm options
       \param log : logging facility
       \post covering reachability algorithm has been run on a model of sysdecl as defined by
       ALGORITHM_MODEL and following options and the exploreation policy implented by WAITING.
       The graph has been output using GRAPH_OUPUTTER
       Every error and warning has been reported to log.
       */
      template
      <template <class NODE_PTR, class STATE_PREDICATE> class COVER_NODE,
      class ALGORITHM_MODEL,
      template <class N, class E, class NO, class EO> class GRAPH_OUTPUTTER,
      template <class NPTR> class WAITING
      >
      void run(tchecker::parsing::system_declaration_t const & sysdecl,
               tchecker::covreach::options_t const & options,
               tchecker::log_t & log)
      {
        using model_t = typename ALGORITHM_MODEL::model_t;
        using ts_t = typename ALGORITHM_MODEL::ts_t;
        using graph_t = typename ALGORITHM_MODEL::graph_t;
        using node_ptr_t = typename ALGORITHM_MODEL::node_ptr_t;
        using state_predicate_t = typename ALGORITHM_MODEL::state_predicate_t;
        using cover_node_t = COVER_NODE<node_ptr_t, state_predicate_t>;
        
        model_t model(sysdecl, log);
        ts_t ts(model);
        cover_node_t cover_node(ALGORITHM_MODEL::state_predicate_args(model), ALGORITHM_MODEL::zone_predicate_args(model));
        
        tchecker::label_index_t label_index(model.system().labels());
        for (std::string const & label : options.accepting_labels()) {
          if (label_index.find_value(label) == label_index.end_value_map())
            label_index.add(label);
        }
        
        tchecker::covreach::accepting_labels_t<node_ptr_t> accepting_labels(label_index, options.accepting_labels());
        
        tchecker::gc_t gc;
        
        graph_t graph(gc,
                      std::tuple<tchecker::gc_t &, std::tuple<model_t &, std::size_t>, std::tuple<>>
                      (gc, std::tuple<model_t &, std::size_t>(model, options.block_size()), std::make_tuple()),
                      options.block_size(),
                      options.nodes_table_size(),
                      ALGORITHM_MODEL::node_to_key,
                      cover_node);
        
        gc.start();
        
        enum tchecker::covreach::outcome_t outcome;
        tchecker::covreach::stats_t stats;
        tchecker::covreach::algorithm_t<ts_t, graph_t, WAITING> algorithm;
        
        try {
          std::tie(outcome, stats) = algorithm.run(ts, graph, accepting_labels);
        }
        catch (...) {
          gc.stop();
          graph.clear();
          graph.free_all();
          throw;
        }
        
        std::cout << "REACHABLE " << (outcome == tchecker::covreach::REACHABLE ? "true" : "false") << std::endl;
        
        if (options.stats()) {
          std::cout << "STORED_NODES " << graph.nodes_count() << std::endl;
          std::cout << stats << std::endl;
        }
        
        if (options.output_format() == tchecker::covreach::options_t::DOT) {
          tchecker::covreach::dot_outputter_t<typename ALGORITHM_MODEL::node_outputter_t>
          dot_outputter(false, ALGORITHM_MODEL::node_outputter_args(model));
          
          dot_outputter.template output<graph_t, typename ALGORITHM_MODEL::node_lt_t>
          (options.output_stream(), graph, model.system().name());
        }
        
        gc.stop();
        graph.clear();
        graph.free_all();
      }
      
      
      /*!
       \brief Run covering reachability algorithm for asynchronous zone graphs
       \tparam ALGORITHM_MODEL : type of algorithm model
       \tparam GRAPH_OUTPUTTER : type of graph outputter
       \tparam WAITING : type of waiting container
       \param sysdecl : a system declaration
       \param options : covering reachability algorithm options
       \param log : logging facility
       \post covering reachability algorithm has been run on a model of sysdecl as defined by ALGORITHM_MODEL
       and following options and the exploreation policy implented by WAITING. The graps has
       been output using GRAPH_OUPUTTER
       Every error and warning has been reported to log.
       */
      template
      <class ALGORITHM_MODEL,
      template <class N, class E, class NO, class EO> class GRAPH_OUTPUTTER,
      template <class NPTR> class WAITING
      >
      void run_async_zg(tchecker::parsing::system_declaration_t const & sysdecl,
                        tchecker::covreach::options_t const & options,
                        tchecker::log_t & log)
      {
        if (options.node_covering() == tchecker::covreach::options_t::INCLUSION)
          tchecker::covreach::details::run<tchecker::covreach::cover_sync_inclusion_t, ALGORITHM_MODEL, GRAPH_OUTPUTTER, WAITING>
          (sysdecl, options, log);
        else
          log.error("Unsupported node covering");
      }
      
      
      /*!
       \brief Run covering reachability algorithm for zone graphs
       \tparam ALGORITHM_MODEL : type of algorithm model
       \tparam GRAPH_OUTPUTTER : type of graph outputter
       \tparam WAITING : type of waiting container
       \param sysdecl : a system declaration
       \param options : covering reachability algorithm options
       \param log : logging facility
       \post covering reachability algorithm has been run on a model of sysdecl as defined by ALGORITHM_MODEL
       and following options and the exploreation policy implented by WAITING. The graps has
       been output using GRAPH_OUPUTTER
       Every error and warning has been reported to log.
       */
      template
      <class ALGORITHM_MODEL,
      template <class N, class E, class NO, class EO> class GRAPH_OUTPUTTER,
      template <class NPTR> class WAITING
      >
      void run_zg(tchecker::parsing::system_declaration_t const & sysdecl,
                  tchecker::covreach::options_t const & options,
                  tchecker::log_t & log)
      {
        switch (options.node_covering()) {
          case tchecker::covreach::options_t::INCLUSION:
            tchecker::covreach::details::run<tchecker::covreach::cover_inclusion_t, ALGORITHM_MODEL, GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ALU_G:
            tchecker::covreach::details::run<tchecker::covreach::cover_alu_global_t, ALGORITHM_MODEL, GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ALU_L:
            tchecker::covreach::details::run<tchecker::covreach::cover_alu_local_t, ALGORITHM_MODEL, GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::AM_G:
            tchecker::covreach::details::run<tchecker::covreach::cover_am_global_t, ALGORITHM_MODEL, GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::AM_L:
            tchecker::covreach::details::run<tchecker::covreach::cover_am_local_t, ALGORITHM_MODEL, GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          default:
            log.error("unsupported node covering");
        }
      }
      
      
      /*!
       \brief Run covering reachability algorithm
       \tparam GRAPH_OUTPUTTER : type of graph outputter
       \tparam WAITING : type of waiting container
       \param sysdecl : a system declaration
       \param log : logging facility
       \param options : covering reachability algorithm options
       \post covering reachability algorithm has been run on a model of sysdecl following options and
       the exploration policy implemented by WAITING. The graph has been output using
       GRAPH_OUPUTTER
       Every error and warning has been reported to log.
       */
      template <template <class N, class E, class NO, class EO> class GRAPH_OUTPUTTER, template <class NPTR> class WAITING>
      void run(tchecker::parsing::system_declaration_t const & sysdecl,
               tchecker::covreach::options_t const & options,
               tchecker::log_t & log)
      {
        switch (options.algorithm_model()) {
          case tchecker::covreach::options_t::ASYNC_ZG_ELAPSED_EXTRALU_PLUS_L:
            tchecker::covreach::details::run_async_zg
            <tchecker::covreach::details::async_zg::ta::algorithm_model_t<tchecker::async_zg::ta::elapsed_extraLUplus_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ASYNC_ZG_NON_ELAPSED_EXTRALU_PLUS_L:
            tchecker::covreach::details::run_async_zg
            <tchecker::covreach::details::async_zg::ta::algorithm_model_t<tchecker::async_zg::ta::non_elapsed_extraLUplus_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_ELAPSED_NOEXTRA:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::elapsed_no_extrapolation_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_ELAPSED_EXTRAM_G:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::elapsed_extraM_global_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_ELAPSED_EXTRAM_L:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::elapsed_extraM_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_ELAPSED_EXTRAM_PLUS_G:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::elapsed_extraMplus_global_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_ELAPSED_EXTRAM_PLUS_L:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::elapsed_extraMplus_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_ELAPSED_EXTRALU_G:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::elapsed_extraLU_global_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_ELAPSED_EXTRALU_L:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::elapsed_extraLU_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_ELAPSED_EXTRALU_PLUS_G:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::elapsed_extraLUplus_global_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_ELAPSED_EXTRALU_PLUS_L:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::elapsed_extraLUplus_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_NON_ELAPSED_NOEXTRA:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::non_elapsed_no_extrapolation_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_NON_ELAPSED_EXTRAM_G:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::non_elapsed_extraM_global_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_NON_ELAPSED_EXTRAM_L:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::non_elapsed_extraM_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_NON_ELAPSED_EXTRAM_PLUS_G:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::non_elapsed_extraMplus_global_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_NON_ELAPSED_EXTRAM_PLUS_L:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::non_elapsed_extraMplus_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_NON_ELAPSED_EXTRALU_G:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::non_elapsed_extraLU_global_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_NON_ELAPSED_EXTRALU_L:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::non_elapsed_extraLU_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_NON_ELAPSED_EXTRALU_PLUS_G:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::non_elapsed_extraLUplus_global_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::ZG_NON_ELAPSED_EXTRALU_PLUS_L:
            tchecker::covreach::details::run_zg
            <tchecker::covreach::details::zg::ta::algorithm_model_t<tchecker::zg::ta::non_elapsed_extraLU_local_t>,
            GRAPH_OUTPUTTER, WAITING>
            (sysdecl, options, log);
            break;
          default:
            log.error("unsupported model");
        }
      }
      
      
      
      
      
      /*!
       \brief Run covering reachability algorithm
       \tparam WAITING : type of waiting container
       \param sysdecl : a system declaration
       \param options : covering reachability algorithm options
       \param log : logging facility
       \post covering reachability algorithm has been run on a model of sysdecl following options and
       the exploration policy implemented by WAITING
       Every error and warning has been reported to log.
       */
      template <template <class NPTR> class WAITING>
      void run(tchecker::parsing::system_declaration_t const & sysdecl,
               tchecker::covreach::options_t const & options,
               tchecker::log_t & log)
      {
        switch (options.output_format()) {
          case tchecker::covreach::options_t::DOT:
            tchecker::covreach::details::run<tchecker::graph::dot_outputter_t, WAITING>(sysdecl, options, log);
            break;
          case tchecker::covreach::options_t::RAW:
            tchecker::covreach::details::run<tchecker::graph::raw_outputter_t, WAITING>(sysdecl, options, log);
            break;
          default:
            log.error("unsupported output format");
        }
      }
      
    } // end of namespace details
    
    
    
    
    /*!
     \brief Run covering reachability algorithm
     \param sysdecl : a system declaration
     \param options : covering reachability algorithm options
     \param log : logging facility
     \post covering reachability algorithm has been run on a model of sysdecl following options.
     Every error and warning has been reported to log.
     */
    void run(tchecker::parsing::system_declaration_t const & sysdecl,
             tchecker::covreach::options_t const & options,
             tchecker::log_t & log);
    
  } // end of namespace covreach
  
} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COVREACH_RUN_HH
