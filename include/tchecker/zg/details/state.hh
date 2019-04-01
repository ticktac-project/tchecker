/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_DETAILS_STATE_HH
#define TCHECKER_ZG_DETAILS_STATE_HH

#include "tchecker/ta/details/state.hh"

/*!
 \file state.hh
 \brief State of a zone graph (details)
 */

namespace tchecker {
  
  namespace zg {
    
    namespace details {
      
      /*!
       \class state_t
       \brief TA's state (details)
       \tparam VLOC : type of tuple of locations
       \tparam INTVARS_VAL : type of integer variables valuations
       \tparam ZONE : type of zone
       \tparam VLOC_PTR : type of pointer to tuple of locations
       \tparam INTVARS_VAL_PTR : type of pointer to integer variables valuation
       \tparam ZONE_PTR : type of pointer to zone
       */
      template <class VLOC, class INTVARS_VAL, class ZONE, class VLOC_PTR, class INTVARS_VAL_PTR, class ZONE_PTR>
      class state_t : public tchecker::ta::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> {
      public:
        /*!
         \brief Type of zone
         */
        using zone_t = ZONE;
        
        /*!
         \brief Type of pointer to zone
         */
        using zone_ptr_t = ZONE_PTR;
        
        /*!
         \brief Constructor
         \param zone : a zone
         \param vloc : tuple of locations
         \param intvars_val : integer variables valuation
         \pre vloc, intvars_val and zone must be valid pointers (not checked)
         \note this keeps a pointer to vloc, intvars_val and zone
         */
        state_t(ZONE_PTR const & zone, VLOC_PTR const & vloc, INTVARS_VAL_PTR const & intvars_val)
        : tchecker::ta::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR>(vloc, intvars_val),
        _zone(zone)
        {}
        
        /*!
         \brief Copy constructor (deleted)
         */
        state_t(tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> const &) = delete;
        
        /*!
         \brief Move constructor (deleted)
         */
        state_t(tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> &&) = delete;
        
        /*!
         \brief Destructor
         \note vloc pointer, intvars valuation pointer and zone pointers are not deleted
         */
        ~state_t() = default;
        
        /*!
         \brief Assignment operator (deleted)
         */
        tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> &
        operator= (tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> const &) = delete;
        
        /*!
         \brief Move-assignment operator (deleted)
         */
        tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> &
        operator= (tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> &&) = delete;
        
        /*!
         \brief Accessor
         \return zone
         */
        inline ZONE const & zone() const
        {
          return *_zone;
        }
        
        /*!
         \brief Accessor
         \return pointer to zone
         */
        inline ZONE_PTR & zone_ptr()
        {
          return _zone;
        }
      protected:
        ZONE_PTR _zone;  /*!< Zone */
      };
      
      
      
      
      /*!
       \brief Equality check
       \param s1 : state
       \param s2 : state
       \return true if s1 and s2 have equal tuple of locations, equal integer variables
       valuation and equal zone, false otherwise
       */
      template <class VLOC, class INTVARS_VAL, class ZONE, class VLOC_PTR, class INTVARS_VAL_PTR, class ZONE_PTR>
      bool operator== (tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> const & s1,
                       tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> const & s2)
      {
        return (tchecker::ta::details::operator==(s1, s2) && (s1.zone() == s2.zone()));
      }
      
      
      /*!
       \brief Disequality check
       \param s1 : state
       \param s2 : state
       \return false if s1 and s2 have equal tuple of locations, equal integer variables
       valuation, and equal zone, true otherwise
       */
      template <class VLOC, class INTVARS_VAL, class ZONE, class VLOC_PTR, class INTVARS_VAL_PTR, class ZONE_PTR>
      bool operator!= (tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> const & s1,
                       tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> const & s2)
      {
        return (! (s1 == s2));
      }
      
      
      /*!
       \brief Hash
       \param s : state
       \return Hash value for state s
       */
      template <class VLOC, class INTVARS_VAL, class ZONE, class VLOC_PTR, class INTVARS_VAL_PTR, class ZONE_PTR>
      std::size_t hash_value(tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> const & s)
      {
        std::size_t h = tchecker::ta::details::hash_value(s);
        boost::hash_combine(h, s.zone().hash());
        return h;
      }
      
    } // end of namespace details
    
  } // end of namespace zg
  
} // end of namespace tchecker

#endif // TCHECKER_ZG_DETAILS_STATE_HH
