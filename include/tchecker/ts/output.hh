/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_OUTPUT_HH
#define TCHECKER_TS_OUTPUT_HH

#include <iostream>

#include "tchecker/ts/state.hh"
#include "tchecker/ts/transition.hh"

/*!
 \file output.hh
 \brief Outputters for transition systems
 */

namespace tchecker {
  
  namespace ts {
    
    /*!
     \brief Output state
     \param os : output stream
     \param s : state
     \post Does not output anything
     \return os
     */
    inline std::ostream & output(std::ostream & os, tchecker::ts::state_t const & s)
    {
      return os;
    }
    
    
    
    /*!
     \class state_outputter_t
     \brief State outputter
     */
    class state_outputter_t {
    public:
      /*!
       \brief Output state
       \param os : output stream
       \param s : a state
       \post see tchecker::ts::output
       \return os
       */
      inline std::ostream & output(std::ostream & os, tchecker::ts::state_t const & s)
      {
        return tchecker::ts::output(os, s);
      }
    };
    
    
    
    /*!
     \brief Output transition
     \param os : output stream
     \param t : transition
     \post Does not output anything
     \return os
     */
    inline std::ostream & output(std::ostream & os, tchecker::ts::transition_t const & t)
    {
      return os;
    }
    
    
    
    
    /*!
     \class transition_outputter_t
     \brief Transition outputter
     */
    class transition_outputter_t {
    public:
      /*!
       \brief Output transition
       \param os : output stream
       \param t : a transition
       \post see tchecker::ts::output
       \return os
       */
      inline std::ostream & output(std::ostream & os, tchecker::ts::transition_t const & t)
      {
        return tchecker::ts::output(os, t);
      }
    };
    
  } // end of namespace ts
  
} // end of namespace tchecker

#endif // TCHECKER_TS_OUTPUT_HH
