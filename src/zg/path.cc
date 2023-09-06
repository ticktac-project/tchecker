/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <numeric>

#include "tchecker/dbm/dbm.hh"
#include "tchecker/zg/path.hh"
#include "tchecker/zg/semantics.hh"

namespace tchecker {

namespace zg {

namespace path {

namespace symbolic {

/* node_t */

node_t::node_t(tchecker::zg::state_sptr_t const & s, bool initial, bool final)
    : tchecker::graph::node_flags_t(initial, final), tchecker::graph::node_zg_state_t(s)
{
}

node_t::node_t(tchecker::zg::const_state_sptr_t const & s, bool initial, bool final)
    : tchecker::graph::node_flags_t(initial, final), tchecker::graph::node_zg_state_t(s)
{
}

/* lexical_cmp on nodes */

int lexical_cmp(tchecker::zg::path::symbolic::node_t const & n1, tchecker::zg::path::symbolic::node_t const & n2)
{
  int state_cmp = tchecker::ta::lexical_cmp(static_cast<tchecker::ta::state_t const &>(n1.state()),
                                            static_cast<tchecker::ta::state_t const &>(n2.state()));
  if (state_cmp != 0)
    return state_cmp;
  if (n1.initial() != n2.initial())
    return n1.initial() < n2.initial();
  return n1.final() < n2.final();
}

/* edge_t */

edge_t::edge_t(tchecker::zg::const_transition_sptr_t const & t) : _t(t) {}

edge_t::edge_t(tchecker::zg::transition_sptr_t const & t) : _t(t) {}

edge_t::edge_t(tchecker::zg::path::symbolic::edge_t const & e) : _t(e._t) {}

edge_t::edge_t(tchecker::zg::path::symbolic::edge_t && e) : _t(std::move(e._t)) {}

tchecker::zg::path::symbolic::edge_t & edge_t::operator=(tchecker::zg::path::symbolic::edge_t const & e)
{
  if (this != &e)
    _t = e._t;
  return *this;
}

tchecker::zg::path::symbolic::edge_t & edge_t::operator=(tchecker::zg::path::symbolic::edge_t && e)
{
  if (this != &e)
    _t = std::move(e._t);
  return *this;
}

/* lexical_cmp on path edges */

int lexical_cmp(tchecker::zg::path::symbolic::edge_t const & e1, tchecker::zg::path::symbolic::edge_t const & e2)
{
  return tchecker::zg::lexical_cmp(e1.transition(), e2.transition()) < 0;
}

/* finite_path_t */

void finite_path_t::attributes(tchecker::zg::path::symbolic::node_t const & n, std::map<std::string, std::string> & m) const
{
  tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::symbolic::node_t,
                              tchecker::zg::path::symbolic::edge_t>::attributes(n, m);
  if (n.initial())
    m["initial"] = "true";
  if (n.final())
    m["final"] = "true";
}

void finite_path_t::attributes(tchecker::zg::path::symbolic::edge_t const & e, std::map<std::string, std::string> & m) const
{
  tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::symbolic::node_t,
                              tchecker::zg::path::symbolic::edge_t>::attributes(e, m);
}

/* output */

/*!
 \brief Lexicographic ordering on path nodes
*/
class node_le_t {
public:
  bool operator()(tchecker::zg::path::symbolic::finite_path_t::node_sptr_t const & n1,
                  tchecker::zg::path::symbolic::finite_path_t::node_sptr_t const & n2) const
  {
    return tchecker::zg::path::symbolic::lexical_cmp(static_cast<tchecker::zg::path::symbolic::node_t const &>(*n1),
                                                     static_cast<tchecker::zg::path::symbolic::node_t const &>(*n2)) < 0;
  }
};

/*!
 \brief Lexicographic ordering on edges
*/
class edge_le_t {
public:
  bool operator()(tchecker::zg::path::symbolic::finite_path_t::edge_sptr_t const & e1,
                  tchecker::zg::path::symbolic::finite_path_t::edge_sptr_t const & e2) const
  {
    return tchecker::zg::path::symbolic::lexical_cmp(static_cast<tchecker::zg::path::symbolic::edge_t const &>(*e1),
                                                     static_cast<tchecker::zg::path::symbolic::edge_t const &>(*e2)) < 0;
  }
};

std::ostream & dot_output(std::ostream & os, tchecker::zg::path::symbolic::finite_path_t const & path, std::string const & name)
{
  return tchecker::graph::dot_output<tchecker::zg::path::symbolic::finite_path_t, tchecker::zg::path::symbolic::node_le_t,
                                     tchecker::zg::path::symbolic::edge_le_t>(os, path, name);
}

/* finite path computation */

tchecker::zg::path::symbolic::finite_path_t * compute_finite_path(std::shared_ptr<tchecker::zg::zg_t> const & zg,
                                                                  tchecker::vloc_t const & initial_vloc,
                                                                  std::vector<tchecker::const_vedge_sptr_t> const & seq,
                                                                  bool last_node_final)
{
  tchecker::zg::path::symbolic::finite_path_t * path = new tchecker::zg::path::symbolic::finite_path_t{zg};

  tchecker::zg::const_state_sptr_t s{tchecker::zg::initial(*zg, initial_vloc)};
  if (s.ptr() == nullptr) {
    delete path;
    throw std::invalid_argument("*** compute_finite_path(symbolic): no initial state with given tuple of locations");
  }

  path->add_first_node(s);
  path->first()->initial(true);

  for (tchecker::const_vedge_sptr_t const & vedge_ptr : seq) {
    s = path->last()->state_ptr();
    auto && [nexts, nextt] = tchecker::zg::next(*zg, s, *vedge_ptr);
    if (nexts.ptr() == nullptr || nextt.ptr() == nullptr) {
      delete path;
      throw std::invalid_argument("*** compute_finite_path(symbolic): sequence is not feasible from given initial locations");
    }
    path->extend_back(nextt, nexts);
  }

  path->last()->final(last_node_final);

  return path;
}

/* lasso_path_t */

void lasso_path_t::attributes(tchecker::zg::path::symbolic::node_t const & n, std::map<std::string, std::string> & m) const
{
  tchecker::ts::lasso_path_t<tchecker::zg::zg_t, tchecker::zg::path::symbolic::node_t,
                             tchecker::zg::path::symbolic::edge_t>::attributes(n, m);
  if (n.initial())
    m["initial"] = "true";
  if (n.final())
    m["final"] = "true";
}

void lasso_path_t::attributes(tchecker::zg::path::symbolic::edge_t const & e, std::map<std::string, std::string> & m) const
{
  tchecker::ts::lasso_path_t<tchecker::zg::zg_t, tchecker::zg::path::symbolic::node_t,
                             tchecker::zg::path::symbolic::edge_t>::attributes(e, m);
}

/* output lasso path */

std::ostream & dot_output(std::ostream & os, tchecker::zg::path::symbolic::lasso_path_t const & path, std::string const & name)
{
  return tchecker::graph::dot_output<tchecker::zg::path::symbolic::lasso_path_t, tchecker::zg::path::symbolic::node_le_t,
                                     tchecker::zg::path::symbolic::edge_le_t>(os, path, name);
}

/* lasso path computation */

tchecker::zg::path::symbolic::lasso_path_t *
compute_lasso_path(std::shared_ptr<tchecker::zg::zg_t> const & zg, tchecker::vloc_t const & initial_vloc,
                   std::vector<tchecker::const_vedge_sptr_t> const & prefix,
                   std::vector<tchecker::const_vedge_sptr_t> const & cycle,
                   std::function<bool(tchecker::zg::state_t const &)> && final_state)
{
  if (cycle.empty())
    throw std::invalid_argument("*** compute_lasso_path: empty cycle");

  tchecker::zg::path::symbolic::lasso_path_t * path = new tchecker::zg::path::symbolic::lasso_path_t{zg};

  // add initial node
  tchecker::zg::const_state_sptr_t s{tchecker::zg::initial(*zg, initial_vloc)};
  if (s.ptr() == nullptr) {
    delete path;
    throw std::invalid_argument("*** compute_lasso_path: no initial state with given tuple of locations");
  }

  path->add_first_node(s);
  path->first()->initial(true);

  // extend with prefix
  for (tchecker::const_vedge_sptr_t const & vedge_ptr : prefix) {
    s = path->loop_root()->state_ptr();
    auto && [nexts, nextt] = tchecker::zg::next(*zg, s, *vedge_ptr);
    if (nexts.ptr() == nullptr || nextt.ptr() == nullptr) {
      delete path;
      throw std::invalid_argument("*** compute_lasso_path: prefix is not feasible from given initial locations");
    }
    path->extend_back(nextt, nexts);
  }

  // compute cycle
  s = path->loop_root()->state_ptr();
  std::vector<std::tuple<tchecker::zg::transition_sptr_t, tchecker::zg::state_sptr_t>> args;
  for (std::size_t i = 0; i < cycle.size() - 1; ++i) {
    auto && [nexts, nextt] = tchecker::zg::next(*zg, s, *cycle[i]);
    if (nexts.ptr() == nullptr || nextt.ptr() == nullptr) {
      delete path;
      throw std::invalid_argument("*** compute_lasso_path: prefix is not feasible from given initial locations");
    }
    args.emplace_back(std::make_tuple(nextt, nexts));
    s = tchecker::zg::const_state_sptr_t{nexts};
  }

  // compute cycle closing edge
  auto && [nexts, nextt] = tchecker::zg::next(*zg, s, *cycle.back());
  if (nexts.ptr() == nullptr || nextt.ptr() == nullptr) {
    delete path;
    throw std::invalid_argument("*** compute_lasso_path: prefix is not feasible from given initial locations");
  }

  // extend path with cycle
  assert(*nexts == path->loop_root()->state());
  path->extend_loop(args, nextt);

  // mark final nodes
  for (auto node : path->nodes())
    if (final_state(node->state()))
      node->final(true);

  return path;
}

} // namespace symbolic

namespace concrete {

/* node_t */

node_t::node_t(tchecker::zg::state_sptr_t const & s, tchecker::clockval_sptr_t const & clockval, bool initial, bool final)
    : tchecker::zg::path::symbolic::node_t(s, initial, final), _clockval(clockval)
{
  if (_clockval->size() != s->zone().dim())
    throw std::invalid_argument("clock values and zone do not have the same size");

  if (!s->zone().belongs(*_clockval))
    throw std::invalid_argument("clock valuation does not belong to the zone");
}

node_t::node_t(tchecker::zg::const_state_sptr_t const & s, tchecker::const_clockval_sptr_t const & clockval, bool initial,
               bool final)
    : tchecker::zg::path::symbolic::node_t(s, initial, final), _clockval(clockval)
{
  if (_clockval->size() != s->zone().dim())
    throw std::invalid_argument("clock values and zone do not have the same size");

  if (!s->zone().belongs(*_clockval))
    throw std::invalid_argument("clock valuation does not belong to the zone");
}

node_t::node_t(std::tuple<tchecker::zg::state_sptr_t, tchecker::clockval_sptr_t, bool, bool> const & t)
    : node_t(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t))
{
}

node_t::node_t(std::tuple<tchecker::zg::const_state_sptr_t, tchecker::const_clockval_sptr_t, bool, bool> const & t)
    : node_t(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t))
{
}

int lexical_cmp(tchecker::zg::path::concrete::node_t const & n1, tchecker::zg::path::concrete::node_t const & n2)
{
  int cmp = tchecker::zg::path::symbolic::lexical_cmp(n1, n2);
  if (cmp != 0)
    return cmp;
  return tchecker::lexical_cmp(n1.clockval(), n2.clockval());
}

/* edge_t */

edge_t::edge_t(tchecker::zg::const_transition_sptr_t const & t, tchecker::clock_rational_value_t delay)
    : tchecker::zg::path::symbolic::edge_t(t), _delay(delay)
{
  if (delay < 0)
    throw std::invalid_argument("negative delay is not allowed");
}

edge_t::edge_t(tchecker::zg::transition_sptr_t const & t, tchecker::clock_rational_value_t delay)
    : tchecker::zg::path::symbolic::edge_t(t), _delay(delay)
{
  if (delay < 0)
    throw std::invalid_argument("negative delay is not allowed");
}

edge_t::edge_t(std::tuple<tchecker::zg::const_transition_sptr_t, tchecker::clock_rational_value_t> const & t)
    : edge_t(std::get<0>(t), std::get<1>(t))
{
}

edge_t::edge_t(std::tuple<tchecker::zg::transition_sptr_t, tchecker::clock_rational_value_t> const & t)
    : edge_t(std::get<0>(t), std::get<1>(t))
{
}

int lexical_cmp(tchecker::zg::path::concrete::edge_t const & e1, tchecker::zg::path::concrete::edge_t const & e2)
{
  int cmp = tchecker::zg::path::symbolic::lexical_cmp(e1, e2);
  if (cmp != 0)
    return cmp;
  tchecker::clock_rational_value_t const d1 = e1.delay(), d2 = e2.delay();
  if (d1 < d2)
    return -1;
  if (d1 > d2)
    return 1;
  return 0;
}

/* finite_path_t */

finite_path_t::finite_path_t(std::shared_ptr<tchecker::zg::zg_t> const & zg, std::size_t clockval_alloc_nb)
    : tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::concrete::node_t,
                                  tchecker::zg::path::concrete::edge_t>::finite_path_t(zg),
      _clockval_pool(clockval_alloc_nb, tchecker::allocation_size_t<tchecker::clockval_t>::alloc_size(
                                            zg->system().clocks_count(tchecker::VK_FLATTENED) + 1))
{
}

finite_path_t::~finite_path_t()
{
  tchecker::graph::finite_path_t<tchecker::zg::path::concrete::node_t, tchecker::zg::path::concrete::edge_t>::clear();
  _clockval_pool.destruct_all();
}

void finite_path_t::add_first_node(tchecker::zg::const_state_sptr_t const & s, tchecker::clockval_t const & clockval,
                                   bool initial, bool final)
{
  tchecker::const_clockval_sptr_t clockval_sptr{_clockval_pool.construct(clockval)};
  tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::concrete::node_t,
                              tchecker::zg::path::concrete::edge_t>::add_first_node(std::make_tuple(s, clockval_sptr, initial,
                                                                                                    final));
}

void finite_path_t::extend_back(tchecker::zg::const_transition_sptr_t const & t, tchecker::clock_rational_value_t delay,
                                tchecker::zg::const_state_sptr_t const & s, tchecker::clockval_t const & clockval, bool initial,
                                bool final)
{
  tchecker::const_clockval_sptr_t clockval_sptr{_clockval_pool.construct(clockval)};
  tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::concrete::node_t,
                              tchecker::zg::path::concrete::edge_t>::extend_back(std::make_tuple(t, delay),
                                                                                 std::make_tuple(s, clockval_sptr, initial,
                                                                                                 final));
}

void finite_path_t::extend_front(tchecker::zg::const_transition_sptr_t const & t, tchecker::clock_rational_value_t delay,
                                 tchecker::zg::const_state_sptr_t const & s, tchecker::clockval_t const & clockval,
                                 bool initial, bool final)
{
  tchecker::const_clockval_sptr_t clockval_sptr{_clockval_pool.construct(clockval)};
  tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::concrete::node_t,
                              tchecker::zg::path::concrete::edge_t>::extend_front(std::make_tuple(t, delay),
                                                                                  std::make_tuple(s, clockval_sptr, initial,
                                                                                                  final));
}

void finite_path_t::attributes(tchecker::zg::path::concrete::node_t const & n, std::map<std::string, std::string> & m) const
{
  tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::concrete::node_t,
                              tchecker::zg::path::concrete::edge_t>::attributes(n, m);
  if (n.initial())
    m["initial"] = "true";
  if (n.final())
    m["final"] = "true";
  tchecker::clock_index_t const & clock_index = ts().system().clock_variables().flattened().index();
  m["clockval"] =
      tchecker::to_string(n.clockval(), [&](tchecker::clock_id_t id) { return (id == 0 ? "$0" : clock_index.value(id - 1)); });
}

void finite_path_t::attributes(tchecker::zg::path::concrete::edge_t const & e, std::map<std::string, std::string> & m) const
{
  tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::concrete::node_t,
                              tchecker::zg::path::concrete::edge_t>::attributes(e, m);
  m["delay"] = tchecker::to_string(e.delay());
}

/* output */

/*!
 \brief Lexicographic ordering on concrete path nodes
*/
class node_le_t {
public:
  bool operator()(tchecker::zg::path::concrete::finite_path_t::node_sptr_t const & n1,
                  tchecker::zg::path::concrete::finite_path_t::node_sptr_t const & n2) const
  {
    return tchecker::zg::path::concrete::lexical_cmp(static_cast<tchecker::zg::path::concrete::node_t const &>(*n1),
                                                     static_cast<tchecker::zg::path::concrete::node_t const &>(*n2)) < 0;
  }
};

/*!
 \brief Lexicographic ordering on concrete edges
*/
class edge_le_t {
public:
  bool operator()(tchecker::zg::path::concrete::finite_path_t::edge_sptr_t const & e1,
                  tchecker::zg::path::concrete::finite_path_t::edge_sptr_t const & e2) const
  {
    return tchecker::zg::path::concrete::lexical_cmp(static_cast<tchecker::zg::path::concrete::edge_t const &>(*e1),
                                                     static_cast<tchecker::zg::path::concrete::edge_t const &>(*e2)) < 0;
  }
};

std::ostream & dot_output(std::ostream & os, tchecker::zg::path::concrete::finite_path_t const & path, std::string const & name)
{
  return tchecker::graph::dot_output<tchecker::zg::path::concrete::finite_path_t, tchecker::zg::path::concrete::node_le_t,
                                     tchecker::zg::path::concrete::edge_le_t>(os, path, name);
}

/* concrete path computation */

/*!
 \class clockval_dbm_t
 \brief DBM representation of a clock valuation
 \note permits to compute clock valuations along a sequence of transitions using
 semantics defined on DBMs
 \note uses a DBM with integer values along with a denominator (common to all values) to
 represent a DBM over rational numbers
 \note first implementation by Ocan Sankur
*/
class clockval_dbm_t {
public:
  /*!
   \brief Constructor
   \param dbm : a DBM
   \param dim : dimension of DBM
   \pre dbm is not nullptr (checked by assertion)
   dbm is a dim*dim allocated array of difference bounds
   dbm is consistent (checked by assertion)
   dbm is tight (checked by assertion)
   dim >= 1
   \post this has been initialized to one satisfying valuation in dbm
  */
  clockval_dbm_t(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim) : _dbm(nullptr), _dim(dim), _clockval(nullptr)
  {
    assert(dbm != nullptr);
    assert(tchecker::dbm::is_consistent(dbm, dim));
    assert(tchecker::dbm::is_tight(dbm, dim));
    assert(dim >= 1);

    _dbm = new tchecker::dbm::db_t[_dim * _dim];
    tchecker::dbm::copy(_dbm, dbm, _dim);

    _denominator = tchecker::dbm::constrain_to_single_valuation(_dbm, _dim);

    _clockval = tchecker::clockval_allocate_and_construct(_dim);
    compute_clock_valuation_from_dbm();
  }

