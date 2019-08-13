/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_CLOCKBOUNDS_VARIABLES_HH
#define TCHECKER_CLOCKBOUNDS_VARIABLES_HH

#include "tchecker/ta/details/variables.hh"

/*!
 \file variables.hh
 \brief Variables for clock bounds model
 */

namespace tchecker {
  
  namespace clockbounds {
    
    /*!
     \class variables_t
     \brief Model variables for clock bounds
     */
    class variables_t : public tchecker::ta::details::variables_t {
    public:
      using tchecker::ta::details::variables_t::variables_t;
    };
    
  } // end of namespace clockbounds
  
} // end of namespace tchecker

#endif // TCHECKER_CLOCKBOUNDS_VARIABLES_HH
