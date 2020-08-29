/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_SYSTEM_HH
#define TCHECKER_TA_SYSTEM_HH

#include <memory>
#include <unordered_map>
#include <vector>

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/expression/typed_expression.hh"
#include "tchecker/parsing/declaration.hh"
#include "tchecker/statement/typed_statement.hh"
#include "tchecker/syncprod/system.hh"
#include "tchecker/system/attribute.hh"
#include "tchecker/system/system.hh"
#include "tchecker/ta/label.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/vm/vm.hh"

/*!
 \file system.hh
 \brief System of processes for timed automata
 */

namespace tchecker {

namespace ta {

/*!
 \class system_t
 \brief System of processes for timed automata
 */
class system_t : private tchecker::syncprod::system_t,
                 private tchecker::ta::labels_t,
                 private tchecker::clockbounds::clockbounds_t {
public:
  /*!
   \brief Constructor
   \param sysdecl : system declaration
   */
  system_t(tchecker::parsing::system_declaration_t const & sysdecl);

  /*!
   \brief Constructor
   \param system : system of timed processes
   \post this is a copy of system
   */
  system_t(tchecker::system::system_t const & system);

  /*!
   \brief Constructor
   \param system : system of timed processes
   \post this is a copy of system
   */
  system_t(tchecker::syncprod::system_t const & system);

  /*!
   \brief Copy constructor
   */
  system_t(tchecker::ta::system_t const &);

  /*!
   \brief Move constructor
   */
  system_t(tchecker::ta::system_t &&) = default;

  /*!
   \brief Destructor
   */
  ~system_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::ta::system_t & operator=(tchecker::ta::system_t const &);

  /*!
   \brief Move-assignment operator
   */
  tchecker::ta::system_t & operator=(tchecker::ta::system_t &&) = default;

  // System
  using tchecker::syncprod::system_t::attributes;
  using tchecker::syncprod::system_t::name;

  // Clocks
  using tchecker::syncprod::system_t::clock_attributes;
  using tchecker::syncprod::system_t::clock_id;
  using tchecker::syncprod::system_t::clock_name;
  using tchecker::syncprod::system_t::clock_variables;
  using tchecker::syncprod::system_t::clocks_count;
  using tchecker::syncprod::system_t::is_clock;

  // Clock bounds
  using tchecker::clockbounds::clockbounds_t::global_lu;
  using tchecker::clockbounds::clockbounds_t::global_m;
  using tchecker::clockbounds::clockbounds_t::local_lu;
  using tchecker::clockbounds::clockbounds_t::local_m;

  /*!
  \brief Accessor
  \return Clock bounds maps
  */
  tchecker::clockbounds::clockbounds_t const & clockbounds() const { return *this; }

  /*!
   \brief Check clock bounds
   \return true if clock bounds have been computed for this system, false otherwise
  */
  inline bool has_clockbounds() const { return _has_clockbounds; }

  // Edges
  using tchecker::syncprod::system_t::asynchronous_incoming_edges;
  using tchecker::syncprod::system_t::asynchronous_outgoing_edges;
  using tchecker::syncprod::system_t::edge;
  using tchecker::syncprod::system_t::edges;
  using tchecker::syncprod::system_t::edges_count;
  using tchecker::syncprod::system_t::incoming_edges;
  using tchecker::syncprod::system_t::incoming_edges_maps;
  using tchecker::syncprod::system_t::incoming_event;
  using tchecker::syncprod::system_t::is_asynchronous;
  using tchecker::syncprod::system_t::is_edge;
  using tchecker::syncprod::system_t::outgoing_edges;
  using tchecker::syncprod::system_t::outgoing_edges_maps;
  using tchecker::syncprod::system_t::outgoing_event;

  /*!
   \brief Accessor
   \param id : edge identifier
   \pre id is an edge identifier (checked by assertion)
   \return guard typed expression for edge id
   */
  tchecker::typed_expression_t const & guard(tchecker::edge_id_t id) const;

  /*!
   \brief Accessor
   \param id : edge identifier
   \pre id is an edge identifier (checked by assertion)
   \return guard bytecode for edge id
   */
  tchecker::bytecode_t const * guard_bytecode(tchecker::edge_id_t id) const;

  /*!
   \brief Accessor
   \param id : edge identifier
   \pre id is an edge identifier (checked by assertion)
   \return type statement for edge id
   */
  tchecker::typed_statement_t const & statement(tchecker::edge_id_t id) const;

  /*!
   \brief Accessor
   \param id : edge identifier
   \pre id is an edge identifier (checked by assertion)
   \return statement bytecode for edge id
   */
  tchecker::bytecode_t const * statement_bytecode(tchecker::edge_id_t id) const;

  // Events
  using tchecker::syncprod::system_t::event_attributes;
  using tchecker::syncprod::system_t::event_id;
  using tchecker::syncprod::system_t::event_name;
  using tchecker::syncprod::system_t::events_count;
  using tchecker::syncprod::system_t::is_event;

  // Bounded integer variables
  using tchecker::syncprod::system_t::integer_variables;
  using tchecker::syncprod::system_t::intvar_attributes;
  using tchecker::syncprod::system_t::intvar_id;
  using tchecker::syncprod::system_t::intvar_name;
  using tchecker::syncprod::system_t::intvars_count;
  using tchecker::syncprod::system_t::is_intvar;

  // Labels
  using tchecker::ta::labels_t::is_label;
  using tchecker::ta::labels_t::label_id;
  using tchecker::ta::labels_t::label_name;
  using tchecker::ta::labels_t::labels_count;

  // Locations
  using tchecker::syncprod::system_t::initial_locations;
  using tchecker::syncprod::system_t::is_location;
  using tchecker::syncprod::system_t::location;
  using tchecker::syncprod::system_t::locations;
  using tchecker::syncprod::system_t::locations_count;

  /*!
   \brief Accessor
   \param id : location identifier
   \pre id is a location identifier (checked by assertion)
   \return true if location id is committed, false otherwise
   */
  bool is_committed(tchecker::loc_id_t id) const;

  /*!
   \brief Accessor
   \param id : location identifier
   \pre id is a location identifier (checked by assertion)
   \return true if location id is urgent, false otherwise
   */
  bool is_urgent(tchecker::loc_id_t id) const;

  /*!
   \brief Type of labels iterator
   */
  using labels_const_iterator_t = std::unordered_multimap<tchecker::loc_id_t, tchecker::label_id_t>::const_iterator;

  /*!
   \brief Accessor
   \param id : location identifier
   \pre id is a location identifier (checked by assertion)
   \return range of labels on location id
   */
  tchecker::range_t<labels_const_iterator_t> labels(tchecker::loc_id_t id) const;

  /*!
   \brief Accessor
   \param id : location identifier
   \pre id is a location identifier (checked by assertion)
   \return invariant typed expression for location id
   */
  tchecker::typed_expression_t const & invariant(tchecker::loc_id_t id) const;

  /*!
   \brief Accessor
   \param id : location identifier
   \pre id is a location identifier (checked by assertion)
   \return invariant bytecode for location id
   */
  tchecker::bytecode_t const * invariant_bytecode(tchecker::loc_id_t id) const;

  // Processes
  using tchecker::syncprod::system_t::is_process;
  using tchecker::syncprod::system_t::process_attributes;
  using tchecker::syncprod::system_t::process_id;
  using tchecker::syncprod::system_t::process_name;
  using tchecker::syncprod::system_t::processes_count;

  // Synchronizations
  using tchecker::syncprod::system_t::synchronization;
  using tchecker::syncprod::system_t::synchronizations;
  using tchecker::syncprod::system_t::synchronizations_count;

  // Virtual machine
  inline tchecker::vm_t & vm() const { return _vm; }

  // Cast
  using tchecker::syncprod::system_t::as_system_system;

  /*!
   \brief Cast
   \return this as a tchecker::syncprod::system_t instance
   */
  constexpr inline tchecker::syncprod::system_t const & as_syncprod_system() const { return *this; }

private:
  // Hidden modifiers and accessors
  using tchecker::clockbounds::clockbounds_t::clear;
  using tchecker::clockbounds::clockbounds_t::global_lu_map;
  using tchecker::clockbounds::clockbounds_t::global_m_map;
  using tchecker::clockbounds::clockbounds_t::local_lu_map;
  using tchecker::clockbounds::clockbounds_t::local_m_map;
  using tchecker::clockbounds::clockbounds_t::resize;

  using tchecker::ta::labels_t::add_label;

  /*!
   \brief Typed and compiled expression
   */
  struct compiled_expression_t {
    std::shared_ptr<tchecker::typed_expression_t> _typed_expr; /*!< Typed expression */
    std::shared_ptr<tchecker::bytecode_t> _compiled_expr;      /*!< Compiled expression */
  };

  /*!
   \brief Typed and compiled statement
   */
  struct compiled_statement_t {
    std::shared_ptr<tchecker::typed_statement_t> _typed_stmt; /*!< Typed statement */
    std::shared_ptr<tchecker::bytecode_t> _compiled_stmt;     /*!< Compiled statement */
  };

  /*!
   \brief Compute data from syncprod::system_t
   */
  void compute_from_syncprod_system();

  /*!
  \brief Compute clock bounds
  \post sets clock bounds as well as flag _has_clockbounds that tells if clock bounds could be
  determined for this system
  */
  void compute_clockbounds();

  /*!
   \brief Set location invariant
   \param id : location identifier
   \param invariants : range of invariants (as strings)
   \post all invariants have been parsed, typed, compiled and set as location id invariant
   \note all compilation errors have been reported to tchecker::log
   \note previous invariant (if any) has been replaced by the new one
   \throw std::invalid_argument : if compilation of invariants fails
   */
  void set_invariant(tchecker::loc_id_t id,
                     tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & invariants);

  /*!
   \brief Set location labels
   \param id : location identifier
   \param labels : range of labels (as strings)
   \post location is is labelled by labels
   */
  void set_labels(tchecker::loc_id_t id, tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & labels);

  /*!
   \brief Set location committed flag
   \param id : location identifier
   \param flags : range of committed flags
   \post location has been set committed if flags if not empty
   */
  void set_committed(tchecker::loc_id_t id, tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & flags);

  /*!
   \brief Set location urgent flag
   \param id : location identifier
   \param flags : range of urgent flags
   \post location has been set urgent if flags if not empty
   */
  void set_urgent(tchecker::loc_id_t id, tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & flags);

  /*!
   \brief Set edge guard (provided attribute)
   \param id : location identifier
   \param guards : range of guards (as strings)
   \post all guards have been parsed, typed, compiled and set as edge id guard
   \note all compilation errors have been reported to tchecker::log
   \note previous guard (if any) has been replaced by the new one
   \throw std::invalid_argument : if compilation of guards fails
   */
  void set_guards(tchecker::edge_id_t id, tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & guards);

  /*!
   \brief Set edge statement (do attribute)
   \param id : edge identifier
   \param statements : range of statements (as strings)
   \post all statements have been parsed, typed, compiled and set as edge id statement
   \note all compilation errors have been reported to tchecker::log
   \note previous statement (if any) has been replaced by the new one
   \throw std::invalid_argument : if compilation of statements fails
   */
  void set_statements(tchecker::edge_id_t id,
                      tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & statements);

  mutable tchecker::vm_t _vm;                                                /*!< Bytecode interpreter */
  std::vector<compiled_expression_t> _invariants;                            /*!< Map : location identifier -> invariant */
  std::vector<compiled_expression_t> _guards;                                /*!< Map : edge identifier -> guard */
  std::vector<compiled_statement_t> _statements;                             /*!< Map : edge identifier -> statement */
  std::unordered_multimap<tchecker::loc_id_t, tchecker::label_id_t> _labels; /*!< Map : location identifier -> labels */
  boost::dynamic_bitset<> _committed;                                        /*!< Committed locations */
  boost::dynamic_bitset<> _urgent;                                           /*!< Urgent locations */
  bool _has_clockbounds;                                                     /*!< Flag if clock bounds could be computed */
};

} // end of namespace ta

} // end of namespace tchecker

#endif // TCHECKER_TA_SYSTEM_HH
