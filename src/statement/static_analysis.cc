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
  {
  }

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
  operator=(tchecker::details::extract_read_variables_visitor_t const &) = delete;

  /*!
   \brief Move assignment operator (deleted)
   */
  tchecker::details::extract_read_variables_visitor_t &
  operator=(tchecker::details::extract_read_variables_visitor_t &&) = delete;

  /*!
   \brief Add variable IDs from the right-hand side expression of stmt to the sets
   */
  virtual void visit(tchecker::typed_assign_statement_t const & stmt)
  {
    tchecker::extract_variables(stmt.rvalue(), _clocks, _intvars);
    tchecker::extract_lvalue_offset_variable_ids(stmt.lvalue(), _clocks, _intvars);
  }

  /*!
   \brief Add variable IDs from the right-hand side value of stmt to the sets
   */
  virtual void visit(tchecker::typed_int_to_clock_assign_statement_t const & stmt)
  {
    tchecker::extract_variables(stmt.value(), _clocks, _intvars);
    tchecker::extract_lvalue_offset_variable_ids(stmt.clock(), _clocks, _intvars);
  }

  /*!
   \brief Add variable IDs from the right-hand side clock of stmt to the sets
   */
  virtual void visit(tchecker::typed_clock_to_clock_assign_statement_t const & stmt)
  {
    tchecker::extract_variables(stmt.rclock(), _clocks, _intvars);
    tchecker::extract_lvalue_offset_variable_ids(stmt.lclock(), _clocks, _intvars);
  }

  /*!
   \brief Add variable IDs from the right-hand side clock and expression of stmt to the sets
   */
  virtual void visit(tchecker::typed_sum_to_clock_assign_statement_t const & stmt)
  {
    tchecker::extract_variables(stmt.rclock(), _clocks, _intvars);
    tchecker::extract_variables(stmt.value(), _clocks, _intvars);
    tchecker::extract_lvalue_offset_variable_ids(stmt.lclock(), _clocks, _intvars);
  }

  /* other visitors */

  virtual void visit(tchecker::typed_nop_statement_t const &) {}

  virtual void visit(tchecker::typed_sequence_statement_t const & stmt)
  {
    stmt.first().visit(*this);
    stmt.second().visit(*this);
  }

  virtual void visit(tchecker::typed_if_statement_t const & stmt)
  {
    tchecker::extract_variables(stmt.condition(), _clocks, _intvars);
    stmt.then_stmt().visit(*this);
    stmt.else_stmt().visit(*this);
  }

  virtual void visit(tchecker::typed_while_statement_t const & stmt)
  {
    tchecker::extract_variables(stmt.condition(), _clocks, _intvars);
    stmt.statement().visit(*this);
  }

  virtual void visit(tchecker::typed_local_var_statement_t const & stmt)
  {
    tchecker::extract_variables(stmt.initial_value(), _clocks, _intvars);
  }

  virtual void visit(tchecker::typed_local_array_statement_t const & stmt)
  {
    tchecker::extract_variables(stmt.size(), _clocks, _intvars);
  }

private:
  std::unordered_set<tchecker::clock_id_t> & _clocks;   /*!< Set of clock IDs */
  std::unordered_set<tchecker::intvar_id_t> & _intvars; /*!< Set of integer variable IDs */
};

} // end of namespace details

