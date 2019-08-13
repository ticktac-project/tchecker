/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/fsm/fsm.hh"

namespace tchecker {
  
  namespace fsm {
    
    /* model_t */
    
    model_t::model_t(tchecker::parsing::system_declaration_t const & system_declaration, tchecker::log_t & log)
    : tchecker::fsm::details::model_t<tchecker::fsm::system_t, tchecker::fsm::variables_t>
    (tchecker::fsm::build_system(system_declaration, log), log)
    {}
    
    
    
    /* fsm_t */
    
    /*!
     \class null_container_t
     \brief A container with a silent method push_back
     */
    template <class CONTAINER>
    class null_container_t : public CONTAINER {
    public:
      template <class T>
      void push_back(T && t) const {}
    };
    
    static null_container_t<tchecker::clock_constraint_container_t> null_clkconstr;  /*!< null clock constraint container */
    static null_container_t<tchecker::clock_reset_container_t> null_clkreset;        /*!< null clock reset container */
    
    
    
    enum tchecker::state_status_t fsm_t::initialize(tchecker::fsm::vloc_t & vloc,
                                                    tchecker::intvars_valuation_t & intvars_val,
                                                    initial_iterator_value_t const & initial_range)
    {
      return tchecker::fsm::fsm_instantiation_t::initialize(vloc, intvars_val, initial_range, null_clkconstr);
    }
    
    
    enum tchecker::state_status_t fsm_t::next(tchecker::fsm::vloc_t & vloc,
                                              tchecker::intvars_valuation_t & intvars_val,
                                              outgoing_edges_iterator_value_t const & vedge)
    {
      return tchecker::fsm::fsm_instantiation_t::next(vloc, intvars_val, vedge, null_clkconstr, null_clkconstr, null_clkreset,
                                                      null_clkconstr);
    }
    
  } // end of namespace fsm
  
} // end of namespace tchecker
