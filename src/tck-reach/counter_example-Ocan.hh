/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_REACH_COUNTER_EXAMPLE_HH
#define TCHECKER_TCK_REACH_COUNTER_EXAMPLE_HH

#include <memory>
#include <vector>

#include "tchecker/algorithms/path/algorithm.hh"
#include "tchecker/refzg/path.hh"
#include "tchecker/refzg/refzg.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/zg/path.hh"
#include "tchecker/zg/zg.hh"
#include "tchecker/utils/log.hh"
/*!
 \file counter_example.hh
 \brief Generation of counter examples
*/

namespace tchecker {

namespace tck_reach {

/*!
 \brief Check if a node is initial
 \tparam GRAPH : type of graph
 \param n : a node
 \return true if n is an initial node, false otherwise
*/
template <class GRAPH> bool initial_node(typename GRAPH::node_sptr_t const & n) { return n->initial(); }

/*!
 \brief Check is a node is final
 \tparam GRAPH : type of graph
 \param n : a node
 \return true if n is a final node, false otherwise
*/
template <class GRAPH> bool final_node(typename GRAPH::node_sptr_t const & n) { return n->final(); }

/*!
 \brief True predicate over edges
 \tparam GRAPH : type of graph
 \param e : an edge
 \return true
*/
template <class GRAPH> bool true_edge(typename GRAPH::edge_sptr_t const & e) { return true; }

/*!
 \brief Compute a counter example over the zone graph
 \tparam GRAPH : type of graph, see tchecker::algorithms::path::finite::algorithm_t for requirements
 \tparam CEX : type of counter example, should inherit from tchecker::zg::path::finite_path_t
 \param g : a graph over the zone graph (reachability graph, subsumption graph, etc)
 \return a finite path from an initial node of g to a final node of g
 */
template <class GRAPH, class CEX> CEX * counter_example_zg(GRAPH const & g)
{
  std::shared_ptr<tchecker::zg::zg_t> zg{
      tchecker::zg::factory(g.zg().system_ptr(), tchecker::zg::STANDARD_SEMANTICS, tchecker::zg::NO_EXTRAPOLATION, 128, 128)};

  // compute sequence of edges from initial to final node in g
  tchecker::algorithms::path::finite::algorithm_t<GRAPH> algorithm;

  std::vector<typename GRAPH::edge_sptr_t> seq =
      algorithm.run(g, &tchecker::tck_reach::initial_node<GRAPH>, &tchecker::tck_reach::final_node<GRAPH>,
                    &tchecker::tck_reach::true_edge<GRAPH>);

  if (seq.empty())
    return new CEX{zg};

  // Extract sequence of vedges
  std::vector<tchecker::const_vedge_sptr_t> vedge_seq;
  for (typename GRAPH::edge_sptr_t const & e : seq)
    vedge_seq.push_back(e->vedge_ptr());

  // Get the corresponding run in a zone graph with standard semantics and no extrapolation
  tchecker::vloc_t const & initial_vloc = g.edge_src(seq[0])->state().vloc();
  CEX * cex = tchecker::zg::path::compute_run(zg, initial_vloc, tchecker::make_range(vedge_seq));
  if (!cex->empty()) {
    cex->first()->initial(true);
    cex->last()->final(true);
  }

  return cex;
}

/*!
 \brief Compute a counter example over the zone graph with reference clocks
 \tparam GRAPH : type of graph, see tchecker::algorithms::path::finite::algorithm_t for requirements
 \tparam CEX : type of counter example, should inherit from tchecker::refzg::path::finite_path_t
 \param g : a graph over the zone graph (reachability graph, subsumption graph, etc)
 \return a finite path from an initial node of g to a final node of g
 */
template <class GRAPH, class CEX> CEX * counter_example_refzg(GRAPH const & g)
{
  std::shared_ptr<tchecker::refzg::refzg_t> refzg{
      tchecker::refzg::factory(g.refzg().system_ptr(), tchecker::refzg::PROCESS_REFERENCE_CLOCKS,
                               tchecker::refzg::STANDARD_SEMANTICS, g.refzg().spread(), 128, 128)};

  // compute sequence of edges from initial to final node in g
  tchecker::algorithms::path::finite::algorithm_t<GRAPH> algorithm;

  std::vector<typename GRAPH::edge_sptr_t> seq =
      algorithm.run(g, &tchecker::tck_reach::initial_node<GRAPH>, &tchecker::tck_reach::final_node<GRAPH>,
                    &tchecker::tck_reach::true_edge<GRAPH>);

  if (seq.empty())
    return new CEX{refzg};

  // Extract sequence of vedges
  std::vector<tchecker::const_vedge_sptr_t> vedge_seq;
  for (typename GRAPH::edge_sptr_t const & e : seq)
    vedge_seq.push_back(e->vedge_ptr());

  // Get the corresponding run in a zone graph with standard semantics and no extrapolation
  tchecker::vloc_t const & initial_vloc = g.edge_src(seq[0])->state().vloc();
  CEX * cex = tchecker::refzg::path::compute_run(refzg, initial_vloc, tchecker::make_range(vedge_seq));
  if (!cex->empty()) {
    cex->first()->initial(true);
    cex->last()->final(true);
  }

  return cex;
}

typedef std::vector<double> valuation;

/**
 * @brief DBMs with rational coefficients represented by a pair (dbm, factor), where
 * dbm is the matrix of numerators, and factor is the common denominator of all components of dbm.
 * 
 */
class rational_dbm_t {
public:
  rational_dbm_t(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t factor = 1) : _dbm(dbm), _dim(dim), _factor(factor){}
  rational_dbm_t(const tchecker::tck_reach::rational_dbm_t & rdbm) {
    this->operator=(rdbm);
  }
  void operator=(const tchecker::tck_reach::rational_dbm_t & rdbm){
    this->_factor = rdbm.factor();
    this->_dim = rdbm.dimension();
    for (tchecker::clock_id_t c = 0; c < _dim; c++) {
      for (tchecker::clock_id_t d = 0; d < _dim; d++) {
        this->_dbm[d*_dim +c] = rdbm.dbm()[d*_dim + c];
      }
    }
  }

