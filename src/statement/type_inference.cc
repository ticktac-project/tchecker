/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/expression/type_inference.hh"
#include "tchecker/statement/type_inference.hh"

namespace tchecker {
  
  enum tchecker::statement_type_t type_assign(enum tchecker::expression_type_t ltype, enum tchecker::expression_type_t rtype)
  {
    if (integer_assignable(ltype) && integer_valued(rtype))
      return tchecker::STMT_TYPE_INTASSIGN;
    
    if (clock_assignable(ltype) && integer_valued(rtype))
      return tchecker::STMT_TYPE_CLKASSIGN_INT;
    
    if (clock_assignable(ltype) && clock_valued(rtype))
      return tchecker::STMT_TYPE_CLKASSIGN_CLK;
    
    if (clock_assignable(ltype) && (rtype == tchecker::EXPR_TYPE_INTCLKSUM))
      return tchecker::STMT_TYPE_CLKASSIGN_SUM;
    
    return tchecker::STMT_TYPE_BAD;
  }
  
  
  enum tchecker::statement_type_t type_seq(enum tchecker::statement_type_t first, enum tchecker::statement_type_t second)
  {
    if ((first != tchecker::STMT_TYPE_BAD) && (second != tchecker::STMT_TYPE_BAD))
      return tchecker::STMT_TYPE_SEQ;
    
    return tchecker::STMT_TYPE_BAD;
  }
  
} // end of namespace tchecker
