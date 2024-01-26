/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/refzg/transition.hh"

namespace tchecker {

namespace refzg {

/* transition_t */

transition_t::transition_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge)
    : tchecker::ta::transition_t(vedge)
{
}

transition_t::transition_t(tchecker::refzg::transition_t const & t,
                           tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge)
    : tchecker::ta::transition_t(t, vedge)
{
}

/* functions */

bool operator==(tchecker::refzg::transition_t const & t1, tchecker::refzg::transition_t const & t2)
{
  return tchecker::ta::operator==(t1, t2);
}

bool shared_equal_to(tchecker::refzg::transition_t const & t1, tchecker::refzg::transition_t const & t2)
{
  return tchecker::ta::shared_equal_to(t1, t2);
}

std::size_t hash_value(tchecker::refzg::transition_t const & t) { return tchecker::ta::hash_value(t); }

std::size_t shared_hash_value(tchecker::refzg::transition_t const & t) { return tchecker::ta::shared_hash_value(t); }

int lexical_cmp(tchecker::refzg::transition_t const & t1, tchecker::refzg::transition_t const & t2)
{
  return tchecker::ta::lexical_cmp(t1, t2);
}

} // end of namespace refzg

} // end of namespace tchecker
