/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_BUILDER_HH
#define TCHECKER_SYSTEM_BUILDER_HH

#include <functional>
#include <string>
#include <tuple>

#include <boost/iterator/transform_iterator.hpp>

#include "tchecker/parsing/declaration.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/log.hh"


/*!
 \file builder.hh
 \brief System builder
 */

namespace tchecker {
  
  /*!
   \class system_builder_t
   \tparam SYSTEM : type of system, should derive from tchecker::system_t
   and have constructor SYSTEM(std::string const &)
   \brief Fills system from declarations
   \note this class is virtual since integer variable, clock, location and
   edge declaration visitors are not defined
   */
  template <class SYSTEM>
  class system_builder_t : public tchecker::parsing::declaration_visitor_t {
  public:
    /*!
     \brief Constructor
     */
    system_builder_t(tchecker::log_t & log)
    : _system(nullptr), _log(log)
    {}
    
    /*!
     \brief Copy constructor
     \param sb : system builder
     \post this is a copy of sb
     */
    system_builder_t(tchecker::system_builder_t<SYSTEM> const & sb)
    : _log(sb._log)
    {
      _system = new SYSTEM(*sb._system);
    }
    
    /*!
     \brief Move constructor
     \param sb : system builder
     \post sb has been moved to this
     */
    system_builder_t(tchecker::system_builder_t<SYSTEM> && sb) = default;
    
    /*!
     \brief Destructor
     */
    virtual ~system_builder_t() = default;
    
    /*!
     \brief Assignment operator
     \param sb : system builder
     \post this is a copy of sb
     */
    tchecker::system_builder_t<SYSTEM> & operator= (tchecker::system_builder_t<SYSTEM> const & sb)
    {
      if (this != &sb) {
        delete _system;
        _system = new SYSTEM(*sb._system);
        _log = sb._log;
      }
      return *this;
    }
    
    /*!
     \brief Move assignment operator
     \param sb : system builder
     \post sb has been moved to this
     */
    tchecker::system_builder_t<SYSTEM> & operator= (tchecker::system_builder_t<SYSTEM> && sb) = default;
    
    /*!
     \brief Accessor
     \return built system (may be nullptr)
     \note this releases ownership on system
     */
    inline SYSTEM * release()
    {
      auto r = _system;
      _system = nullptr;
      return r;
    }
    
    /*!
     \brief Build system and visit nested declarations
     */
    virtual void visit(tchecker::parsing::system_declaration_t const & d)
    {
      try {
        _system = new SYSTEM(d.name());
      }
      catch (std::exception const & e) {
        _log.error("", e.what());
      }
      
      try {
        for (auto const * decl : d.declarations())
          decl->visit(*this);
      }
      catch (...) {
        delete _system;
        throw;
      }
    }
    
    /*!
     \brief Add process
     */
    virtual void visit(tchecker::parsing::process_declaration_t const & d)
    {
      try {
        _system->add_process(d.name());
      }
      catch (std::exception const & e) {
        _log.error("process " + d.name(), e.what());
        throw;
      }
    }
    
    /*!
     \brief Add event
     */
    virtual void visit(tchecker::parsing::event_declaration_t const & d)
    {
      try {
        _system->add_event(d.name());
      }
      catch (std::exception const & e) {
        _log.error("event " + d.name(), e.what());
        throw;
      }
    }
    
    // add synchronization
  protected:
    /*!
     \brief Transformed synchronization
     */
    using str_tuple_t = std::tuple<std::string, std::string, enum tchecker::sync_strength_t>;
    
    /*!
     \brief Type of proxy for const transformation iterator on synchronizations
     */
    using const_sync_trans_iterator_proxy_t = std::function<str_tuple_t(tchecker::parsing::sync_constraint_t const *)>;
    
    /*!
     \brief Proxy for const transformation iterator over synchronizations
     */
    static str_tuple_t const_sync_trans_iterator_proxy(tchecker::parsing::sync_constraint_t const * c)
    {
      return std::make_tuple(c->process().name(), c->event().name(), c->strength());
    }
  public:
    /*!
     \brief Add synchronization
     */
    virtual void visit(tchecker::parsing::sync_declaration_t const & d)
    {
      auto syncs = d.sync_constraints();
      
      auto proxy = &tchecker::system_builder_t<SYSTEM>::const_sync_trans_iterator_proxy;
      
      boost::transform_iterator<const_sync_trans_iterator_proxy_t, tchecker::parsing::sync_declaration_t::const_iterator_t>
      begin(syncs.begin(), proxy), end(syncs.end(), proxy);
      
      _system->add_synchronization(tchecker::make_range(begin, end));
    }
  protected:
    SYSTEM * _system;                /*!< System */
    tchecker::log_t & _log;          /*!< Logging facility */
  };
  
  
} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_BUILDER_HH