void extract_read_variables(tchecker::typed_statement_t const & stmt, std::unordered_set<tchecker::clock_id_t> & clocks,
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
  {
  }

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
  operator=(tchecker::details::extract_written_variables_visitor_t const &) = delete;

  /*!
   \brief Move assignment operator (deleted)
   */
  tchecker::details::extract_written_variables_visitor_t &
  operator=(tchecker::details::extract_written_variables_visitor_t &&) = delete;

  /*!
   \brief Add variable IDs from the left-hand side expression of stmt to the sets
   */
  virtual void visit(tchecker::typed_assign_statement_t const & stmt)
  {
    tchecker::extract_lvalue_base_variable_ids(stmt.lvalue(), _clocks, _intvars);
  }

  /*!
   \brief Add variable IDs from the left-hand side clock of stmt to the sets
   */
  virtual void visit(tchecker::typed_int_to_clock_assign_statement_t const & stmt)
  {
    tchecker::extract_lvalue_base_variable_ids(stmt.clock(), _clocks, _intvars);
  }

  /*!
   \brief Add variable IDs from the left-hand side clock of stmt to the sets
   */
  virtual void visit(tchecker::typed_clock_to_clock_assign_statement_t const & stmt)
  {
    tchecker::extract_lvalue_base_variable_ids(stmt.lclock(), _clocks, _intvars);
  }

  /*!
   \brief Add variable IDs from the left-hand side clock of stmt to the sets
   */
  virtual void visit(tchecker::typed_sum_to_clock_assign_statement_t const & stmt)
  {
    tchecker::extract_lvalue_base_variable_ids(stmt.lclock(), _clocks, _intvars);
  }

  /* other visitors */

  virtual void visit(tchecker::typed_nop_statement_t const &) {}

  virtual void visit(tchecker::typed_sequence_statement_t const & stmt)
  {
    stmt.first().visit(*this);
    stmt.second().visit(*this);
  }

  virtual void visit(tchecker::typed_if_statement_t const & stmt)
  {
    stmt.then_stmt().visit(*this);
    stmt.else_stmt().visit(*this);
  }
  virtual void visit(tchecker::typed_while_statement_t const & stmt) { stmt.statement().visit(*this); }

  virtual void visit(tchecker::typed_local_var_statement_t const & stmt) {}

  virtual void visit(tchecker::typed_local_array_statement_t const & stmt) {}

private:
  std::unordered_set<tchecker::clock_id_t> & _clocks;   /*!< Set of clock IDs */
  std::unordered_set<tchecker::intvar_id_t> & _intvars; /*!< Set of integer variable IDs */
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
void extract_written_variables(tchecker::typed_statement_t const & stmt, std::unordered_set<tchecker::clock_id_t> & clocks,
                               std::unordered_set<tchecker::intvar_id_t> & intvars)
{
  tchecker::details::extract_written_variables_visitor_t v(clocks, intvars);
  stmt.visit(v);
}

/* local_declaration */

namespace details {

/*!
 \class local_declaration_visitor_t
 \brief Visitor of statements that check if at least one local variable exists
 */
class local_declaration_visitor_t : public tchecker::typed_statement_visitor_t {
public:
  /*!
   \brief Constructor
   */
  local_declaration_visitor_t() : _value(false) {}

  /*!
   \brief Copy constructor
   */
  local_declaration_visitor_t(tchecker::details::local_declaration_visitor_t const &) = default;

  /*!
   \brief Move constructor
   */
  local_declaration_visitor_t(tchecker::details::local_declaration_visitor_t &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~local_declaration_visitor_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::details::local_declaration_visitor_t & operator=(tchecker::details::local_declaration_visitor_t const &) = delete;

  /*!
   \brief Move assignment operator (deleted)
   */
  tchecker::details::local_declaration_visitor_t & operator=(tchecker::details::local_declaration_visitor_t &&) = delete;

  virtual void visit(tchecker::typed_sequence_statement_t const & stmt)
  {
    stmt.first().visit(*this);
    if (!_value)
      stmt.second().visit(*this);
  }

  virtual void visit(tchecker::typed_local_var_statement_t const & stmt) { _value = true; }

  virtual void visit(tchecker::typed_local_array_statement_t const & stmt) { _value = true; }

  // irrelevant methods
  virtual void visit(tchecker::typed_assign_statement_t const & stmt) {}
  virtual void visit(tchecker::typed_int_to_clock_assign_statement_t const & stmt) {}
  virtual void visit(tchecker::typed_clock_to_clock_assign_statement_t const & stmt) {}
  virtual void visit(tchecker::typed_sum_to_clock_assign_statement_t const & stmt) {}
  virtual void visit(tchecker::typed_nop_statement_t const &) {}
  virtual void visit(tchecker::typed_if_statement_t const & stmt) {}
  virtual void visit(tchecker::typed_while_statement_t const & stmt) {}

  inline bool value() { return _value; }

private:
  bool _value;
};

} // end of namespace details

bool has_local_declarations(tchecker::typed_statement_t const & stmt)
{
  tchecker::details::local_declaration_visitor_t v;

  stmt.visit(v);
  return v.value();
}

/* has_clock_resets */

namespace details {

/*!
 \class clock_resets_visitor_t
 \brief Visitor of statements that computes the types of clock resets in a statement
 */
class clock_resets_visitor_t : public tchecker::typed_statement_visitor_t {
public:
  /*!
   \brief Constructor
   */
  clock_resets_visitor_t() : _has_clock_resets{.constant = false, .clock = false, .sum = false} {}

  /*!
   \brief Copy constructor
   */
  clock_resets_visitor_t(tchecker::details::clock_resets_visitor_t const &) = default;

  /*!
   \brief Move constructor
   */
  clock_resets_visitor_t(tchecker::details::clock_resets_visitor_t &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~clock_resets_visitor_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::details::clock_resets_visitor_t & operator=(tchecker::details::clock_resets_visitor_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::details::clock_resets_visitor_t & operator=(tchecker::details::clock_resets_visitor_t &&) = default;

  /*!
   \brief Accessor
   \return Types of clock resets in last visited statement
   */
  inline tchecker::has_clock_resets_t const & has_clock_resets() const { return _has_clock_resets; }

  /*!
   \brief Clock reset to constant
   */
  virtual void visit(tchecker::typed_int_to_clock_assign_statement_t const & stmt) { _has_clock_resets.constant = true; }

  /*!
   \brief Clock reset to clock
   */
  virtual void visit(tchecker::typed_clock_to_clock_assign_statement_t const & stmt) { _has_clock_resets.clock = true; }

  /*!
   \brief Clock reset to sum
   */
  virtual void visit(tchecker::typed_sum_to_clock_assign_statement_t const & stmt) { _has_clock_resets.sum = true; }

  // Other methods: recursion and stop

  virtual void visit(tchecker::typed_local_var_statement_t const & stmt) {}

  virtual void visit(tchecker::typed_local_array_statement_t const & stmt) {}

  virtual void visit(tchecker::typed_sequence_statement_t const & stmt)
  {
    stmt.first().visit(*this);
    stmt.second().visit(*this);
  }

  virtual void visit(tchecker::typed_assign_statement_t const & stmt) {}

  virtual void visit(tchecker::typed_nop_statement_t const &) {}

  virtual void visit(tchecker::typed_if_statement_t const & stmt)
  {
    stmt.then_stmt().visit(*this);
    stmt.else_stmt().visit(*this);
  }

  virtual void visit(tchecker::typed_while_statement_t const & stmt) { stmt.statement().visit(*this); }

private:
  tchecker::has_clock_resets_t _has_clock_resets; /*!< Type of clocks resets contained in statement last visited */
};

} // end of namespace details

tchecker::has_clock_resets_t has_clock_resets(tchecker::typed_statement_t const & stmt)
{
  tchecker::details::clock_resets_visitor_t v;
  stmt.visit(v);
  return v.has_clock_resets();
}

/* compute_clock_updates */

class clock_updates_visitor_t : public tchecker::typed_statement_visitor_t {
public:
  /*!
   \brief Constructor
   \param clock_nb : clock number
   \post this is a clock updates visitor over clock identifiers in [0..clock_nb)
   */
  clock_updates_visitor_t(std::size_t clock_nb) : _u(clock_nb), _loop_depth(0) {}

  /*!
   \brief Copy constructor
   */
  clock_updates_visitor_t(tchecker::clock_updates_visitor_t const &) = default;

  /*!
   \brief Move constructor
   */
  clock_updates_visitor_t(tchecker::clock_updates_visitor_t &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~clock_updates_visitor_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::clock_updates_visitor_t & operator=(tchecker::clock_updates_visitor_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::clock_updates_visitor_t & operator=(tchecker::clock_updates_visitor_t &&) = default;

  /*!
   \brief Accessor
   \return Internal clock updates map
   */
  inline tchecker::clock_updates_map_t const & clock_updates_map() const { return _u; }

  /*!
   \brief Accessor
   \return true if current statement is in a loop, false otherwise
   */
  bool in_loop() const { return _loop_depth > 0; }

  /*!
   \brief Nop statement visitor
   \post done nothing
   */
  virtual void visit(tchecker::typed_nop_statement_t const & stmt) {}

  /*!
   \brief Assign statement
   \post done nothing
  */
  virtual void visit(tchecker::typed_assign_statement_t const & stmt) {}

  /*!
   \brief Int-to-clock assignment
   \post clock updates map has been updated accoding to update_int_to_clock_inside_loop or to update_int_to_clock_outside_loop
   */
  virtual void visit(tchecker::typed_int_to_clock_assign_statement_t const & stmt)
  {
    if (in_loop())
      update_int_to_clock_inside_loop(stmt.clock(), stmt.value_ptr());
    else
      update_int_to_clock_outside_loop(stmt.clock(), stmt.value_ptr());
  }

  /*!
   \brief Clock-to-clock assignment
   \post clock updates map has been updated according to update_sum_to_clock_inside_loop or to update_sum_to_clock_outside_loop
   */
  virtual void visit(tchecker::typed_clock_to_clock_assign_statement_t const & stmt)
  {
    if (in_loop())
      update_sum_to_clock_inside_loop(stmt.lclock(), stmt.rclock(), std::make_shared<tchecker::int_expression_t>(0));
    else
      update_sum_to_clock_outside_loop(stmt.lclock(), stmt.rclock(), std::make_shared<tchecker::int_expression_t>(0));
  }

  /*!
   \brief Sum-to-clock assignment
   \post clock updates map has been updated according to update_sum_to_clock_inside_loop or to update_sum_to_clock_outside_loop
   */
  virtual void visit(tchecker::typed_sum_to_clock_assign_statement_t const & stmt)
  {
    if (in_loop())
      update_sum_to_clock_inside_loop(stmt.lclock(), stmt.rclock(), stmt.value_ptr());
    else
      update_sum_to_clock_outside_loop(stmt.lclock(), stmt.rclock(), stmt.value_ptr());
  }

  /*!
   \brief Sequence statement
   \post updated clock updates map along first statement, then along second statement
   */
  virtual void visit(tchecker::typed_sequence_statement_t const & stmt)
  {
    stmt.first().visit(*this);
    stmt.second().visit(*this);
  }

  /*!
   \brief If statement
   \post updated clock updates map along then branch, and a copy along the else branch.
   Then merged the two copies
   */
  virtual void visit(tchecker::typed_if_statement_t const & stmt)
  {
    tchecker::clock_updates_visitor_t v(*this);
    stmt.then_stmt().visit(*this);
    stmt.else_stmt().visit(v);
    _u.absorbing_merge(v._u);
  }

  /*!
   \brief While statement
   \post set loop mode, update clock updates map
   */
  virtual void visit(tchecker::typed_while_statement_t const & stmt)
  {
    ++_loop_depth;
    stmt.statement().visit(*this);
    --_loop_depth;
  }

  /*!
   \brief Local variable statement
   \post done nothing
   */
  virtual void visit(tchecker::typed_local_var_statement_t const & stmt) {}

  /*!
   \brief Local array statement
   \post done nothing
   */
  virtual void visit(tchecker::typed_local_array_statement_t const & stmt) {}

private:
  // In the following comments, `|_|` is the absorbing union which results in an empty set
  // if one of the two operands are empty, and their union otherwise

  /*!
   \brief Update the clock updates map w.r.t. reset x := c inside a loop
   \param x : clock expression
   \param c : expression
   \post if c can be statically evaluated then set `_u[z]` to `_u[z] |_| {(REFCLOCK, c)}`
   for every instance z of x (recall that the loop may not be executed)
   otherwise, set `_u[z]` to `{}` (undetermined) for every instance z of x
   */
  void update_int_to_clock_inside_loop(tchecker::typed_lvalue_expression_t const & x,
                                       std::shared_ptr<tchecker::expression_t const> const & c)
  {
    tchecker::range_t<tchecker::variable_id_t> x_instances = tchecker::extract_lvalue_variable_ids(x);
    if (tchecker::has_const_value(*c)) {
      tchecker::clock_update_t x_upd{tchecker::REFCLOCK_ID, c};
      for (tchecker::clock_id_t z = x_instances.begin(); z != x_instances.end(); ++z)
        _u[z].absorbing_push_back(x_upd);
    }
    else {
      for (tchecker::clock_id_t z = x_instances.begin(); z != x_instances.end(); ++z)
        _u[z].clear();
    }
  }

  /*!
   \brief Update the clock updates map w.r.t. reset x := c outside of a loop
   \param x : clock expression
   \param c : expression
   \post if x has a single instance, then set `_u[x]` to `{(REFCLOCK, c)}`
   otherwise, for every instance z of x, set `_u[z]` to `_u[z] |_| {(REFCLOCK, c)}`
   */
  void update_int_to_clock_outside_loop(tchecker::typed_lvalue_expression_t const & x,
                                        std::shared_ptr<tchecker::expression_t const> const & c)
  {
    tchecker::range_t<tchecker::variable_id_t> x_instances = tchecker::extract_lvalue_variable_ids(x);
    tchecker::clock_update_t x_upd{tchecker::REFCLOCK_ID, c};
    if (single_instance(x_instances)) {
      tchecker::clock_id_t z = x_instances.begin();
      _u[z].set(x_upd);
    }
    else {
      for (tchecker::clock_id_t z = x_instances.begin(); z != x_instances.end(); ++z)
        _u[z].absorbing_push_back(x_upd);
    }
  }

  /*!
   \brief Update the clock updates map w.r.t. reset x := y + c inside a loop
   \param x : clock expression
   \param y : clock expression
   \param c : expression
   \post set `_u(z)` to `{}` (undetermined) for every `z` in `instances(x)`
   */
  void update_sum_to_clock_inside_loop(tchecker::typed_lvalue_expression_t const & x,
                                       tchecker::typed_lvalue_expression_t const & y,
                                       std::shared_ptr<tchecker::expression_t const> const & c)
  {
    tchecker::range_t<tchecker::variable_id_t> x_instances = tchecker::extract_lvalue_variable_ids(x);
    for (tchecker::clock_id_t z = x_instances.begin(); z != x_instances.end(); ++z)
      _u[z].clear();
  }

  /*!
   \brief Update the clock updates map w.r.t. reset x := y + c outside of a loop
   \param x : clock expression
   \param y : clock expression
   \param c : expression
   \post let A be the absorbing union of `_u[t]+c` for every intance t of y
   if x has a single instance then set `_u[x]` to A
   otherwise, set `_u[z]` to `_u[z] |_| A` for every instance z of x
   */
  void update_sum_to_clock_outside_loop(tchecker::typed_lvalue_expression_t const & x,
                                        tchecker::typed_lvalue_expression_t const & y,
                                        std::shared_ptr<tchecker::expression_t const> const & c)
  {
    tchecker::clock_updates_list_t A = clock_updates_of_all_clock_instances(y) + c;
    tchecker::range_t<tchecker::variable_id_t> x_instances = tchecker::extract_lvalue_variable_ids(x);
    if (single_instance(x_instances)) {
      tchecker::clock_id_t z = x_instances.begin();
      _u[z] = A;
    }
    else {
      for (tchecker::clock_id_t z = x_instances.begin(); z != x_instances.end(); ++z)
        _u[z].absorbing_merge(A);
    }
  }

  /*!
   \brief Checks if a range of variable identifier contains a single instance
   \param r : range
   \return true if r contains a single instance, false otherwise
   */
  static bool single_instance(tchecker::range_t<tchecker::variable_id_t> const & r) { return r.end() - r.begin() == 1; }

  /*!
   \brief Compute absorbing merge of all updates of a clock
   \param x : clock expression
   \return the absorbing union of the updates of all instances of x
   */
  tchecker::clock_updates_list_t clock_updates_of_all_clock_instances(tchecker::typed_lvalue_expression_t const & x) const
  {
    tchecker::range_t<tchecker::variable_id_t> x_instances = tchecker::extract_lvalue_variable_ids(x);
    assert(!x_instances.empty());

    tchecker::clock_id_t z = x_instances.begin();
    tchecker::clock_updates_list_t l = _u[z];
    for (++z; z != x_instances.end(); ++z)
      l.absorbing_merge(_u[z]);

    return l;
  }

  tchecker::clock_updates_map_t _u; /*!< Clock updates map */
  std::size_t _loop_depth;          /*!< Nested while-loops depth */
};

tchecker::clock_updates_map_t compute_clock_updates(std::size_t clock_nb, tchecker::typed_statement_t const & stmt)
{
  tchecker::clock_updates_visitor_t v{clock_nb};
  stmt.visit(v);
  return v.clock_updates_map();
}

} // end of namespace tchecker
