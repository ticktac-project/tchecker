/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COVREACH_STATS_HH
#define TCHECKER_ALGORITHMS_COVREACH_STATS_HH

#include <iostream>

/*!
 \file stats.hh
 \brief Statistics for covering reachability algorithm
 */

namespace tchecker {
  
  namespace covreach {
    
    /*!
     \class stats_t
     \brief Statistics for covering reachability algorithm
     */
    class stats_t {
    public:
      /*!
       \brief Constructor
       */
      stats_t();
      
      /*!
       \brief Copy constructor
       */
      stats_t(tchecker::covreach::stats_t const &);
      
      /*!
       \brief Move constructor
       */
      stats_t(tchecker::covreach::stats_t &&);
      
      /*!
       \brief Destructor
       */
      ~stats_t();
      
      /*!
       \brief Assignment operator
       */
      tchecker::covreach::stats_t & operator= (tchecker::covreach::stats_t const &);
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::covreach::stats_t & operator= (tchecker::covreach::stats_t &&);
      
      /*!
       \brief Accessor
       \return the number of visited nodes
       */
      unsigned long visited_nodes() const;
      
      /*!
       \brief Accessor
       \return the numer of covered leaf nodes
       */
      unsigned long covered_leaf_nodes() const;
      
      /*!
       \brief Accessor
       \return the number of covered non-leaf nodes
       */
      unsigned long covered_nonleaf_nodes() const;
      
      /*!
       \brief Increment counter of visited nodes
       \post the number of visited nodes has increased by 1
       */
      void increment_visited_nodes();
      
      /*!
       \brief Increment counter of covered leaf nodes
       \post the number of covered leaf nodes has increased by 1
       */
      void increment_covered_leaf_nodes();
      
      /*!
       \brief Increment counter of covered non-leaf nodes
       \post the number of covered non-leaf nodes has increased by 1
       */
      void increment_covered_nonleaf_nodes();
    private:
      unsigned long _visited_nodes;          /*!< Number of visited nodes */
      unsigned long _covered_leaf_nodes;     /*!< Number of covered leaf nodes */
      unsigned long _covered_nonleaf_nodes;  /*!< Number of covered non-leaf nodes */
    };
    
    
    
    
    /*!
     \brief Output operator for stats
     \param os : output stream
     \param s : stats
     \post s has been output to os
     \return os after output
     */
    std::ostream & operator<< (std::ostream & os, tchecker::covreach::stats_t const & stats);
    
  } // end of namespace covereach
  
} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COVREACH_STATS_HH
