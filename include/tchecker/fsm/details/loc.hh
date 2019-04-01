/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_LOC_HH
#define TCHECKER_FSM_DETAILS_LOC_HH

#include <string>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/expression/expression.hh"
#include "tchecker/system/loc.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file loc.hh
 \brief Locations for finite state machines
 */

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \class loc_t
       \tparam EDGE : type of edge, should derive from tchecker::edge_t
       \brief Location
       */
      template <class EDGE>
      class loc_t : public tchecker::loc_t<EDGE> {
      public:
        /*!
         \brief Constructor
         \tparam LABELS_ITER : iterator over labels
         \param pid : process identifier
         \param id : location identifier
         \param name : location name
         \param initial : initial location flag
         \param invariant : location invariant
         \param labels : location labels
         \pre name is not empty and invariant is not nullptr
         \throw std::invalid_argument : if the precondition is violated
         \note this takes ownership on invariant
         \note LABELS_ITER is supposed to dereference to tchecker::label_id_t
         */
        template <class LABELS_ITER>
        loc_t(tchecker::process_id_t pid,
              tchecker::loc_id_t id,
              std::string const & name,
              bool initial,
              tchecker::expression_t * invariant,
              tchecker::range_t<LABELS_ITER> const & labels)
        : tchecker::loc_t<EDGE>(pid, id, name),
        _initial(initial),
        _invariant(invariant)
        {
          static_assert(std::is_same<typename LABELS_ITER::value_type, tchecker::label_id_t>::value,
                        "expecting iterator on label_id_t");
          
          if (invariant == nullptr)
            throw std::invalid_argument("nullptr invariant");
          
          _labels.insert(_labels.end(), labels.begin(), labels.end());
          _labels.shrink_to_fit();
        }
        
        
        /*!
         \brief Copy constructor
         \param loc : location
         \post this is a copy of loc
         */
        loc_t(tchecker::fsm::details::loc_t<EDGE> const & loc)
        : tchecker::loc_t<EDGE>(loc),
        _initial(loc._initial),
        _invariant(loc._invariant->clone()),
        _labels(loc._labels)
        {}
        
        
        /*!
         \brief Move constructor
         \param loc : location
         \post loc has been moved to this
         */
        loc_t(tchecker::fsm::details::loc_t<EDGE> && loc) = default;
        
        
        /*!
         \brief Destructor
         */
        ~loc_t()
        {
          delete _invariant;
        }
        
        
        /*!
         \brief Assignment operator
         \param loc : location
         \post this is a copy of loc
         */
        tchecker::fsm::details::loc_t<EDGE> & operator= (tchecker::fsm::details::loc_t<EDGE> const & loc)
        {
          if (this != &loc) {
            this->tchecker::loc_t<EDGE>::operator=(loc);
            _initial = loc._initial;
            delete _invariant;
            _invariant = loc._invariant->clone();
            _labels = loc._labels;
          }
          return *this;
        }
        
        
        /*!
         \brief Move assignment operator
         \param loc : location
         \post loc has been moved to this
         */
        tchecker::fsm::details::loc_t<EDGE> & operator= (tchecker::fsm::details::loc_t<EDGE> && loc) = default;
        
        
        /*!
         \brief Accessor
         \return true if the location is initial, false otherwise
         */
        inline bool initial() const
        {
          return _initial;
        }
        
        
        /*!
         \brief Accessor
         \return Invariant
         */
        inline tchecker::expression_t const & invariant() const
        {
          return (* _invariant);
        }
        
        
        /*!
         \brief Type of labels iterator
         */
        using const_labels_iterator_t = std::vector<tchecker::label_id_t>::const_iterator;
        
        
        /*!
         \brief Accessor
         \return range of labels
         */
        inline tchecker::range_t<const_labels_iterator_t> labels() const
        {
          return tchecker::make_range(_labels.begin(), _labels.end());
        }
      private:
        bool _initial;                               /*!< Initial flag */
        tchecker::expression_t * _invariant;         /*!< Invariant */
        std::vector<tchecker::label_id_t> _labels;   /*!< Labels */
      };
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_LOC_HH
