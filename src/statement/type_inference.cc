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

  enum tchecker::statement_type_t type_if(enum tchecker::expression_type_t cond,
                                           enum tchecker::statement_type_t then_stmt,
                                           enum tchecker::statement_type_t else_stmt)
  {
    if (tchecker::bool_valued(cond) &&
        (then_stmt!= tchecker::STMT_TYPE_BAD) &&
        (else_stmt!= tchecker::STMT_TYPE_BAD)
        )
      return tchecker::STMT_TYPE_IF;

    return tchecker::STMT_TYPE_BAD;
  }

  enum tchecker::statement_type_t type_while(enum tchecker::expression_type_t cond,
                                             enum tchecker::statement_type_t stmt)
  {
    if (tchecker::bool_valued(cond) && (stmt!= tchecker::STMT_TYPE_BAD))
      return tchecker::STMT_TYPE_WHILE;

    return tchecker::STMT_TYPE_BAD;
  }

  enum tchecker::statement_type_t type_local(enum tchecker::expression_type_t variable,
                                             enum tchecker::expression_type_t init)
  {
    if (tchecker::integer_valued (init) && variable == tchecker::EXPR_TYPE_LOCALINTVAR)
      return tchecker::STMT_TYPE_LOCAL_INT;
    return tchecker::STMT_TYPE_BAD;
  }

  enum tchecker::statement_type_t type_local_array(enum tchecker::expression_type_t variable,
                                                   enum tchecker::expression_type_t size)
  {
    if (tchecker::integer_valued (size) && variable == tchecker::EXPR_TYPE_LOCALINTARRAY)
      return tchecker::STMT_TYPE_LOCAL_ARRAY;
    return tchecker::STMT_TYPE_BAD;
  }

} // end of namespace tchecker
