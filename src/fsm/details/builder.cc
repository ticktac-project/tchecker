/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include "tchecker/fsm/details/builder.hh"
#include "tchecker/parsing/parsing.hh"


namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      // location_attributes_parser_t
      
      location_attributes_parser_t::location_attributes_parser_t(tchecker::log_t & log,
                                                                 bool & initial,
                                                                 tchecker::expression_t * & invariant,
                                                                 std::vector<std::string> & labels)
      : _log(log),
      _initial(initial),
      _invariant(invariant),
      _labels(labels)
      {
        _initial = false;
        assert( _invariant == nullptr );
        _invariant = nullptr;
        assert( _labels.empty() );
        _labels.clear();
      }
      
      
      void location_attributes_parser_t::parse(tchecker::parsing::attributes_t const & attributes)
      {
        tchecker::parsing::attributes_parser_t::parse(attributes);
        
        // true invariant if no invariant given
        if (_invariant == nullptr)
          _invariant = new tchecker::int_expression_t(1);
      }
      
      
      void location_attributes_parser_t::do_attr(tchecker::parsing::attr_t const & attr)
      {
        if (attr.key() == "initial")
          _initial = true;
        else if (attr.key() == "invariant") {
          // parse invariant expression
          _invariant = tchecker::parsing::parse_expression(attr.value_context() + " in attribute invariant", attr.value(), _log);
        }
        else if (attr.key() == "labels") {
          // parse comma-separated list of labels
          boost::tokenizer<boost::escaped_list_separator<char> > tokenizer(attr.value());
          auto end = tokenizer.end();
          for (auto it = tokenizer.begin(); it != end; ++it) {
            std::string label(*it);
            boost::trim(label);
            _labels.push_back(label);
          }
        }
        else
          _log.warning(attr.key_context(), "ignoring attribute " + attr.key());
      }
      
      
      
      
      // edge_attributes_parser_t
      
      edge_attributes_parser_t::edge_attributes_parser_t(tchecker::log_t & log,
                                                         tchecker::expression_t * & guard,
                                                         tchecker::statement_t * & statement)
      : _log(log),
      _guard(guard),
      _statement(statement)
      {
        assert( _guard == nullptr );
        _guard = nullptr;
        assert( _statement == nullptr );
        _statement = nullptr;
      }
      
      
      void edge_attributes_parser_t::parse(tchecker::parsing::attributes_t const & attributes)
      {
        tchecker::parsing::attributes_parser_t::parse(attributes);
        
        // true guard if no given guard
        if (_guard == nullptr)
          _guard = new tchecker::int_expression_t(1);
        
        // nop statement if no given statement
        if (_statement == nullptr)
          _statement = new tchecker::nop_statement_t();
      }
      
      
      void edge_attributes_parser_t::do_attr(tchecker::parsing::attr_t const & attr)
      {
        if (attr.key() == "provided") {
          // parse guard expression
          _guard = tchecker::parsing::parse_expression(attr.value_context() + " in attribute provided", attr.value(), _log);
        }
        else if (attr.key() == "do") {
          // parse do statement
          _statement = tchecker::parsing::parse_statement(attr.value_context() + " in attribute do", attr.value(), _log);
        }
        else
          _log.warning(attr.key_context(), "ignoring attribute " + attr.key());
      }
      
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

