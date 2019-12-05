/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/expression/typed_expression.hh"

namespace tchecker {
  
  /* enum expression_type_t */
  
  std::ostream & operator<< (std::ostream & os, enum tchecker::expression_type_t type)
  {
    switch (type) {
      case EXPR_TYPE_BAD:                 return os << "BAD";
      case EXPR_TYPE_INTTERM:             return os << "INTTERM";
      case EXPR_TYPE_INTVAR:              return os << "INTVAR";
      case EXPR_TYPE_INTARRAY:            return os << "INTARRAY";
      case EXPR_TYPE_LOCALINTVAR:         return os << "LOCALINTVAR";
      case EXPR_TYPE_LOCALINTARRAY:       return os << "LOCALINTARRAY";
      case EXPR_TYPE_INTLVALUE:           return os << "INTLVALUE";
      case EXPR_TYPE_CLKVAR:              return os << "CLKVAR";
      case EXPR_TYPE_CLKARRAY:            return os << "CLKARRAY";
      case EXPR_TYPE_CLKLVALUE:           return os << "CLKLVALUE";
      case EXPR_TYPE_CLKDIFF:             return os << "CLKDIFF";
      case EXPR_TYPE_INTCLKSUM:           return os << "INTCLKSUM";
      case EXPR_TYPE_ATOMIC_PREDICATE:    return os << "ATOMIC_PREDICATE";
      case EXPR_TYPE_CLKCONSTR_SIMPLE:    return os << "CLKCONSTR_SIMPLE";
      case EXPR_TYPE_CLKCONSTR_DIAGONAL:  return os << "CLKCONSTR_DIAGONAL";
      case EXPR_TYPE_CONJUNCTIVE_FORMULA: return os << "CONJUNCTIVE_FORMULA";
      default:                            throw std::runtime_error("incomplete switch statement");
    }
  }
  
  
  
  
  /* typed_expression_t */
  
  typed_expression_t::typed_expression_t(enum tchecker::expression_type_t type)
  : _type(type)
  {}
  
  
  
  void typed_expression_t::visit(tchecker::typed_expression_visitor_t & v) const
  {
    return this->do_visit(v);
  }
  
  
  
  
  /* typed_lvalue_expression_t */
  
  typed_lvalue_expression_t::typed_lvalue_expression_t(enum tchecker::expression_type_t type)
  : tchecker::typed_expression_t(type)
  {}
  
  
  
  
  /* typed_int_expression_t */
  
