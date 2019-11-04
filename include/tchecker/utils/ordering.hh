/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ORDERING_HH
#define TCHECKER_ORDERING_HH

#include "tchecker/utils/iterator.hh"

/*!
 \file ordering.hh
 \brief Ordering relations
 */

namespace tchecker {

  /*!
   \brief Lexical ordering
   \tparam I1 : type of first iterator
   \tparam I2 : type of second iterator
   \tparam CMP : type of values comparison operator
   \param begin1 : first iterator
   \param end1 : first past-the-end iterator
   \param begin2 : second iterator
   \param end2 : second past-the-end iterator
   \param cmp : comparison iterator on values (v1, v2) from the first and second iterators. Must be callable as cmp(v1, v2) and
   return 0 if v1 and v2 are equal, a negative value if v1 < v2, and a positive value if v1 > v2
   \return 0 if the ranges (begin1, end1) and (begin2, end2) are equal, a negative value if (begin1, end1) is smaller than (begin2, end2) w.r.t.
   lexical ordering, and a positive value otherwise. The values in the two ranges are compared w.r.t. cmp.
   */
  template <class I1, class I2, class CMP>
  int lexical_cmp(I1 const & begin1, I1 const & end1, I2 const & begin2, I2 const & end2, CMP cmp)
  {
    I1 it1 = begin1;
    I2 it2 = begin2;
    for ( ; 1; ++it1, ++it2) {
      if (it1 == end1)
        return (it2 == end2 ? 0 : -1);
      if (it2 == end2)
        return 1;
      int cmp_value = cmp(*it1, *it2);
      if (cmp_value < 0)
        return -1;
      if (cmp_value > 0)
        return 1;
    }
  }
  
  
  /*!
   \brief Lexical ordering
   \tparam I1 : type of first iterator
   \tparam I2 : type of second iterator
   \tparam CMP : type of values comparison operator
   \param r1 : first range
   \param r2 : second range
   \param cmp : comparison iterator on values (v1, v2) from the first and second iterators. Must be callable as cmp(v1, v2) and
   return 0 if v1 and v2 are equal, a negative value if v1 < v2, and a positive value if v1 > v2
   \return 0 if the ranges r1 and r2 are equal, a negative value if r1 is smaller than r2 w.r.t. lexical ordering, and a positive value otherwise.
   The values in the two ranges are compared w.r.t. cmp.
  */
  template <class I1, class I2, class CMP>
  int lexical_cmp(tchecker::range_t<I1> const & r1, tchecker::range_t<I2> const & r2, CMP cmp)
  {
    return tchecker::lexical_cmp(r1.begin(), r1.end(), r2.begin(), r2.end(), cmp);
  }
  
} // end of namespace tchecker

#endif // TCHECKER_ORDERING_HH
