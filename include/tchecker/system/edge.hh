/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_EDGE_HH
#define TCHECKER_SYSTEM_EDGE_HH

#include "tchecker/basictypes.hh"

/*!
 \file edge.hh
 \brief System edges
 */

namespace tchecker {
  
  // forward declaration
  template <class L, class E> class system_t;
  
  
  
  /*!
   \class edge_t
   \tparam LOC : type of location, should derive from tchecker::loc_t
   \brief An edge
   */
  template <class LOC>
  class edge_t {
  public:
    /*!
     \brief Type of location
     */
    using loc_t = LOC;
    
    /*!
     \brief Constructor
     \param pid : process identifier
     \param id : identifier
     \param src : source location
     \param tgt : target location
     \param event_id :  event identifier
     \pre src!=nullptr, tgt!=nullptr, and src and tgt belong to process pid
     \throw std::invalid_argument : if the precondition is violated
     */
    edge_t(tchecker::process_id_t pid, tchecker::edge_id_t id, LOC const * src, LOC const * tgt, tchecker::event_id_t event_id)
    : _pid(pid), _id(id), _src(src), _tgt(tgt), _event_id(event_id)
    {
      if (_src == nullptr)
        throw std::invalid_argument("nullptr source location");
      
      if (_tgt == nullptr)
        throw std::invalid_argument("nullptr target location");
      
      if (_src->pid() != pid)
        throw std::invalid_argument("bad source location");
      
      if (_tgt->pid() != pid)
        throw std::invalid_argument("bad target location");
    }
    
    /*!
     \brief Copy constructor
     \param e : edge
     \post this is a copy of e
     */
    edge_t(tchecker::edge_t<LOC> const & e) = default;
    // NB: _src and _tgt are not cloned (references)
    
    /*!
     \brief Move constructor
     \param e : edge
     \post e has been moved to this
     */
    edge_t(tchecker::edge_t<LOC> && e) = default;
    
    /*!
     \brief Destructor
     */
    ~edge_t() = default;
    // NB: do not delete _src and _tgt (references)
    
    /*!
     \brief Assignement operator
     \param e : edge
     \post this is a copy of e
     */
    tchecker::edge_t<LOC> & operator = (tchecker::edge_t<LOC> const & e) = default;
    // NB: _src and _tgt are not cloned (references)
    
    /*!
     \brief Move assignement operator
     \param e : edge
     \post this is a copy of e
     */
    tchecker::edge_t<LOC> & operator = (tchecker::edge_t<LOC> && e) = default;
    
    /*!
     \brief Accessor
     \return process identifier
     */
    inline tchecker::process_id_t pid() const
    {
      return _pid;
    }
    
    /*!
     \brief Accessor
     \return Identifier
     */
    inline tchecker::edge_id_t id() const
    {
      return _id;
    }
    
    /*!
     \brief Accessor
     \return Source location
     */
    inline LOC const * src() const
    {
      return _src;
    }
    
    /*!
     \brief Accessor
     \return Target location
     */
    inline LOC const * tgt() const
    {
      return _tgt;
    }
    
    /*!
     \brief Accessor
     \return Event
     */
    inline tchecker::event_id_t event_id() const
    {
      return _event_id;
    }
  private:
    template <class L, class E> friend class tchecker::system_t;
    
    /*!
     \brief Reset edge source location
     \param new_src : new source location
     \pre new_src != nullptr
     \post this edge has source location new_src
     \throw std::invalid_argument : if new_src is nullptr
     */
    inline void reset_src(LOC const * new_src)
    {
      if (new_src == nullptr)
        throw std::invalid_argument("nullptr source location");
      _src = new_src;
    }
    
    /*!
     \brief Reset edge target location
     \param new_tgt : new target location
     \pre new_tgt != nullptr
     \post this edge has target location new_tgt
     \throw std::invalid_argument : if new_tgt is nullptr
     */
    inline void reset_tgt(LOC const * new_tgt)
    {
      if (new_tgt == nullptr)
        throw std::invalid_argument("nullptr target location");
      _tgt = new_tgt;
    }
    
    tchecker::process_id_t _pid;                 /*!< Process ID */
    tchecker::edge_id_t _id;                     /*!< Identifier */
    LOC const * _src;                            /*!< Source location */
    LOC const * _tgt;                            /*!< Target location */
    tchecker::event_id_t _event_id;              /*!< Event ID */
  };
  
} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_EDGE_HH