  tchecker::expression_t * typed_int_expression_t::do_clone() const
  {
    return new tchecker::typed_int_expression_t(_type, _value);
  }
  
  
  void typed_int_expression_t::do_visit(typed_expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  /* typed_var_expression_t */
  
  typed_var_expression_t::typed_var_expression_t(enum tchecker::expression_type_t type, std::string const & name,
                                                 tchecker::variable_id_t id, tchecker::variable_size_t size)
  : tchecker::make_typed_lvalue_expression_t<tchecker::var_expression_t>(type, name),
  _id(id),
  _size(size)
  {
    if (size < 1)
      throw std::invalid_argument("variable size should be >= 1");
  }
  
  
  tchecker::expression_t * typed_var_expression_t::do_clone() const
  {
    return new tchecker::typed_var_expression_t(_type, _name, _id, _size);
  }
  
  
  void typed_var_expression_t::do_visit(tchecker::typed_expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  /* typed_bounded_var_expression_t */
  
  typed_bounded_var_expression_t::typed_bounded_var_expression_t(enum tchecker::expression_type_t type, std::string const & name,
                                                                 tchecker::variable_id_t id, tchecker::variable_size_t size,
                                                                 tchecker::integer_t min, tchecker::integer_t max)
  : tchecker::typed_var_expression_t(type, name, id, size),
  _min(min),
  _max(max)
  {
    if (_min > _max)
      throw std::invalid_argument("variable min should be <= max");
  }
  
  
  tchecker::expression_t * typed_bounded_var_expression_t::do_clone() const
  {
    return new tchecker::typed_bounded_var_expression_t(_type, _name, _id, _size, _min, _max);
  }
  
  
  void typed_bounded_var_expression_t::do_visit(tchecker::typed_expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  /* typed_array_expression_t */
  
  typed_array_expression_t::typed_array_expression_t(enum tchecker::expression_type_t type,
                                                     tchecker::typed_var_expression_t const * variable,
                                                     tchecker::typed_expression_t const * offset)
  :tchecker::make_typed_lvalue_expression_t<tchecker::array_expression_t>(type, variable, offset)
  {}
  
  
  tchecker::expression_t * typed_array_expression_t::do_clone() const
  {
    auto const * variable_clone = dynamic_cast<tchecker::typed_var_expression_t const *>(_variable->clone());
    auto const * offset_clone = dynamic_cast<tchecker::typed_expression_t const *>(_offset->clone());
    return new tchecker::typed_array_expression_t(_type, variable_clone, offset_clone);
  }
  
  
  void typed_array_expression_t::do_visit(tchecker::typed_expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  /* typed_par_expression_t */
  
  tchecker::expression_t * typed_par_expression_t::do_clone() const
  {
    return new tchecker::typed_par_expression_t(_type, expr_clone());
  }
  
  
  void typed_par_expression_t::do_visit(tchecker::typed_expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  /* typed_binary_expression_t */
  
  typed_binary_expression_t::typed_binary_expression_t(enum tchecker::expression_type_t type,
                                                       enum tchecker::binary_operator_t op,
                                                       tchecker::typed_expression_t * left,
                                                       tchecker::typed_expression_t * right)
  : tchecker::make_typed_expression_t<tchecker::binary_expression_t>(type, op, left, right)
  {}
  
  
  tchecker::typed_expression_t const & typed_binary_expression_t::left_operand() const
  {
    return dynamic_cast<tchecker::typed_expression_t const &>
    (this->tchecker::make_typed_expression_t<tchecker::binary_expression_t>::left_operand());
  }
  
  
  tchecker::typed_expression_t const & typed_binary_expression_t::right_operand() const
  {
    return dynamic_cast<tchecker::typed_expression_t const &>
    (this->tchecker::make_typed_expression_t<tchecker::binary_expression_t>::right_operand());
  }
  
  
  tchecker::typed_expression_t * typed_binary_expression_t::left_operand_clone() const
  {
    return dynamic_cast<tchecker::typed_expression_t *>
    (this->tchecker::make_typed_expression_t<tchecker::binary_expression_t>::left_operand().clone());
  }
  
  
  tchecker::typed_expression_t * typed_binary_expression_t::right_operand_clone() const
  {
    return dynamic_cast<tchecker::typed_expression_t *>
    (this->tchecker::make_typed_expression_t<tchecker::binary_expression_t>::right_operand().clone());
  }
  
  
  tchecker::expression_t * typed_binary_expression_t::do_clone() const
  {
    return new tchecker::typed_binary_expression_t(_type, _op, left_operand_clone(), right_operand_clone());
  }
  
  
  void typed_binary_expression_t::do_visit(tchecker::typed_expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  /* typed_unary_expression_t */
  
  typed_unary_expression_t::typed_unary_expression_t(enum tchecker::expression_type_t type,
                                                     enum tchecker::unary_operator_t op,
                                                     tchecker::typed_expression_t * operand)
  : tchecker::make_typed_expression_t<tchecker::unary_expression_t>(type, op, operand)
  {}
  
  
  tchecker::typed_expression_t const & typed_unary_expression_t::operand() const
  {
    return dynamic_cast<tchecker::typed_expression_t const &>
    (this->tchecker::make_typed_expression_t<tchecker::unary_expression_t>::operand());
  }
  
  
  tchecker::typed_expression_t * typed_unary_expression_t::operand_clone() const
  {
    return dynamic_cast<tchecker::typed_expression_t *>
    (this->tchecker::make_typed_expression_t<tchecker::unary_expression_t>::operand().clone());
  }
  
  
  tchecker::expression_t * typed_unary_expression_t::do_clone() const
  {
    return new tchecker::typed_unary_expression_t(_type, _operator, operand_clone());
  }
  
  
  void typed_unary_expression_t::do_visit(tchecker::typed_expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  /* typed_simple_clkconstr_expression_t */
  
  typed_simple_clkconstr_expression_t::typed_simple_clkconstr_expression_t(enum tchecker::expression_type_t type,
                                                                           enum tchecker::binary_operator_t op,
                                                                           tchecker::typed_expression_t * left,
                                                                           tchecker::typed_expression_t * right)
  : tchecker::typed_binary_expression_t(type, op, left, right)
  {}
  
  
  void typed_simple_clkconstr_expression_t::do_visit(tchecker::typed_expression_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  /* typed_diagonal_clkconstr_expression_t */
  
  typed_diagonal_clkconstr_expression_t::typed_diagonal_clkconstr_expression_t(enum tchecker::expression_type_t type,
                                                                               enum tchecker::binary_operator_t op,
                                                                               tchecker::typed_expression_t * left,
                                                                               tchecker::typed_expression_t * right)
  : tchecker::typed_binary_expression_t(type, op, left, right)
  {}
  
  
  void typed_diagonal_clkconstr_expression_t::do_visit(tchecker::typed_expression_visitor_t & v) const
  {
    v.visit(*this);
  }

  /* typed_ite_expression_t */

  typed_ite_expression_t:: typed_ite_expression_t(tchecker::expression_type_t type,
                                                  tchecker::typed_expression_t * cond,
                                                  tchecker::typed_expression_t * then_value,
                                                  tchecker::typed_expression_t * else_value)
      : tchecker::make_typed_expression_t<tchecker::ite_expression_t>(type, cond, then_value, else_value)
  {}


  tchecker::typed_expression_t const & typed_ite_expression_t::condition () const
  {
    return dynamic_cast<tchecker::typed_expression_t const &>
    (this->tchecker::make_typed_expression_t<tchecker::ite_expression_t>::condition());
  }

  tchecker::typed_expression_t const & typed_ite_expression_t::then_value() const
  {
    return dynamic_cast<tchecker::typed_expression_t const &>
    (this->tchecker::make_typed_expression_t<tchecker::ite_expression_t>::then_value());
  }


  tchecker::typed_expression_t const & typed_ite_expression_t::else_value() const
  {
    return dynamic_cast<tchecker::typed_expression_t const &>
    (this->tchecker::make_typed_expression_t<tchecker::ite_expression_t>::else_value());
  }


  tchecker::typed_expression_t * typed_ite_expression_t::condition_clone() const
  {
    return dynamic_cast<tchecker::typed_expression_t *>
    (this->tchecker::make_typed_expression_t<tchecker::ite_expression_t>::condition().clone());
  }

  tchecker::typed_expression_t * typed_ite_expression_t::then_value_clone() const
  {
    return dynamic_cast<tchecker::typed_expression_t *>
    (this->tchecker::make_typed_expression_t<tchecker::ite_expression_t>::then_value().clone());
  }


  tchecker::typed_expression_t * typed_ite_expression_t::else_value_clone () const
  {
    return dynamic_cast<tchecker::typed_expression_t *>
    (this->tchecker::make_typed_expression_t<tchecker::ite_expression_t>::else_value().clone());
  }


  tchecker::expression_t * typed_ite_expression_t::do_clone() const
  {
    return new tchecker::typed_ite_expression_t(_type, condition_clone (),
                                                then_value_clone (),
                                                else_value_clone ());
  }


  void typed_ite_expression_t::do_visit(tchecker::typed_expression_visitor_t & v) const
  {
    v.visit(*this);
  }

} // end of namespace tchecker
