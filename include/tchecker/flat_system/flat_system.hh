/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FLAT_SYSTEM_HH
#define TCHECKER_FLAT_SYSTEM_HH

#include "tchecker/basictypes.hh"
#include "tchecker/flat_system/model.hh"
#include "tchecker/flat_system/synchronizer.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file flat_system.hh
 \brief Flat system
 */

namespace tchecker {
  
  namespace flat_system {
    
    /*!
     \class flat_system_t
     \tparam MODEL : type of model, should inherit from tchecker::flat_system::model_t
     \tparam VLOC : type of tuple of locations
     \brief Synchronized product of a system of processes
     */
    template <class MODEL, class VLOC>
    class flat_system_t {
      
      static_assert(std::is_base_of<tchecker::flat_system::model_t<typename MODEL::system_t>, MODEL>::value,
                    "MODEL should derive from tchecker::flat_system::model_t");
      
    public:
      /*!
       \brief Type of model
       */
      using model_t = MODEL;
      
      /*!
       \brief Type of tuple of locations
       */
      using vloc_t = VLOC;
      
      /*!
       \brief Constructor
       \param model : a model
       \note this keeps a reference on model
       */
      explicit flat_system_t(MODEL & model)
      : _model(model)
      {}
      
      /*!
       \brief Copy constructor
       */
      flat_system_t(tchecker::flat_system::flat_system_t<MODEL, VLOC> const &) = default;
      
      /*!
       \brief Move constructor
       */
      flat_system_t(tchecker::flat_system::flat_system_t<MODEL, VLOC> &&) = default;
      
      /*!
       \brief Destructor
       */
      ~flat_system_t() = default;
      
      /*!
       \brief Assignment operator
       */
      tchecker::flat_system::flat_system_t<MODEL, VLOC> &
      operator= (tchecker::flat_system::flat_system_t<MODEL, VLOC> const &) = default;
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::flat_system::flat_system_t<MODEL, VLOC> &
      operator= (tchecker::flat_system::flat_system_t<MODEL, VLOC> &&) = default;
      
      /*!
       \brief Type of iterator over initial states
       */
      using initial_iterator_t = tchecker::cartesian_iterator_t<typename MODEL::system_t::const_initial_loc_iterator_t>;
      
      /*!
       \brief Accessor
       \return range of initial states
       */
      tchecker::range_t<initial_iterator_t> initial() const
      {
        tchecker::process_index_t const & processes = _model.system().processes();
        auto procs_begin = processes.begin_key_map(), procs_end = processes.end_key_map();
        
        initial_iterator_t begin;
        for (auto it = procs_begin; it != procs_end; ++it)
          begin.push_back(_model.system().initial_locations(it->first));
        
        initial_iterator_t end(begin);
        end.to_end();
        
        return tchecker::make_range(begin, end);
      }
      
      /*!
       \brief Dereference type for iterator over initial states
       */
      using initial_iterator_value_t = typename std::iterator_traits<initial_iterator_t>::value_type;
      
      /*!
       \brief Initialize state
       \param vloc : tuple of locations
       \param initial_range : range of initial locations
       \pre the size of vloc is equal to the size of initial_range
       \post vloc has been initialized to the tuple of initial locations in initial_range
       \return STATE_OK
       \throw std::invalid_argument : if the precondition is not met
       */
      enum tchecker::state_status_t initialize(VLOC & vloc, initial_iterator_value_t const & initial_range) const
      {
        auto begin = initial_range.begin(), end = initial_range.end();
        tchecker::loc_id_t loc_id = 0;
        for (auto it = begin; it != end; ++it, ++loc_id) {
          if (loc_id >= vloc.size())
            throw std::invalid_argument("vloc and initial range have incompatible sizes");
          vloc[loc_id] = *it;
        }
        if (loc_id != vloc.size())
          throw std::invalid_argument("vloc and initial range have incompatible sizes");
        return tchecker::STATE_OK;
      }
      
      /*!
       \brief Type of iterator over outgoing edges
       */
      using outgoing_edges_iterator_t = typename tchecker::synchronizer_t<typename MODEL::system_t>::template iterator_t<VLOC>;
      
      /*!
       \brief Accessor
       \param vloc : tuple of locations
       \return range of outgoing synchronized and asynchronous edges from vloc
       */
      tchecker::range_t<outgoing_edges_iterator_t> outgoing_edges(VLOC const & vloc) const
      {
        return _model.synchronizer().outgoing_edges(vloc);
      }
      
      /*!
       \brief Type of iterator over a synchronized outgoing edge
       \note type dereferenced by outgoing_edges_iterator_t, corresponds to tchecker::vedge_iterator_t
       */
      using outgoing_edges_iterator_value_t = tchecker::range_t<typename outgoing_edges_iterator_t::vedge_iterator_t>;
      
      /*!
       \brief Compute next state
       \param vloc : tuple of locations
       \param vedge : range of edges in a vedge
       \pre the source location of edges in edges_range match the locations in vloc,
       no process has more than one edge in edges_range, and the pid of every process
       in vedge is less than the size of vloc
       \post the locations in vloc have been updated to target locations
       of edges for processes in vedge, and have been left unchanged for the other
       processes
       \return STATE_OK if the sources locations for edges in vedge match the
       locations in vloc, STATE_INCOMPATIBLE_EDGE otherwise
       \throw std::invalid_argument : if the pid of an edge in vedge is greater or
       equal to the size of vloc
       */
      enum tchecker::state_status_t next(VLOC & vloc, outgoing_edges_iterator_value_t const & vedge) const
      {
        auto begin = vedge.begin(), end = vedge.end();
        for (auto it = begin; it != end; ++it) {
          typename MODEL::system_t::edge_t const * edge = *it;
          if (edge->pid() >= vloc.size())
            throw std::invalid_argument("incompatible vloc and vedge");
          if (vloc[edge->pid()] != edge->src())
            return tchecker::STATE_INCOMPATIBLE_EDGE;
          vloc[edge->pid()] = edge->tgt();
        }
        return tchecker::STATE_OK;
      }
      
      /*!
       \brief Accessor
       \return Underlying model
       */
      inline constexpr MODEL const & model() const
      {
        return _model;
      }
    protected:
      MODEL & _model;
    };
    
  } // end of namespace flat_system
  
} // end of namespace tchecker

#endif // TCHECKER_FLAT_SYSTEM_HH
