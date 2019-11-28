/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <tchecker/expression/type_inference.hh>
#include "tchecker/expression/typechecking.hh"
#include "tchecker/statement/type_inference.hh"
#include "tchecker/statement/typechecking.hh"

namespace tchecker {
  
  namespace details {
    
    /*!
     \class statement_typechecker_t
     \brief Statement typechecking visitor
     */
    class statement_typechecker_t : public tchecker::statement_visitor_t {
    public:
      /*!
       \brief Constructor
       \param intvars : integer variables
       \param clocks : clock variables
       \param error : error logging function
       */
      statement_typechecker_t(tchecker::integer_variables_t const & intvars,
                              tchecker::clock_variables_t const & clocks,
                              std::function<void(std::string const &)> error)
      : _typed_stmt(nullptr),
      _intvars(intvars),
      _clocks(clocks),
      _error(error)
      {}
      
      
      /*!
       \brief Copy constructor (DELETED)
       */
      statement_typechecker_t(tchecker::details::statement_typechecker_t const &) = delete;
      
      
      /*!
       \brief Move constructor (DELETED)
       */
      statement_typechecker_t(tchecker::details::statement_typechecker_t &&) = delete;
      
      
      /*!
       \brief Destructor
       */
      virtual ~statement_typechecker_t()
      {
        delete _typed_stmt;
      }
      
      
      /*!
       \brief Assignment operator (DELETED)
       */
      tchecker::details::statement_typechecker_t & operator= (tchecker::details::statement_typechecker_t const &) = delete;
      
      
      /*!
       \brief Move assignment operator (DELETED)
       */
      tchecker::details::statement_typechecker_t & operator= (tchecker::details::statement_typechecker_t &&) = delete;
      
      
      /*!
       \brief Accessor
       \return typed statement
       \note this releases ownership on typed statement
       */
      tchecker::typed_statement_t * release()
      {
        auto p = _typed_stmt;
        _typed_stmt = nullptr;
        return p;
      }
      
      
      /*!
       \brief Visitor
       \param stmt : statement
       \post _type_stmt points to a typed clone of this
       */
      virtual void visit(tchecker::nop_statement_t const & stmt)
      {
        _typed_stmt = new tchecker::typed_nop_statement_t(tchecker::STMT_TYPE_NOP);
      }
      
      
      /*!
       \brief Visitor
       \param stmt : assignment statement
       \post _type_stmt points to a typed clone of this
       */
      virtual void visit(tchecker::assign_statement_t const & stmt)
      {
        // Left and right values
        tchecker::typed_lvalue_expression_t * typed_lvalue =
        dynamic_cast<tchecker::typed_lvalue_expression_t *>(tchecker::typecheck(stmt.lvalue(), _intvars, _clocks, _error));
        
        tchecker::typed_expression_t * typed_rvalue = tchecker::typecheck(stmt.rvalue(), _intvars, _clocks, _error);
        
        // Typed statement
        enum tchecker::statement_type_t stmt_type = type_assign(typed_lvalue->type(), typed_rvalue->type());
        
        switch (stmt_type) {
          case STMT_TYPE_CLKASSIGN_INT:
            _typed_stmt = new tchecker::typed_int_to_clock_assign_statement_t(stmt_type, typed_lvalue, typed_rvalue);
            break;
          case STMT_TYPE_CLKASSIGN_CLK:
            _typed_stmt = new tchecker::typed_clock_to_clock_assign_statement_t
            (stmt_type, typed_lvalue, dynamic_cast<tchecker::typed_lvalue_expression_t *>(typed_rvalue));
            break;
          case STMT_TYPE_CLKASSIGN_SUM:
            _typed_stmt = new tchecker::typed_sum_to_clock_assign_statement_t(stmt_type, typed_lvalue, typed_rvalue);
            break;
          default: // either STMT_TYPE_INTASSIGN or STMT_TYPE_BAD
            _typed_stmt = new tchecker::typed_assign_statement_t(stmt_type, typed_lvalue, typed_rvalue);
            break;
        }
        
        
        // Report bad type
        if (stmt_type != tchecker::STMT_TYPE_BAD)
          return;
        
        if ((typed_lvalue->type() != tchecker::EXPR_TYPE_BAD) && (typed_rvalue->type() != tchecker::EXPR_TYPE_BAD))
          _error("in statement " + stmt.to_string() + ", invalid assignement");
      }
      
      
      /*!
       \brief Visitor
       \param stmt : sequence statement
       \post _type_stmt points to a typed clone of this
       */
      virtual void visit(tchecker::sequence_statement_t const & stmt)
      {
        // First and second statements
        stmt.first().visit(*this);
        tchecker::typed_statement_t * typed_first = this->release();
        
        stmt.second().visit(*this);
        tchecker::typed_statement_t * typed_second = this->release();
        
        // Typed statement
        enum tchecker::statement_type_t stmt_type = type_seq(typed_first->type(), typed_second->type());
        
        _typed_stmt = new tchecker::typed_sequence_statement_t(stmt_type, typed_first, typed_second);
        
        // Report bad type
        if (stmt_type != tchecker::STMT_TYPE_BAD)
          return;
        
        if ((typed_first->type() != tchecker::STMT_TYPE_BAD) && (typed_second->type() != tchecker::STMT_TYPE_BAD))
          _error("in statement " + stmt.to_string() + ", invalid sequence of " + stmt.first().to_string() + " then " +
                 stmt.second().to_string());
      }

