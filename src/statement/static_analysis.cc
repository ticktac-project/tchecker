/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/expression/static_analysis.hh"
#include "tchecker/statement/static_analysis.hh"

namespace tchecker {
	
	namespace details {
		
		/* extract_read_variables */
		
		/*!
		 \class extract_read_variables_visitor_t
		 \brief Visitor of statements that extract read variables
		 */
		class extract_read_variables_visitor_t : public tchecker::typed_statement_visitor_t {
		public:
			/*!
			 \brief Constructor
			 */
			extract_read_variables_visitor_t(std::unordered_set<tchecker::clock_id_t> & clocks,
																			 std::unordered_set<tchecker::intvar_id_t> & intvars)
			: _clocks(clocks), _intvars(intvars)
			{}
			
			/*!
			 \brief Copy constructor
			 */
			extract_read_variables_visitor_t(tchecker::details::extract_read_variables_visitor_t const &) = default;
			
			/*!
			 \brief Move constructor
			 */
			extract_read_variables_visitor_t(tchecker::details::extract_read_variables_visitor_t &&) = default;
			
			/*!
			 \brief Destructor
			 */
			virtual ~extract_read_variables_visitor_t() = default;
			
			/*!
			 \brief Assignment operator (deleted)
			 */
			tchecker::details::extract_read_variables_visitor_t &
      operator= (tchecker::details::extract_read_variables_visitor_t const &) = delete;
			
			/*!
			 \brief Move assignment operator (deleted)
			 */
			tchecker::details::extract_read_variables_visitor_t &
      operator= (tchecker::details::extract_read_variables_visitor_t &&) = delete;
			
			/*!
			 \brief Add variable IDs from the right-hand side expression of stmt to the sets
			 */
			virtual void visit(tchecker::typed_assign_statement_t const & stmt)
			{
				tchecker::extract_variables(stmt.rvalue(), _clocks, _intvars);
			}
			
			/*!
			 \brief Add variable IDs from the right-hand side value of stmt to the sets
			 */
			virtual void visit(tchecker::typed_int_to_clock_assign_statement_t const & stmt)
			{
				tchecker::extract_variables(stmt.value(), _clocks, _intvars);
			}
			
			/*!
			 \brief Add variable IDs from the right-hand side clock of stmt to the sets
			 */
			virtual void visit(tchecker::typed_clock_to_clock_assign_statement_t const & stmt)
			{
				tchecker::extract_variables(stmt.rclock(), _clocks, _intvars);
			}
			
			/*!
			 \brief Add variable IDs from the right-hand side clock and expression of stmt to the sets
			 */
			virtual void visit(tchecker::typed_sum_to_clock_assign_statement_t const & stmt)
			{
				tchecker::extract_variables(stmt.rclock(), _clocks, _intvars);
				tchecker::extract_variables(stmt.value(), _clocks, _intvars);
			}
			
			/* other visitors */
			
			virtual void visit(tchecker::typed_nop_statement_t const &) {}
			
			virtual void visit(tchecker::typed_sequence_statement_t const & stmt)
			{
				stmt.first().visit(*this);
				stmt.second().visit(*this);
			}
		private:
			std::unordered_set<tchecker::clock_id_t> & _clocks;    /*!< Set of clock IDs */
			std::unordered_set<tchecker::intvar_id_t> & _intvars;  /*!< Set of integer variable IDs */
		};
		
	} // end of namespace details
	
	
	void extract_read_variables(tchecker::typed_statement_t const & stmt,
															std::unordered_set<tchecker::clock_id_t> & clocks,
															std::unordered_set<tchecker::intvar_id_t> & intvars)
	{
		tchecker::details::extract_read_variables_visitor_t v(clocks, intvars);
		stmt.visit(v);
	}
	
	
	
	
	/* extract_written_variables */
	
	namespace details {
		
