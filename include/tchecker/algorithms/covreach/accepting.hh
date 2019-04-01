/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COVREACH_ACCEPTING_HH
#define TCHECKER_ALGORITHMS_COVREACH_ACCEPTING_HH

#include <string>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/fsm/details/system.hh"

/*!
 \file accepting.hh
 \brief Accepting conditions for covering reachability algorithm
 */

namespace tchecker {
  
  namespace covreach {
    
    /*!
     \class accepting_labels_t
     \brief Accepting conditon based on labels
     \tparam NODE_PTR : type of pointer to node
     */
    template <class NODE_PTR>
    class accepting_labels_t {
    public:
      /*!
       \brief Constructor
       \param index : maps label names to label identifiers
       \param accepting_labels : range of label names, should dereference to std::string
       */
      template <class LABELS_RANGE>
      accepting_labels_t(tchecker::label_index_t const & index, LABELS_RANGE const & accepting_labels)
      : _accepting_labels(index.size()), _node_labels(index.size())
      {
        for (std::string const & label : accepting_labels)
          _accepting_labels[index.key(label)] = 1;
      }
      
      /*!
       \brief Copy constructor
       */
      accepting_labels_t(tchecker::covreach::accepting_labels_t<NODE_PTR> const &) = default;
      
      /*!
       \brief Move constructor
       */
      accepting_labels_t(tchecker::covreach::accepting_labels_t<NODE_PTR> &&) = default;
      
      /*!
       \brief Destructor
       */
      ~accepting_labels_t() = default;
      
      /*!
       \brief Assignment operator
       */
      tchecker::covreach::accepting_labels_t<NODE_PTR> &
      operator= (tchecker::covreach::accepting_labels_t<NODE_PTR> const &) = default;
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::covreach::accepting_labels_t<NODE_PTR> &
      operator= (tchecker::covreach::accepting_labels_t<NODE_PTR> &&) = default;
      
      /*!
       \brief Predicate
       \param node : a node
       \return true if node is accepting w.r.t. accepting labels, false otherwise
       */
      bool operator() (NODE_PTR const & node)
      {
        _node_labels.reset();
        for (auto const * loc : node->vloc())
          for (tchecker::label_id_t id : loc->labels())
            _node_labels[id] = 1;
        return _accepting_labels.is_subset_of(_node_labels);
      }
    private:
      boost::dynamic_bitset<> _accepting_labels;  /*!< Set of accepting labels */
      boost::dynamic_bitset<> _node_labels;       /*!< Set of node labels */
    };
    
  } // end of namespace covreach
  
} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COVREACH_ACCEPTING_HH