  /*!
   \brief Copy constructor
  */
  clockval_dbm_t(tchecker::zg::path::concrete::clockval_dbm_t const & cvdbm)
      : _dbm(nullptr), _dim(cvdbm._dim), _denominator(cvdbm._denominator), _clockval(nullptr)
  {
    _dbm = new tchecker::dbm::db_t[_dim * _dim];
    tchecker::dbm::copy(_dbm, cvdbm._dbm, _dim);

    _clockval = tchecker::clockval_allocate_and_construct(_dim);
    for (tchecker::clock_id_t id = 0; id < _dim; ++id)
      (*_clockval)[id] = (*cvdbm._clockval)[id];
  }

  /*!
   \brief Move constructor
  */
  clockval_dbm_t(tchecker::zg::path::concrete::clockval_dbm_t && cvdbm)
      : _dbm(std::move(cvdbm._dbm)), _dim(std::move(cvdbm._dim)), _denominator(std::move(cvdbm._denominator)),
        _clockval(std::move(cvdbm._clockval))
  {
    cvdbm._dbm = nullptr;
    cvdbm._dim = 0;
    cvdbm._denominator = 0;
    cvdbm._clockval = nullptr;
  }

  /*!
   \brief Destructor
  */
  ~clockval_dbm_t()
  {
    delete[] _dbm;
    tchecker::clockval_destruct_and_deallocate(_clockval);
  }

