/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <exception>
#include <sstream>

#include "tchecker/statement/statement.hh"

namespace tchecker {
  
  // statement_t
  
  tchecker::statement_t * statement_t::clone() const
  {
    return this->do_clone();
  }
  
  
  void statement_t::visit(tchecker::statement_visitor_t & v) const
  {
    this->do_visit(v);
  }
  
  
  std::string statement_t::to_string() const
  {
    std::stringstream s;
    this->do_output(s);
    return s.str();
  }
  
  
  std::ostream & operator<< (std::ostream & os, tchecker::statement_t const & stmt)
  {
    return stmt.do_output(os);
  }
  
  
  
  
  // nop_statement_t
  
  nop_statement_t::nop_statement_t() = default;
  
  
  nop_statement_t::~nop_statement_t() = default;
  
  
  std::ostream & nop_statement_t::do_output(std::ostream & os) const
  {
    return os << "nop";
  }
  
  
  tchecker::statement_t * nop_statement_t::do_clone() const
  {
    return new tchecker::nop_statement_t();
  }
  
  
  void
  nop_statement_t::do_visit(tchecker::statement_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  
  // assign_statement_t
  
  
  assign_statement_t::assign_statement_t(tchecker::lvalue_expression_t const * lvalue, tchecker::expression_t const * rvalue)
  : _lvalue(lvalue), _rvalue(rvalue)
  {
    if (_lvalue == nullptr)
      throw std::invalid_argument("nullptr lvalue");
    if (_rvalue == nullptr)
      throw std::invalid_argument("nullptr rvalue");
  }
  
  
  
  assign_statement_t::~assign_statement_t()
  {
    delete _lvalue;
    delete _rvalue;
  }
  
  
  
  std::ostream & assign_statement_t::do_output(std::ostream & os) const
  {
    return os << *_lvalue << "=" << *_rvalue;
  }
  
  
  
  tchecker::statement_t * assign_statement_t::do_clone() const
  {
    auto * lvalue = dynamic_cast<tchecker::lvalue_expression_t*>(_lvalue->clone());
    return new tchecker::assign_statement_t(lvalue, _rvalue->clone());
  }
  
  
  
  void assign_statement_t::do_visit(tchecker::statement_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  // sequence_statement_t
  
  sequence_statement_t::sequence_statement_t(tchecker::statement_t const * first, tchecker::statement_t const * second)
  : _first(first), _second(second)
  {
    if (first == nullptr)
      throw std::invalid_argument("nullptr first statement");
    if (second == nullptr)
      throw std::invalid_argument("nullptr second statement");
  }
  
  
  
  sequence_statement_t::~sequence_statement_t()
  {
    delete _first;
    delete _second;
  }
  
  
  
  std::ostream & sequence_statement_t::do_output(std::ostream & os) const
  {
    return os << *_first << "; " << *_second;
  }
  
  
  
  tchecker::statement_t * sequence_statement_t::do_clone() const
  {
    return new tchecker::sequence_statement_t(_first->clone(), _second->clone());
  }
  
  
  
  void sequence_statement_t::do_visit(tchecker::statement_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  // if_statement_t

  if_statement_t::if_statement_t(tchecker::expression_t const *cond,
                                 tchecker::statement_t const * then_stmt,
                                 tchecker::statement_t const * else_stmt)
  : _condition(cond), _then_stmt(then_stmt), _else_stmt(else_stmt)
  {
    if (cond == nullptr)
      throw std::invalid_argument("nullptr condition");
    if (then_stmt == nullptr)
      throw std::invalid_argument("nullptr then statement");
    if (else_stmt == nullptr)
      throw std::invalid_argument("nullptr else statement");
  }



  if_statement_t::~if_statement_t()
  {
    delete _condition;
    delete _then_stmt;
    delete _else_stmt;
  }



  std::ostream & if_statement_t::do_output(std::ostream & os) const
  {
    return os << "if " << _condition
              << " then " << *_then_stmt
              << " else " << *_else_stmt;
  }



  tchecker::statement_t * if_statement_t::do_clone() const
  {
    return new tchecker::if_statement_t(_condition->clone(),
                                        _then_stmt->clone(),
                                        _else_stmt->clone());
  }



  void if_statement_t::do_visit(tchecker::statement_visitor_t & v) const
  {
    v.visit(*this);
  }


} // end of namespace tchecker
