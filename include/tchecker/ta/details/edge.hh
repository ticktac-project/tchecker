/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_EDGE_HH
#define TCHECKER_TA_DETAILS_EDGE_HH

#include "tchecker/fsm/details/edge.hh"

/*!
 \file edge.hh
 \brief Edges for timed automata
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \brief Edge
       \tparam LOC : type of location, should derive from
       tchecker::ta::details::loc_t
       */
      template <class LOC>
      using edge_t = tchecker::fsm::details::edge_t<LOC>;
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_EDGE_HH