  /*!
  \brief Constrains the rational dbm to a single valuation possibly after changing the scale factor.
  \param rdbm : rational dbm
  \pre dbm is not nullptr (checked by assertion)
  dbm is a dim*dim array of difference bounds
  dbm is consistent (checked by assertion)
  dbm is tight (checked by assertion)
  dim >= 1 (checked by assertion).
  factor >= 1 (checked by assertion)
  \post dbm represents a single valuation.
  \note this can increase factor in case of tight strict constraints
  */
  void constrain_to_valuation();

  /*!
  \brief Get the valuation representation represented by the rational dbm.
  \pre dbm represents a single valuation, that is, is_single_valuation() holds
  */
  tchecker::tck_reach::valuation get_valuation() const;

  /*!
    \brief Whether the rational dbm represents a single valuation
  */
  bool is_single_valuation() const;

  /**
   * @brief Scale down all components of dbm by multiplying them by div when div is the largest number d that can be written as factor/SCALE_FACTOR^i
   * for i in {0,1,2,...}, and which divides all components; and return the new scale factor.
   * The input arguments represent a dbm with rational coefficients dbm / factor; and the post condition is that this is equal to dbm' / factor'
   * where dbm' is dbm after the call, and factor' is the return value.
   *
   * @post rdbm's factor becomes factor/d where d is the quantity by which all components were divided.
   */
  void simplify();

