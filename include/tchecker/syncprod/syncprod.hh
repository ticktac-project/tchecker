/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYNCPROD_SYNCPROD_HH
#define TCHECKER_SYNCPROD_SYNCPROD_HH

#include "tchecker/basictypes.hh"
#include "tchecker/syncprod/edges_iterators.hh"
#include "tchecker/syncprod/state.hh"
#include "tchecker/syncprod/system.hh"
#include "tchecker/syncprod/transition.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/ts/ts.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file syncprod.hh
 \brief Synchronised product of a system of processes
 */

namespace tchecker {
  
  namespace syncprod {
    
    /*!
     \brief Type of iterator over initial states
     */
    using initial_iterator_t = tchecker::cartesian_iterator_t<tchecker::system::locs_t::const_iterator_t>;
    
    
    /*!
     \brief Accessor to initial states
     \param system : a system
     \return range of initial states
     */
    tchecker::range_t<tchecker::syncprod::initial_iterator_t>
    initial(tchecker::syncprod::system_t const & system);
    
    
    /*!
     \brief Dereference type for iterator over initial states
     */
    using initial_iterator_value_t = std::iterator_traits<tchecker::syncprod::initial_iterator_t>::value_type;
    
    
    /*!
     \brief Initialize state
     \param system : a system
     \param vloc : tuple of locations
     \param vedge : tuple of edges
     \param initial_range : range of initial locations
     \pre the size of vloc and vedge is equal to the size of initial_range.
     initial_range has been obtained from system.
     initial_range yields the initial locations of all the processes ordered by increasing process identifier
     \post vloc has been initialized to the tuple of initial locations in initial_range.
     vedge has been initialized to an empty tuple of edges
     \return tchecker::STATE_OK
     \throw std::invalid_argument : if the size of vloc, vedge and initial_range do not coincide
     */
    enum tchecker::state_status_t
    initialize(tchecker::syncprod::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
               tchecker::syncprod::initial_iterator_value_t const & initial_range);
    
    
    /*!
     \brief Type of iterator over outgoing edges
     */
    using outgoing_edges_iterator_t = tchecker::syncprod::vloc_edges_iterator_t;
    
    
    /*!
     \brief Accessor to outgoing edges
     \param system : a system
     \param vloc : tuple of locations
     \return range of outgoing synchronized and asynchronous edges from vloc in system
     */
    tchecker::range_t<tchecker::syncprod::outgoing_edges_iterator_t>
    outgoing_edges(tchecker::syncprod::system_t const & system,
                   tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc);
    
    
    /*!
     \brief Type of iterator over an outgoing vedge
     \note type dereferenced by outgoing_edges_iterator_t, corresponds to tchecker::vedge_iterator_t
     */
    using outgoing_edges_iterator_value_t = tchecker::range_t<tchecker::syncprod::edges_iterator_t>;
    
    /*!
     \brief Compute next state
     \param vloc : tuple of locations
     \param vedge : tuple of edges
     \param edges : range of edges in a asynchronous/synchronized edge from vloc
     \pre the source locations of edges match the locations in vloc,
     no process has more than one edge in vedge,
     and the pid of every process in vedge is less than the size of vloc
     \post the locations in vloc have been updated to target locations of edges for processes involved in edges.
     They and have been left unchanged for the other processes.
     vedge contains the identifiers of the edges in edges
     \return tchecker::STATE_OK if the sources locations in edges match the locations in vloc,
     tchecker::STATE_INCOMPATIBLE_EDGE otherwise
     \throw std::invalid_argument : if the sizes of vloc and vedge do not match, or
     if the pid of an edge in edges is greater or equal to the size of vloc/vedge
     */
    enum tchecker::state_status_t next(tchecker::syncprod::system_t const & system,
                                       tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                       tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                       tchecker::syncprod::outgoing_edges_iterator_value_t const & edges);
    
    
    
    
    /*!
     \class syncprod_t
     \brief Synchronized product of timed processes
     */
    class syncprod_t final
    : public tchecker::ts::ts_t
    <tchecker::syncprod::state_t,
    tchecker::syncprod::transition_t,
    tchecker::syncprod::initial_iterator_t,
    tchecker::syncprod::outgoing_edges_iterator_t,
    tchecker::syncprod::initial_iterator_value_t,
    tchecker::syncprod::outgoing_edges_iterator_value_t>
    {
    public:
      /*!
       \brief Constructor
       \param system : a system of timed processes
       */
      syncprod_t(std::shared_ptr<tchecker::syncprod::system_t const> const & system);
      
      /*!
       \brief Copy constructor
       */
      syncprod_t(tchecker::syncprod::syncprod_t const &) = default;
      
      /*!
       \brief Move constructor
       */
      syncprod_t(tchecker::syncprod::syncprod_t &&) = default;
      
      /*!
       \brief Destructor
       */
      virtual ~syncprod_t() = default;
      
      /*!
       \brief Assignment operator
       */
      tchecker::syncprod::syncprod_t & operator= (tchecker::syncprod::syncprod_t const &) = default;
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::syncprod::syncprod_t & operator= (tchecker::syncprod::syncprod_t &&) = default;
            
      /*!
       \brief Accessor
       \return initial state iterators
       */
      virtual tchecker::range_t<tchecker::syncprod::initial_iterator_t> initial();
      
      /*!
       \brief Initialize state
       \param s : state
       \param t : transition
       \param v : initial iterator value
       \post s has been initialized from v, and t is an empty transition
       \return status of state s after initialization
       */
      virtual enum tchecker::state_status_t initialize(tchecker::syncprod::state_t & s,
                                                       tchecker::syncprod::transition_t & t,
                                                       tchecker::syncprod::initial_iterator_value_t const & v);
      
      /*!
       \brief Accessor
       \param s : state
       \return outgoing edges from state s
       */
      virtual tchecker::range_t<tchecker::syncprod::outgoing_edges_iterator_t>
      outgoing_edges(tchecker::syncprod::state_t const & s);
      
      /*!
       \brief Next state computation
       \param s : state
       \param t : transition
       \param v : outgoing edge value
       \post s have been updated from v, and t is the set of edges in v
       \return status of state s after update
       */
      virtual enum tchecker::state_status_t next(tchecker::syncprod::state_t & s,
                                                 tchecker::syncprod::transition_t & t,
                                                 tchecker::syncprod::outgoing_edges_iterator_value_t const & v);
      
      
      /*!
       \brief Accessor
       \return Underlying system of timed processes
       */
      tchecker::syncprod::system_t const & system() const;
    private:
      std::shared_ptr<tchecker::syncprod::system_t const> _system; /*!< System of timed processes */
    };
    
  } // end of namespace syncprod
  
} // end of namespace tchecker

#endif // TCHECKER_SYNCPROD_SYNCPROD_HH