      /*!
       \brief Visitor
       \param stmt : if statement
       \post _type_stmt points to a typed clone of this
       */
      virtual void visit(tchecker::if_statement_t const & stmt)
      {
        tchecker::typed_expression_t * typed_cond =
            tchecker::typecheck(stmt.condition (), _intvars, _clocks, _error);

        stmt.then_stmt ().visit(*this);
        tchecker::typed_statement_t * typed_then = this->release();

        stmt.else_stmt ().visit(*this);
        tchecker::typed_statement_t * typed_else = this->release();

        // Typed statement
        enum tchecker::statement_type_t stmt_type =
            type_if(typed_cond->type(), typed_then->type(), typed_else->type());

        _typed_stmt = new tchecker::typed_if_statement_t(stmt_type, typed_cond,
                                                         typed_then, typed_else);

        // Report bad type
        if (stmt_type != tchecker::STMT_TYPE_BAD)
          return;

        if (! tchecker::bool_valued (typed_cond->type ()))
          _error("in statement " + stmt.to_string() + ", not a Boolean condition '" + stmt.condition().to_string());
        else
          _error("invalid if-then-else statement " + stmt.to_string());
      }

      /*!
       \brief Visitor
       \param stmt : while statement
       \post _type_stmt points to a typed clone of this
       */
      virtual void visit(tchecker::while_statement_t const & stmt)
      {
        tchecker::typed_expression_t * typed_cond =
            tchecker::typecheck(stmt.condition (), _intvars, _clocks, _error);

        stmt.statement ().visit(*this);
        tchecker::typed_statement_t * typed_stmt = this->release();

        // Typed statement
        enum tchecker::statement_type_t stmt_type =
            type_while(typed_cond->type(), typed_stmt->type());

        _typed_stmt = new tchecker::typed_while_statement_t(stmt_type, typed_cond, typed_stmt);;

        // Report bad type
        if (stmt_type != tchecker::STMT_TYPE_BAD)
          return;

        if (! tchecker::bool_valued (typed_cond->type ()))
          _error("in statement " + stmt.to_string() + ", not a Boolean condition '" + stmt.condition().to_string());
        else
          _error("invalid while statement " + stmt.to_string());
      }

    protected:
      tchecker::typed_statement_t * _typed_stmt;       /*!< Typed statement */
      tchecker::integer_variables_t const & _intvars;  /*!< Integer variables */
      tchecker::clock_variables_t const & _clocks;     /*!< Clock variables */
      std::function<void(std::string const &)> _error; /*!< Error logging func */
    };
    
  } // end of namespace details
  
  
  
  
  tchecker::typed_statement_t * typecheck(tchecker::statement_t const & stmt,
                                          tchecker::integer_variables_t const & intvars,
                                          tchecker::clock_variables_t const & clocks,
                                          std::function<void(std::string const &)> error)
  {
    tchecker::details::statement_typechecker_t v(intvars, clocks, error);
    stmt.visit(v);
    return v.release();
  }
  
} // end of namespace tchecker