		/*!
		 \class extract_written_variables_visitor_t
		 \brief Visitor of statements that extract written variables
		 */
		class extract_written_variables_visitor_t : public tchecker::typed_statement_visitor_t {
		public:
			/*!
			 \brief Constructor
			 */
			extract_written_variables_visitor_t(std::unordered_set<tchecker::clock_id_t> & clocks,
																					std::unordered_set<tchecker::intvar_id_t> & intvars)
			: _clocks(clocks), _intvars(intvars)
			{}
			
			/*!
			 \brief Copy constructor
			 */
			extract_written_variables_visitor_t(tchecker::details::extract_written_variables_visitor_t const &) = default;
			
			/*!
			 \brief Move constructor
			 */
			extract_written_variables_visitor_t(tchecker::details::extract_written_variables_visitor_t &&) = default;
			
			/*!
			 \brief Destructor
			 */
			virtual ~extract_written_variables_visitor_t() = default;
			
			/*!
			 \brief Assignment operator (deleted)
			 */
			tchecker::details::extract_written_variables_visitor_t &
			operator= (tchecker::details::extract_written_variables_visitor_t const &) = delete;
			
			/*!
			 \brief Move assignment operator (deleted)
			 */
			tchecker::details::extract_written_variables_visitor_t &
			operator= (tchecker::details::extract_written_variables_visitor_t &&) = delete;
			
			/*!
			 \brief Add variable IDs from the left-hand side expression of stmt to the sets
			 */
			virtual void visit(tchecker::typed_assign_statement_t const & stmt)
			{
				tchecker::extract_variables(stmt.lvalue(), _clocks, _intvars);
			}
			
			/*!
			 \brief Add variable IDs from the left-hand side clock of stmt to the sets
			 */
			virtual void visit(tchecker::typed_int_to_clock_assign_statement_t const & stmt)
			{
				tchecker::extract_variables(stmt.clock(), _clocks, _intvars);
			}
			
			/*!
			 \brief Add variable IDs from the left-hand side clock of stmt to the sets
			 */
			virtual void visit(tchecker::typed_clock_to_clock_assign_statement_t const & stmt)
			{
				tchecker::extract_variables(stmt.lclock(), _clocks, _intvars);
			}
			
			/*!
			 \brief Add variable IDs from the left-hand side clock of stmt to the sets
			 */
			virtual void visit(tchecker::typed_sum_to_clock_assign_statement_t const & stmt)
			{
				tchecker::extract_variables(stmt.lclock(), _clocks, _intvars);
			}
			
			/* other visitors */
			
			virtual void visit(tchecker::typed_nop_statement_t const &) {}
			
			virtual void visit(tchecker::typed_sequence_statement_t const & stmt)
			{
				stmt.first().visit(*this);
				stmt.second().visit(*this);
			}
		private:
			std::unordered_set<tchecker::clock_id_t> & _clocks;    /*!< Set of clock IDs */
			std::unordered_set<tchecker::intvar_id_t> & _intvars;  /*!< Set of integer variable IDs */
		};
		
	} // end of namespace details
	
	
	/*!
	 \brief Extract typed written variables IDs from a statement
	 \param stmt : statement
	 \param clocks : a set of clock IDs
	 \param intvars : a set of integer variable
	 \post for every occurrence of a variable x in the left-hand side of stmt, x has been added to clocks if x
	 is a clock, and to intvars if x is an integer variable. For array expressions (i.e. x[e]) in the left-hand
	 side of stmt such that offset expression e cannot be evaluated statically, all x[k] have been added to the
	 set (according to the type of x) for all k in the domain of x
	 */
	void extract_written_variables(tchecker::typed_statement_t const & stmt,
																 std::unordered_set<tchecker::clock_id_t> & clocks,
																 std::unordered_set<tchecker::intvar_id_t> & intvars)
	{
		tchecker::details::extract_written_variables_visitor_t v(clocks, intvars);
		stmt.visit(v);
	}
	
} // end of namespace tchecker
