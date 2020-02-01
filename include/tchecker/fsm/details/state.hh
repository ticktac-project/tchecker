/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_STATE_HH
#define TCHECKER_FSM_DETAILS_STATE_HH

#if BOOST_VERSION <= 106600
# include <boost/functional/hash.hpp>
#else
# include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/ts/state.hh"

/*!
 \file state.hh
 \brief States for transition systems over finite state machines
 */

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \class state_t
       \brief FSM's state (details)
       \tparam VLOC : type of tuple of locations
       \tparam INTVARS_VAL : type of integer variables valuations
       \tparam VLOC_PTR : type of pointer to tuple of locations
       \tparam INTVARS_VAL_PTR : type of pointer to integer variables valuation
       */
      template <class VLOC, class INTVARS_VAL, class VLOC_PTR=VLOC *, class INTVARS_VAL_PTR=INTVARS_VAL *>
      class state_t : public tchecker::ts::state_t {
      public:
        /*!
         \brief Type of tuple of locations
         */
        using vloc_t = VLOC;
        
        /*!
         \brief Type of integer variables valuation
         */
        using intvars_valuation_t = INTVARS_VAL;
        
        /*!
         \brief Type of pointer to tuple of locations
         */
        using vloc_ptr_t = VLOC_PTR;
        
        /*!
         \brief Type of pinter to integer variables valuation
         */
        using intvars_valuation_ptr_t = INTVARS_VAL_PTR;
        
        /*!
         \brief Constructor
         \param vloc : tuple of locations
         \param intvars_val : integer variables valuation
         \pre vloc and intvars_val must not be nullptr (not checked)
         \note this keeps a pointer to vloc and intvars_val
         */
        explicit state_t(VLOC_PTR const & vloc, INTVARS_VAL_PTR const & intvars_val)
        : _vloc(vloc), _intvars_val(intvars_val)
        {}
        
        /*!
         \brief Constructor
         \param state : a state
         \param vloc : tuple of locations
         \param intvars_val : integer variables valuation
         \pre vloc and intvars_val must not be nullptr (not checked)
         \note this keeps a pointer to vloc and to intvars_val
         */
        state_t(tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s,
                VLOC_PTR const & vloc, INTVARS_VAL_PTR const & intvars_val)
        : tchecker::ts::state_t(s), _vloc(vloc), _intvars_val(intvars_val)
        {}
        
        /*!
         \brief Copy constructor (deleted)
         */
        state_t(tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const &)
        = delete;
        
        /*!
         \brief Move constructor (deleted)
         */
        state_t(tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> &&) = delete;
        
        /*!
         \brief Destructor
         \note vloc pointer and intvars valuation pointer are not deleted
         */
        ~state_t() = default;
        
        /*!
         \brief Assignment operator (deleted)
         */
        tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> &
        operator= (tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const &)
        = delete;
        
        /*!
         \brief Move-assignment operator (deleted)
         */
        tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> &
        operator= (tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> &&) = delete;
        
        /*!
         \brief Accessor
         \return tuple of locations
         */
        VLOC const & vloc() const
        {
          return *_vloc;
        }
        
        /*!
         \brief Accessor
         \return integer variables valuation
         */
        INTVARS_VAL const & intvars_valuation() const
        {
          return *_intvars_val;
        }
        
        /*!
         \brief Accessor
         \return pointer to tuple of locations
         */
        VLOC_PTR & vloc_ptr()
        {
          return _vloc;
        }
        
        /*!
         \brief Accessor
         \return pointer to integer variables valuation
         */
        INTVARS_VAL_PTR & intvars_valuation_ptr()
        {
          return _intvars_val;
        }
      protected:
        VLOC_PTR _vloc;                  /*!< Tuple of locations */
        INTVARS_VAL_PTR _intvars_val;    /*!< Valuation of integer variables */
      };
      
    } // end of namespace details
    
    
    /*!
     \brief Equality check
     \param s1 : state
     \param s2 : state
     \return true if s1 and s2 have equal tuple of locations, and equal integer variables
     valuation, false otherwise
     */
    template <class VLOC, class INTVARS_VAL, class VLOC_PTR, class INTVARS_VAL_PTR>
    bool operator== (tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s1,
                     tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s2)
    {
      return ((static_cast<tchecker::ts::state_t const &>(s1) == static_cast<tchecker::ts::state_t const &>(s2))
              && (s1.vloc() == s2.vloc())
              && (s1.intvars_valuation() == s2.intvars_valuation())
              );
    }
    
    
    /*!
     \brief Disequality check
     \param s1 : state
     \param s2 : state
     \return false if s1 and s2 have equal tuple of locations, and equal integer variables
     valuation, true otherwise
     */
    template <class VLOC, class INTVARS_VAL, class VLOC_PTR, class INTVARS_VAL_PTR>
    bool operator!= (tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s1,
                     tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s2)
    {
      return (! (s1 == s2));
    }
    
    
    /*!
     \brief Hash
     \param s : state
     \return Hash value for state s
     */
    template <class VLOC, class INTVARS_VAL, class VLOC_PTR, class INTVARS_VAL_PTR>
    std::size_t
    hash_value(tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s)
    {
      std::size_t h = hash_value(static_cast<tchecker::ts::state_t const &>(s));
      boost::hash_combine(h, s.vloc());
      boost::hash_combine(h, s.intvars_valuation());
      return h;
    }
    
    /*!
     \brief Lexical ordering on FSM states
     \param s1 : first state
     \param s2 : second state
     \return 0 if s1 and s2 are equal, a negative value if s1 is smaller than s2 w.r.t. lexical ordering on tuple of locations then integer variables
     valuation, a positive value otherwise
     */
    template <class VLOC, class INTVARS_VAL, class VLOC_PTR, class INTVARS_VAL_PTR>
    int lexical_cmp(tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s1,
                    tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s2)
    {
      int ts_cmp = tchecker::ts::lexical_cmp(s1, s2);
      if (ts_cmp != 0)
        return ts_cmp;
      int vloc_cmp = tchecker::lexical_cmp(s1.vloc(), s2.vloc());
      if (vloc_cmp != 0)
        return vloc_cmp;
      return tchecker::lexical_cmp(s1.intvars_valuation(), s2.intvars_valuation());
    }
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_STATE_HH

