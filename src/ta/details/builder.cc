/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/ta/details/builder.hh"

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      // location_attributes_parser_t
      
      location_attributes_parser_t::location_attributes_parser_t(tchecker::log_t & log, bool & committed, bool & initial,
                                                                 bool & urgent, tchecker::expression_t * & invariant,
                                                                 std::vector<std::string> & labels)
      : tchecker::fsm::details::location_attributes_parser_t(log, initial, invariant, labels),
      _committed(committed),
      _urgent(urgent)
      {
        _committed = false;
        _urgent = false;
      }
      
      
      void location_attributes_parser_t::do_attr(tchecker::parsing::attr_t const & attr)
      {
        if(attr.key() == "committed")
          _committed = true;
        else if (attr.key() == "urgent")
          _urgent = true;
        else
          tchecker::fsm::details::location_attributes_parser_t::do_attr(attr);
      }
      
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

