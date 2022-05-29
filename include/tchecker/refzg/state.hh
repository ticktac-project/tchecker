/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_REFZG_STATE_HH
#define TCHECKER_REFZG_STATE_HH

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/refzg/zone.hh"
#include "tchecker/ta/state.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file state.hh
 \brief State of a zone graph with reference clocks
 */

namespace tchecker {

namespace refzg {

/*!
\brief Type of shared zone, DBM implementation
*/
using shared_zone_t = tchecker::make_shared_t<tchecker::refzg::zone_t>;

/*!
 \class state_t
 \brief state of a zone graph with reference clocks
 */
class state_t : public tchecker::ta::state_t {
public:
  /*!
   \brief Constructor
   \param vloc : tuple of locations
   \param intval : integer variables valuation
   \param zone : zone with reference clocks
   \pre vloc, intval and zone must not point to nullptr (checked by assertion)
   */
  explicit state_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                   tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                   tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone);

  /*!
   \brief Partial copy constructor
   \param state : a state
   \param vloc : tuple of locations
   \param intval : integer variables valuation
   \param zone : zone with reference clocks
   \pre vloc, intval and zone must not point to nullptr (checked by assertion)
   \note the state is copied from s, except the tuple of locations which is
   initialized from vloc, and the valuation of bounded integer variables which
   is initialized from intval, and the zone which is initialized from zone
   */
  state_t(tchecker::ta::state_t const & s, tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
          tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
          tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone);

  /*!
   \brief Copy constructor (deleted)
   */
  state_t(tchecker::refzg::state_t const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  state_t(tchecker::refzg::state_t &&) = delete;

  /*!
   \brief Destructor
   */
  ~state_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::refzg::state_t & operator=(tchecker::refzg::state_t const &) = default;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::refzg::state_t & operator=(tchecker::refzg::state_t &&) = default;

  /*!
  \brief Accessor
  \return zone in this state
  */
  constexpr inline tchecker::refzg::zone_t const & zone() const { return *_zone; }

  /*!
  \brief Accessor
  \return reference to pointer to the zone in this state
  */
  inline tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> & zone_ptr() { return _zone; }

  /*!
  \brief Accessor
  \return pointer to const zone in this state
  */
  inline tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t const> zone_ptr() const
  {
    return tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t const>(_zone.ptr());
  }

private:
  tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> _zone; /*!< Zone with reference clocks */
};

/*!
 \brief Equality check
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have same zone, same tuple of locations and same
 valuation of bounded integer variables, false otherwise
 */
bool operator==(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2);

/*!
 \brief Disequality check
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have different zones, or different tuples of
 locations or different valuations of bounded integer variables, false otherwise
 */
bool operator!=(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2);

/*!
 \brief Covering check
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have the same tuple of locations and integer
 variables valuation, and the zone in s1 is included in the zone in s2, false
 otherwise
*/
bool operator<=(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2);

/*!
 \brief aLU* subsumption check
 \param l : clock lower bounds
 \param u : clock upper bounds
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have the same tuple of locations and integer
 variables valuation, and the zone in s1 is included in aLU*-abstraction of the
 zone in s2, false otherwise
*/
bool is_alu_star_le(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2,
                    tchecker::clockbounds::map_t const & l, tchecker::clockbounds::map_t const & u);

/*!
 \brief Subsumption check for aLU* combined with time-elapse
 \param l : clock lower bounds
 \param u : clock upper bounds
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have the same tuple of locations and integer
 variables valuation, and the time-elapsed zone of the zone in s1 is included in aLU*-abstraction of the
 time-elapsed zone of the zone in s2, false otherwise
*/
bool is_time_elapse_alu_star_le(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2,
                                tchecker::clockbounds::map_t const & l, tchecker::clockbounds::map_t const & u);

/*!
 \brief Sync-subsumption check
 \param l : clock lower bounds
 \param u : clock upper bounds
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have the same tuple of locations and integer
 variables valuation, and the zone in s1 is sync-subsumed by the zone in s2,
 false otherwise
*/
bool is_sync_alu_le(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2,
                    tchecker::clockbounds::map_t const & l, tchecker::clockbounds::map_t const & u);

/*!
 \brief Hash
 \param s : state
 \return Hash value for state s
 */
std::size_t hash_value(tchecker::refzg::state_t const & s);

/*!
 \brief Lexical ordering on states of the local-time zone graph
 \param s1 : a state
 \param s2 : a state
 \return 0 if s1 and s2 are equal, a negative value if s1 is smaller than s2
 w.r.t. lexical ordering on zone,tuple of locations and valuation of bounded
 integer variables, a positive value otherwise
 */
int lexical_cmp(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2);

/*!
 \brief Type of shared state
 */
using shared_state_t = tchecker::make_shared_t<tchecker::refzg::state_t>;

/*!
\brief Type of pointer to shared state
*/
using state_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_state_t>;

/*!
\brief Type of pointer to shared const state
*/
using const_state_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_state_t const>;

} // end of namespace refzg

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::refzg::state_t
 */
template <> class allocation_size_t<tchecker::refzg::state_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::refzg::state_t
   \return allocation size for objects of class tchecker::refzg::state_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... args) { return sizeof(tchecker::refzg::state_t); }
};

} // end of namespace tchecker

#endif // TCHECKER_REFZG_STATE_HH