  /*!
   \brief Assignment operator
  */
  tchecker::zg::path::concrete::clockval_dbm_t & operator=(tchecker::zg::path::concrete::clockval_dbm_t const & cvdbm)
  {
    if (&cvdbm != this) {
      if (_dim != cvdbm._dim) {
        delete[] _dbm;
        _dbm = new tchecker::dbm::db_t[_dim * _dim];

        tchecker::clockval_destruct_and_deallocate(_clockval);
        _clockval = tchecker::clockval_allocate_and_construct(_dim);
      }

      _dim = cvdbm._dim;
      _denominator = cvdbm._denominator;

      tchecker::dbm::copy(_dbm, cvdbm._dbm, _dim);

      for (tchecker::clock_id_t id = 0; id < _dim; ++id)
        (*_clockval)[id] = (*cvdbm._clockval)[id];
    }
    return *this;
  }

  /*!
   \brief Move assignment operator
  */
  tchecker::zg::path::concrete::clockval_dbm_t & operator=(tchecker::zg::path::concrete::clockval_dbm_t && cvdbm)
  {
    if (&cvdbm != this) {
      _dim = std::move(cvdbm._dim);
      _denominator = std::move(cvdbm._denominator);

      delete[] _dbm;
      _dbm = std::move(cvdbm._dbm);

      tchecker::clockval_destruct_and_deallocate(_clockval);
      _clockval = std::move(cvdbm._clockval);

      cvdbm._dim = 0;
      cvdbm._denominator = 0;
      cvdbm._dbm = nullptr;
      cvdbm._clockval = nullptr;
    }
    return *this;
  }