  const tchecker::dbm::db_t * dbm() const { return _dbm; }
  tchecker::dbm::db_t * dbm(){ return _dbm; }
  tchecker::clock_id_t dimension() const { return _dim; }
  tchecker::integer_t factor() const { return _factor; }

private:  
  tchecker::dbm::db_t * _dbm;
  tchecker::clock_id_t _dim;
  tchecker::integer_t _factor;
  static const tchecker::integer_t SCALE_FACTOR = 10;
};


/**
 * \brief Compute a valuation that is a predecessor of val through transition, and that is inside predecessor_zone.
 * \param val : a rational dbm representing a single valuation
 * \param transition : the transition through which predecessor is to be computed
 * \param tgt_delayed_allowed : whether delaying is allowed in the target location of the transition
 * \param predecessor_zone : zone in which a predecessor is to be found
 * \param concrete_predecessor_reset : a rational dbm in which will contain the image of concrete_predecessor
 * by the reset of transition.
 * \pre val admits a predecessor inside predecessor_zone
 * \pre val represents a single valuation (checked by assertion)
 * \post val |= g /\ predecessor_zone, concrete_predecessor_reset = val[R := 0], 
 * and exists d, val = concrete_predecessor_reset + d
 * where g is the guard, and R the reset of the transition; and such that if !tgt_delay_allowed, then d=0.
 */
void concrete_predecessor(tchecker::tck_reach::rational_dbm_t & val,
                          const tchecker::zg::transition_t & transition, bool tgt_delay_allowed,
                          const tchecker::dbm::db_t * predecessor_zone,
                          tchecker::tck_reach::rational_dbm_t & concrete_predecessor_reset);

/*!
  \brief Generates a vector of rational valuations from a symbolic counterexample.
  This computes a sequence {v_0, v_1, v_2, v_3, ...} of valuations with the following properties:\n 
  - v0 is the 0 vector\n 
  - for each i, v_{2i+1} = v_{2i} + d for some d>=0\n 
  - v_{2i+1} satisfies the guard of the i-th edge\n 
  - v_{2i+2} is obtained from v_{2i+1} by applying the reset of the i-th edge\n 
  In other terms, {v_0, v_2, v_4, ... } is a run of cex, and the v_{2i+1} are the intermediate valuations after a delay, and before the edge is applied.
*/
template <class CEX> 
std::vector<tchecker::tck_reach::valuation>
generate_concrete_trace(CEX & cex, std::shared_ptr<tchecker::ta::system_t const> system)
{
  auto dim = system->clocks_count(tchecker::variable_kind_t::VK_FLATTENED) + 1;
  std::vector<tchecker::tck_reach::valuation> concrete_trace;
  tchecker::dbm::db_t * vdbm = new tchecker::dbm::db_t[dim * dim];
  tchecker::dbm::db_t * vdbm_reset = new tchecker::dbm::db_t[dim * dim];  
  memcpy(vdbm, cex.last()->state_ptr()->zone().dbm(), dim * dim * sizeof(tchecker::dbm::db_t));
  tchecker::tck_reach::rational_dbm_t vrdbm(vdbm, dim, 1);
  tchecker::tck_reach::rational_dbm_t vrdbm_reset(vdbm_reset, dim, 1);

  try {
    vrdbm.constrain_to_valuation();
    concrete_trace.push_back(vrdbm.get_valuation());

    for (auto n = cex.last(); n != cex.first();) {
      auto e = cex.incoming_edge(n);
      auto prev_n = cex.edge_src(e);
      tchecker::zg::transition_t const & transition = e->transition();
      bool tgt_delay_allowed = tchecker::ta::delay_allowed(*system, *n->state_ptr()->vloc_ptr());
      concrete_predecessor(vrdbm, transition, tgt_delay_allowed, prev_n->state_ptr()->zone().dbm(), vrdbm_reset);
      concrete_trace.push_back(vrdbm_reset.get_valuation());
      concrete_trace.push_back(vrdbm.get_valuation());
      n = prev_n;
    }
    tchecker::tck_reach::valuation init_val;
    for(tchecker::clock_id_t x = 1; x < dim; x++){
      init_val.push_back(0);
    }
    concrete_trace.push_back(init_val);
    std::reverse(concrete_trace.begin(), concrete_trace.end());
  } catch (tchecker::dbm::overflow & ex) {
    std::cerr << tchecker::log_warning << "Cannot compute concrete trace: " << ex.what() << "\n";
    concrete_trace.clear();
  }
  delete[] vdbm_reset;
  delete[] vdbm;
  
  return concrete_trace;
}


}
} // namespace tchecker

#endif // TCHECKER_TCK_REACH_COUNTER_EXAMPLE_HH
