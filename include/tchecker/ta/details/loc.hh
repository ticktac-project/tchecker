/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_LOC_HH
#define TCHECKER_TA_DETAILS_LOC_HH

#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/expression/expression.hh"
#include "tchecker/fsm/details/loc.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file loc.hh
 \brief Locations for timed automata
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \class loc_t
       \tparam EDGE : type of edge, should derive from
       tchecker::ta::details::edge_t
       \brief Location
       */
      template <class EDGE>
      class loc_t : public tchecker::fsm::details::loc_t<EDGE> {
      public:
        /*!
         \brief Constructor
         \tparam LABELS_ITER : iterator over labels
         \param pid : process identifier
         \param id : location identifier
         \param name : location name
         \param initial : initial location flag
         \param committed : committed location flag
         \param urgent : urgent location flag
         \param invariant : location invariant
         \param labels : location labels
         \pre committed and urgent are not both set, see tchecker::loc_t for
         extra precondition
         \throw std::invalid_argument : if the precondition is violated
         \note see tchecker::loc_t for more information
         */
        template <class LABELS_ITER>
        loc_t(tchecker::process_id_t pid,
              tchecker::loc_id_t id,
              std::string const & name,
              bool initial,
              bool committed,
              bool urgent,
              tchecker::expression_t * invariant,
              tchecker::range_t<LABELS_ITER> const & labels)
        : tchecker::fsm::details::loc_t<EDGE>(pid, id, name, initial, invariant, labels),
        _committed(committed),
        _urgent(urgent)
        {
          if (_committed && _urgent)
            throw std::invalid_argument("error, locations cannot be both committed and urgent");
        }
        
        /*!
         \brief Copy constructor
         \param loc : location
         \post this is a copy of loc
         */
        loc_t(tchecker::ta::details::loc_t<EDGE> const & loc)
        : tchecker::fsm::details::loc_t<EDGE>(loc),
        _committed(loc._committed),
        _urgent(loc._urgent)
        {}
        
        /*!
         \brief Move constructor
         */
        loc_t(tchecker::ta::details::loc_t<EDGE> &&) = default;
        
        /*!
         \brief Destructor
         */
        ~loc_t() = default;
        
        /*!
         \brief Assignment operator
         \param loc : location
         \post this is copy of loc
         */
        tchecker::ta::details::loc_t<EDGE> & operator= (tchecker::ta::details::loc_t<EDGE> const & loc)
        {
          if (this != loc) {
            this->tchecker::fsm::details::loc_t<EDGE>::operator=(loc);
            _committed = loc._committed;
            _urgent = loc._urgent;
          }
          return *this;
        }
        
        /*!
         \brief Move assignment operator
         */
        tchecker::ta::details::loc_t<EDGE> & operator= (tchecker::ta::details::loc_t<EDGE> &&) = default;
        
        /*!
         \brief Accessor
         \return committed location flag
         */
        inline bool committed() const
        {
          return _committed;
        }
        
        /*!
         \brief Accessor
         \return urgent location flag
         */
        inline bool urgent() const
        {
          return _urgent;
        }
        
        /*!
         \brief Accessor
         \return true is time delay is allowed in this location, false otherwise
         */
        inline bool delay_allowed() const
        {
          return (! _committed) && (! _urgent);
        }
      protected:
        bool _committed;   /*!< Committed location flag */
        bool _urgent;      /*!< Urgent location flag */
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_LOC_HH