  /*!
   \brief Update to predecessor along a transition
   \param t : zone graph transition
   \param src_delay_allowed : delay allowed in source state of t
   \param tgt_delay_allowed : delay allowed in target state of t
   \param bounding_dbm : a DBM
   \pre bounding_dbm is not nullptr (checked by assertion)
   bounding_dbm is a _dim*_dim array of difference bounds
   bounding_dbm is consistent (checked by assertion)
   bounding_dbm is tight (checked by assertion)
   \post the clock valuation represented by its DBM has been updated to one of its predecessor
   within bounding_dbm, according to t, src_delay_allowed and tgt_delay_allowed if computation succeeded
   \return status of pre computation (see tchecker::zg::standard_semantics_t::prev)
   tchecker::STATE_BAD if no such valuation exists in bounding_dbm
   \note this object is invalid when computation fails
  */
  tchecker::state_status_t prev(tchecker::zg::transition_t const & t, bool src_delay_allowed, bool tgt_delay_allowed,
                                tchecker::dbm::db_t const * bounding_dbm)
  {
    assert(bounding_dbm != nullptr);
    assert(tchecker::dbm::is_consistent(bounding_dbm, _dim));
    assert(tchecker::dbm::is_tight(bounding_dbm, _dim));

    // scale up invariants, guard and reset to _denominator
    tchecker::clock_constraint_container_t scaled_up_src_invariant = scale_up(t.src_invariant_container());
    tchecker::clock_constraint_container_t scaled_up_tgt_invariant = scale_up(t.tgt_invariant_container());
    tchecker::clock_constraint_container_t scaled_up_guard = scale_up(t.guard_container());
    tchecker::clock_reset_container_t scaled_up_reset = scale_up(t.reset_container());

    // compute DBM of previous clock valiations according to t and delays
    tchecker::state_status_t prev_status =
        _semantics.prev(_dbm, _dim, src_delay_allowed, scaled_up_src_invariant, scaled_up_guard, scaled_up_reset,
                        tgt_delay_allowed, scaled_up_tgt_invariant);

    if (prev_status != tchecker::STATE_OK) {
      invalidate();
      return prev_status;
    }

    // restrict to scaled-up bounding_dbm
    enum tchecker::dbm::status_t bounding_status = restrict_to_scaled_up_bounding_dbm(bounding_dbm);

    if (bounding_status != tchecker::dbm::NON_EMPTY) {
      invalidate();
      return tchecker::STATE_BAD;
    }

    // restrict to single valuation
    simplify();
    _denominator *= tchecker::dbm::constrain_to_single_valuation(_dbm, _dim);
    compute_clock_valuation_from_dbm();

    return tchecker::STATE_OK;
  }

