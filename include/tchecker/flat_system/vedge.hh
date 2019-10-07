/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VEDGE_HH
#define TCHECKER_VEDGE_HH

#include "tchecker/utils/iterator.hh"

/*!
 \file vedge.h
 \brief Tuple of edges
 */

namespace tchecker {
  
  /*!
   \class vedge_iterator_t
   \brief Iterator on a vedge that abstracts vedge representation
   \tparam EDGE : type of edge
   \tparam EDGE_ITERATOR : type of edge iterator, must be default constructible
   and must dereference to EDGE
   \note Iterator can be built either from a pointer to EDGE (vedge made of a
   single edge) or from an iterator on EDGE (vedge made of many edges)
   */
  template <class EDGE, class EDGE_ITERATOR>
  class vedge_iterator_t {
  public:
    /*!
     \brief Constructor
     \param edge : single edge in vedge
     \pre edge != nullptr
     \post this is an iterator on edge
     \throw std::invalid_argument : if edge is nullptr
     */
    explicit vedge_iterator_t(EDGE const * edge)
    : _edge(edge)
    {
      if (edge == nullptr)
        throw std::invalid_argument("nullptr edge");
    }
    
    /*!
     \brief Constructor
     \param it : iterator on vedge
     \post this is an iterator on it
     */
    vedge_iterator_t(EDGE_ITERATOR const & it)
    : _edge(nullptr),
    _it(it)
    {}
    
    /*!
     \brief Copy constructor
     \post this is a copy of it
     */
    vedge_iterator_t(tchecker::vedge_iterator_t<EDGE, EDGE_ITERATOR> const & it)
    = default;
    
    /*!
     \brief Move constructor
     \post it has been moved to this
     */
    vedge_iterator_t(tchecker::vedge_iterator_t<EDGE, EDGE_ITERATOR> &&)
    = default;
    
    /*!
     \brief Destructor
     */
    ~vedge_iterator_t() = default;
    // DO NOT delete _edge (reference)
    
    /*!
     \brief Assignment operator
     \post this is a copy of it
     */
    tchecker::vedge_iterator_t<EDGE, EDGE_ITERATOR> &
    operator= (tchecker::vedge_iterator_t<EDGE, EDGE_ITERATOR> const & it)
    = default;
    
    /*!
     \brief Move-assignment operator
     \post it has been moed to this
     */
    tchecker::vedge_iterator_t<EDGE, EDGE_ITERATOR> &
    operator= (tchecker::vedge_iterator_t<EDGE, EDGE_ITERATOR> && it)
    = default;
    
    /*!
     \brief Equality check
     \return true if it and this point to same edge in vedge, false otherwise
     */
    bool operator==
    (tchecker::vedge_iterator_t<EDGE, EDGE_ITERATOR> const & it) const
    {
      return ((_edge == it._edge) && (_it == it._it));
    }
    
    /*!
     \brief Disequality check
     \return false if it and this point to same edge in vedge, true otherwise
     */
    bool operator!=
    (tchecker::vedge_iterator_t<EDGE, EDGE_ITERATOR> const & it) const
    {
      return !(*this == it);
    }
    
    /*!
     \brief Dereference operator
     \pre this is valid
     \return edge pointed by this
     */
    EDGE const * operator* ()
    {
      if (_edge == nullptr)
        return *_it;
      return _edge;
    }
    
    /*!
     \brief Increment operator
     \pre this is valid
     \post this points to next edge in vedge
     \return this after incrementation
     */
    tchecker::vedge_iterator_t<EDGE, EDGE_ITERATOR> & operator++ ()
    {
      if (_edge == nullptr)
        ++_it;
      else
        ++ _edge;
      return *this;
    }
  private:
    EDGE const * _edge;   /*!< Single edge in vedge (if not nullptr) */
    EDGE_ITERATOR _it;    /*!< Iterator (if _edge is nullptr) */
  };
  
} // end of namespace tchecker

#endif // TCHECKER_VEDGE_HH
