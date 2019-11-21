/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <sstream>

#include "tchecker/expression/expression.hh"

namespace tchecker {
  
  // expression_t
  
  tchecker::expression_t * expression_t::clone() const
  {
    return this->do_clone();
  }
  
  
  void expression_t::visit(tchecker::expression_visitor_t & v) const
  {
    this->do_visit(v);
  }
  
  
  std::string expression_t::to_string() const
  {
    std::stringstream s;
    this->do_output(s);
    return s.str();
  }
  
  
  std::ostream & operator<< (std::ostream & os, tchecker::expression_t const & expr)
  {
    return expr.do_output(os);
  }
  
  
  
  // int_expression_t
  
  int_expression_t::int_expression_t(tchecker::integer_t value)
  : _value(value)
  {}
  
  
  std::ostream & int_expression_t::do_output(std::ostream & os) const
  {
    return os << _value;
  }
  
  
  tchecker::expression_t * int_expression_t::do_clone() const
  {
    return (new tchecker::int_expression_t(_value));
  }
  
  
  void int_expression_t::do_visit(tchecker::expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  // var_expression_t
  
  var_expression_t::var_expression_t(std::string const & name)
  : _name(name)
  {
    if (_name.empty())
      throw std::invalid_argument("empty variable name");
  }
  
  
  std::ostream & var_expression_t::do_output(std::ostream & os) const
  {
    return os << _name;
  }
  
  
  tchecker::expression_t * var_expression_t::do_clone() const
  {
    return (new tchecker::var_expression_t(_name));
  }
  
  
  void var_expression_t::do_visit(tchecker::expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  // array_expression_t
  
  array_expression_t::array_expression_t(tchecker::var_expression_t const * variable,
                                         tchecker::expression_t const * offset)
  : _variable(variable), _offset(offset)
  {
    if (_offset == nullptr)
      throw std::invalid_argument("nullptr offset");
    if (_variable == nullptr)
      throw std::invalid_argument("nullptr variable");
  }
  
  
  array_expression_t::~array_expression_t()
  {
    delete _variable;
    delete _offset;
  }
  
  
  std::ostream & array_expression_t::do_output(std::ostream & os) const
  {
    return os << *_variable << "[" << *_offset << "]";
  }
  
  
  tchecker::expression_t * array_expression_t::do_clone() const
  {
    auto * variable_clone = dynamic_cast<tchecker::var_expression_t *>(_variable->clone());
    return new tchecker::array_expression_t(variable_clone, _offset->clone());
  }
  
  
  void array_expression_t::do_visit(tchecker::expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  // par_expression_t
  
  par_expression_t::par_expression_t(tchecker::expression_t const * expr)
  : _expr(expr)
  {}
  
  
  par_expression_t::~par_expression_t()
  {
    delete _expr;
  }
  
  
  std::ostream & par_expression_t::do_output(std::ostream & os) const
  {
    return os << "(" << * _expr << ")";
  }
  
  
  tchecker::expression_t * par_expression_t::do_clone() const
  {
    return (new tchecker::par_expression_t(_expr->clone()));
  }
  
  
  void par_expression_t::do_visit(tchecker::expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  // unary_operator_t
  
  std::ostream & operator<< (std::ostream & os, enum unary_operator_t op)
  {
    switch (op) {
      case tchecker::EXPR_OP_NEG:   return os << "-";
      case tchecker::EXPR_OP_LNOT:  return os << "!";
      default: throw std::runtime_error("incomplete switch statement");
    }
  }
  
  
  
  
  // unary_expression_t
  
  unary_expression_t::unary_expression_t(enum unary_operator_t op, tchecker::expression_t const * expr)
  : _operator(op), _expr(expr)
  {
    if (_expr == nullptr)
      throw std::invalid_argument("nullptr expression");
  }
  
  
  unary_expression_t::~unary_expression_t()
  {
    delete _expr;
  }
  
  
  std::ostream & unary_expression_t::do_output(std::ostream & os) const
  {
    return os << _operator << * _expr;
  }
  
  
  tchecker::expression_t * unary_expression_t::do_clone() const
  {
    return new tchecker::unary_expression_t(_operator, _expr->clone());
  }
  
  
  void unary_expression_t::do_visit(tchecker::expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  // binary_operator_t
  
  std::ostream & operator<< (std::ostream & os, enum binary_operator_t op)
  {
    switch (op) {
      case tchecker::EXPR_OP_LAND:   return os << "&&";
      case tchecker::EXPR_OP_LT:     return os << "<";
      case tchecker::EXPR_OP_LE:     return os << "<=";
      case tchecker::EXPR_OP_EQ:     return os << "==";
      case tchecker::EXPR_OP_NEQ:    return os << "!=";
      case tchecker::EXPR_OP_GE:     return os << ">=";
      case tchecker::EXPR_OP_GT:     return os << ">";
      case tchecker::EXPR_OP_MINUS:  return os << "-";
      case tchecker::EXPR_OP_PLUS:   return os << "+";
      case tchecker::EXPR_OP_TIMES:  return os << "*";
      case tchecker::EXPR_OP_DIV:    return os << "/";
      case tchecker::EXPR_OP_MOD:    return os << "%";
      default: throw std::runtime_error("incomplete switch statement");
    }
  }
  
  
  bool predicate(enum tchecker::binary_operator_t op)
  {
    return ((op == tchecker::EXPR_OP_LT) ||
            (op == tchecker::EXPR_OP_LE) ||
            (op == tchecker::EXPR_OP_EQ) ||
            (op == tchecker::EXPR_OP_NEQ) ||
            (op == tchecker::EXPR_OP_GE) ||
            (op == tchecker::EXPR_OP_GT));
  }
  
  
  
  // binary_expression_t
  
  binary_expression_t::binary_expression_t(enum binary_operator_t op,
                                           tchecker::expression_t const * left,
                                           tchecker::expression_t const * right)
  :	_op(op), _left(left), _right(right)
  {
    if (_left == nullptr)
      throw std::invalid_argument("nullptr left expression");
    if (_right == nullptr)
      throw std::invalid_argument("nullptr right expression");
  }
  
  
  binary_expression_t::~binary_expression_t()
  {
    delete _left;
    delete _right;
  }
  
  
  std::ostream & binary_expression_t::do_output(std::ostream & os) const
  {
    return os << * _left << _op << * _right;
  }
  
  
  tchecker::expression_t * binary_expression_t::do_clone() const
  {
    return new tchecker::binary_expression_t(_op, _left->clone(), _right->clone());
  }
  
  
  void binary_expression_t::do_visit(tchecker::expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  // ite_expression_t

  ite_expression_t::ite_expression_t(tchecker::expression_t const * condition,
                                     tchecker::expression_t const * then_value,
                                     tchecker::expression_t const * else_value)
  :	_condition(condition), _then_value(then_value), _else_value(else_value)
  {
    if (_condition == nullptr)
      throw std::invalid_argument("nullptr ite_condition expression");
    if (_then_value == nullptr)
      throw std::invalid_argument("nullptr then_value expression");
    if (_else_value == nullptr)
      throw std::invalid_argument("nullptr else_value expression");
  }


  ite_expression_t::~ite_expression_t()
  {
    delete _condition;
    delete _then_value;
    delete _else_value;
  }


  std::ostream & ite_expression_t::do_output(std::ostream & os) const
  {
    return os << "if " << condition ()
              << " then " << then_value ()
              << " else " << else_value ();
  }


  tchecker::expression_t * ite_expression_t::do_clone() const
  {
    return new tchecker::ite_expression_t(_condition->clone (),
                                          _then_value->clone(),
                                          _else_value->clone());
  }


  void ite_expression_t::do_visit(tchecker::expression_visitor_t & v) const
  {
    v.visit(*this);
  }

} // end of namespace tchecker
