/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/algorithms/covreach/stats.hh"

namespace tchecker {
  
  namespace covreach {
    
    stats_t::stats_t()
    : _visited_nodes(0), _covered_leaf_nodes(0), _covered_nonleaf_nodes(0)
    {}
    
    
    stats_t::stats_t(tchecker::covreach::stats_t const &) = default;
    
    
    stats_t::stats_t(tchecker::covreach::stats_t &&) = default;
    
    
    stats_t::~stats_t() = default;
    
    
    tchecker::covreach::stats_t & stats_t::operator= (tchecker::covreach::stats_t const &) = default;
    
    
    tchecker::covreach::stats_t & stats_t::operator= (tchecker::covreach::stats_t &&) = default;
    
    
    unsigned long stats_t::visited_nodes() const
    {
      return _visited_nodes;
    }
    
    
    unsigned long stats_t::covered_leaf_nodes() const
    {
      return _covered_leaf_nodes;
    }
    
    
    unsigned long stats_t::covered_nonleaf_nodes() const
    {
      return _covered_nonleaf_nodes;
    }
    
    
    void stats_t::increment_visited_nodes()
    {
      ++ _visited_nodes;
    }
    
    
    void stats_t::increment_covered_leaf_nodes()
    {
      ++ _covered_leaf_nodes;
    }
    
    
    void stats_t::increment_covered_nonleaf_nodes()
    {
      ++ _covered_nonleaf_nodes;
    }
    
    
    
    
    std::ostream & operator<< (std::ostream & os, tchecker::covreach::stats_t const & stats)
    {
      os << "VISITED_NODES " << stats.visited_nodes() << std::endl;
      os << "COVERED_LEAF_NODES " << stats.covered_leaf_nodes() << std::endl;
      os << "COVERED_NONLEAF_NODES " << stats.covered_nonleaf_nodes() << std::endl;
      return os;
    }
    
  } // end of namespace covereach
  
} // end of namespace tchecker