  /*!
   \brief Accessor
   \return the clock valuation represented by this DBM
  */
  inline tchecker::clockval_t const & clock_valuation() const { return *_clockval; }

  /*!
   \brief Accessor
   \return the internal DBM
  */
  inline tchecker::dbm::db_t const * dbm() const { return _dbm; }

  /*!
   \brief Accessor
   \return dimension
  */
  inline tchecker::clock_id_t dim() const { return _dim; }

private:
  /*!
   \brief fill clock valuation from DBM
   \pre _dbm is single valuation DBM
   \post _clockval is the single valuation in _dbm
  */
  void compute_clock_valuation_from_dbm()
  {
    std::vector<tchecker::integer_t> clock_values(_dim, 0);
    tchecker::dbm::satisfying_integer_valuation(_dbm, _dim, clock_values);
    for (tchecker::clock_id_t id = 0; id < _dim; ++id)
      (*_clockval)[id] = tchecker::clock_rational_value_t{clock_values[id], _denominator};
  }

  /*!
   \brief Simplify rational numbers in the DBM (_dbm, _denominator) while keeping
   a common denominator
  */
  void simplify()
  {
    tchecker::integer_t gcd = tchecker::dbm::gcd(_dbm, _dim);
    gcd = std::gcd(gcd, _denominator);
    if (gcd > 1) {
      tchecker::dbm::scale_down(_dbm, _dim, gcd);
      _denominator /= gcd;
    }
  }

  /*!
   \brief Restrict the internal DBM to scaled-up given DBM
   \post _dbm has been intersected with bounding_dbm scaled-up by _denominator
   \return tchecker::dbm::EMPTY if intersection is empty, tchecker::dbm::NON_EMPTY otherwise
  */
  enum tchecker::dbm::status_t restrict_to_scaled_up_bounding_dbm(tchecker::dbm::db_t const * bounding_dbm)
  {
    tchecker::dbm::db_t scaled_up_bounding_dbm[_dim * _dim];
    tchecker::dbm::copy(scaled_up_bounding_dbm, bounding_dbm, _dim);
    tchecker::dbm::scale_up(scaled_up_bounding_dbm, _dim, _denominator);
    return tchecker::dbm::intersection(_dbm, _dbm, scaled_up_bounding_dbm, _dim);
  }

  /*!
   \brief Invalidate this clock valuation DBM
   \post _dbm represents the empty zone, and _clockval has negative values
   \note used to invalidate this in case of error
  */
  void invalidate()
  {
    tchecker::dbm::zero(_dbm, _dim);
    for (tchecker::clock_id_t x = 0; x < _dim; ++x)
      (*_clockval)[x] = -1;
  }

  /*!
   \brief Scale up clock constraints
   \param cc : clock constraint container
   \return cc scaled up by _denominator
  */
  tchecker::clock_constraint_container_t scale_up(tchecker::clock_constraint_container_t const & cc) const
  {
    tchecker::clock_constraint_container_t scaled_up_cc;
    for (tchecker::clock_constraint_t const & c : cc)
      scaled_up_cc.emplace_back(c.id1(), c.id2(), c.comparator(), c.value() * _denominator);
    return scaled_up_cc;
  }

  /*!
   \brief Scale up clock resets
   \param reset: clock reset
   \return reset scaled up by _denominator
  */
  tchecker::clock_reset_container_t scale_up(tchecker::clock_reset_container_t const & reset) const
  {
    tchecker::clock_reset_container_t scaled_up_reset;
    for (tchecker::clock_reset_t const & r : reset)
      scaled_up_reset.emplace_back(r.left_id(), r.right_id(), r.value() * _denominator);
    return scaled_up_reset;
  }

  tchecker::dbm::db_t * _dbm;                    /*!< DBM with integer values */
  tchecker::clock_id_t _dim;                     /*!< Dimension of _dbm */
  tchecker::integer_t _denominator;              /*!< Denominator */
  tchecker::clockval_t * _clockval;              /*!< Clock valuation */
  tchecker::zg::standard_semantics_t _semantics; /*!< Zone graph semantics */
};

tchecker::zg::path::concrete::finite_path_t *
compute_finite_path(tchecker::zg::path::symbolic::finite_path_t const & symbolic_run)
{
  tchecker::zg::path::concrete::finite_path_t * concrete_run =
      new tchecker::zg::path::concrete::finite_path_t{symbolic_run.zg_ptr(), 1024};

  tchecker::zg::path::symbolic::finite_path_t::node_sptr_t const first_symbolic_node{symbolic_run.first()};
  tchecker::zg::path::symbolic::finite_path_t::node_sptr_t symbolic_node{symbolic_run.last()};

  tchecker::zg::path::concrete::clockval_dbm_t clockval_dbm{
      symbolic_node->state().zone().dbm(), static_cast<tchecker::clock_id_t>(symbolic_node->state().zone().dim())};

  // build final concrete node
  concrete_run->add_first_node(symbolic_node->state_ptr(), clockval_dbm.clock_valuation());

  if (symbolic_node == first_symbolic_node) {
    // path is just one node
    concrete_run->first()->initial(symbolic_run.first()->initial());
    concrete_run->last()->final(symbolic_run.last()->final());
    return concrete_run;
  }

  // concretise all other nodes
  do {
    // compute prev(clockval_dbm) along symbolic_edge
    bool tgt_delay_allowed = tchecker::ta::delay_allowed(symbolic_run.zg().system(), symbolic_node->state().vloc());

    tchecker::zg::path::symbolic::finite_path_t::edge_sptr_t symbolic_edge = symbolic_run.incoming_edge(symbolic_node);
    symbolic_node = symbolic_run.edge_src(symbolic_edge);

    bool src_delay_allowed = tchecker::ta::delay_allowed(symbolic_run.zg().system(), symbolic_node->state().vloc());

    tchecker::state_status_t status = clockval_dbm.prev(symbolic_edge->transition(), src_delay_allowed, tgt_delay_allowed,
                                                        symbolic_node->state().zone().dbm());
    if (status != tchecker::STATE_OK) {
      delete concrete_run;
      throw std::runtime_error("Symbolic run concretization failed: previous state computation failed");
    }

    // compute delay
    tchecker::clock_rational_value_t const delay =
        tchecker::delay(clockval_dbm.clock_valuation(), symbolic_edge->transition().src_invariant_container(),
                        symbolic_edge->transition().guard_container(), symbolic_edge->transition().reset_container(),
                        concrete_run->first()->clockval());
    if (delay < 0) {
      delete concrete_run;
      throw std::runtime_error("Symbolic run concretization failed: negative delay");
    }

    // extend concrete run
    concrete_run->extend_front(symbolic_edge->transition_ptr(), delay, symbolic_node->state_ptr(),
                               clockval_dbm.clock_valuation());

  } while (symbolic_node != first_symbolic_node);

  concrete_run->first()->initial(symbolic_run.first()->initial());
  concrete_run->last()->final(symbolic_run.last()->final());

  return concrete_run;
}

} // namespace concrete

} // namespace path

} // namespace zg

} // namespace tchecker
