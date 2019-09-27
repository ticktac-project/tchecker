/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ITERATOR_HH
#define TCHECKER_ITERATOR_HH

#include <iterator>
#include <tuple>
#include <vector>
#include <cassert>
#include <functional>

#include <boost/iterator/transform_iterator.hpp>

/*!
 \file iterator.hh
 \brief Iterators
 */

namespace tchecker {
  
  /*!
   \class range_t
   \brief Range [begin,end) of iterators
   \tparam I : type of iterators
   */
  template <class I>
  class range_t {
  public:
    /*!
     \brief Type of iterator
     */
    using iterator_t = I;
    
    /*!
     \brief Constructor
     \param begin : iterator to first element
     \param end : past-the-end iterator
     */
    range_t(I const & begin, I const & end)
    : _begin(begin), _end(end)
    {}
    
    /*!
     \brief Copy constructor
     \param range : a range
     \post this is a copy of range
     */
    range_t(tchecker::range_t<I> const & range) = default;
    
    /*!
     \brief Move constructor
     \param range : a range
     \post range gas been moved to this
     */
    range_t(tchecker::range_t<I> && range) = default;
    
    /*!
     \brief Destructor
     */
    ~range_t() = default;
    
    /*!
     \brief Assignment operator
     \param range : a range
     \post this is a copy of range
     \return this after assignment
     */
    tchecker::range_t<I> & operator= (tchecker::range_t<I> const & range) = default;
    
    /*!
     \brief Move assignment operator
     \param range : a range
     \post range has been moved to this
     \return this after assignment
     */
    tchecker::range_t<I> & operator= (tchecker::range_t<I> && range) = default;
    
    /*!
     \brief Accessor
     \return iterator to first element
     */
    inline constexpr I const & begin() const
    {
      return _begin;
    }
    
    /*!
     \brief Accessor
     \return past-the-end iterator
     */
    inline constexpr I const & end() const
    {
      return _end;
    }
    
    /*!
     \brief Makes a tuple of iterators from a range
     \return tuple of iterators (begin,end) corresponding to this range
     */
    inline std::tuple<I, I> iterators() const
    {
      return std::make_tuple(_begin, _end);
    }
    
    /*!
     \brief Equality check
     \param range : a range
     \return true if this and range are equal, false otherwise
     */
    inline constexpr bool operator== (tchecker::range_t<I> const & range) const
    {
      return ((_begin == range._begin) && (_end == range._end));
    }
    
    /*!
     \brief Disequality check
     \param range : a range
     \return true if this and range are not equal, false otherwise
     */
    inline constexpr bool operator!= (tchecker::range_t<I> const & range) const
    {
      return (! (*this == range));
    }
    
    /*!
     \brief Emptiness check
     \return true if this is an empty range, false otherwise
     */
    inline constexpr bool empty() const
    {
      return (_begin == _end);
    }
  protected:
    I _begin;  /*!< Iterator to first element */
    I _end;    /*!< Past-the-end iterator */
  };
  
  
  /*!
   \brief Make a range
   \tparam I : type of iterator
   \param begin : iterator on first element
   \param end : past-the-end operator
   \return range over [begin,end)
   */
  template <class I>
  tchecker::range_t<I> make_range(I const & begin, I const & end)
  {
    return tchecker::range_t<I>(begin, end);
  }
  
  
  /*!
   \brief Make range from a container (or any object with a access to a range
   though methods begin() and end())
   \tparam C : type of container
   \param c : container
   \return range over values in c
   */
  template <class C>
  decltype(auto) make_range(C & c)
  {
    return tchecker::make_range(c.begin(), c.end());
  }
  
  
  
  
  /*!
   \class join_iterator_t
   \brief Join iterator over ranges. join_iterator_t makes a range of ranges
   ((x11,...,x1N),...,(xK1,...,xKM)) appear as the flat range
   (x11,...,x1N,...,xK1,...,xKM), skipping empty sub-ranges
   \tparam I : type of iterator over sub-ranges
   \tparam II : type of iterator in sub-ranges
   */
  template <class I, class II = typename I::value_type::iterator_t>
  class join_iterator_t {
  public:
    /*!
     \brief Type of iterator
     */
    using iterator_t = I;
    
    /*!
     \brief Type of sub-range iterator
     */
    using range_iterator_t = II;
    
    /*!
     \brief Default constructor
     \note required by C++ concept ForwardIterator
     */
    join_iterator_t()
    : _get_sub_range([] (I const & it) { return std::make_tuple(II{}, II{}); })
    {}
    
    /*!
     \brief Constructor
     \param it : iterator
     \param end : past-the-end iterator
     \param get_sub_range : function that yields an II range from an I iterator
     \post this iterator ranges from it to end
     */
    join_iterator_t(I const & it, I const & end, std::function<std::tuple<II, II>(I const &)> get_sub_range)
    : _it(it), _end(end), _get_sub_range(get_sub_range)
    {
      advance_while_empty_range();
    }
    
    /*!
     \brief Constructor
     \param r : range
     \param get_sub_range : function that yields an II range from an I iterator
     \post this iterator ranges from r.begin() to r.end()
     */
    join_iterator_t(tchecker::range_t<I> const & r, std::function<std::tuple<II, II>(I const &)> get_sub_range)
    : join_iterator_t(r.begin(), r.end(), get_sub_range)
    {}
    
    /*!
     \brief Copy constructor
     \param it : join iterator
     \post this is a copy of it
     */
    join_iterator_t(tchecker::join_iterator_t<I, II> const & it) = default;
    
    /*!
     \brief Move constructor
     \param it : join iterator
     \post it has been moved to this
     */
    join_iterator_t(tchecker::join_iterator_t<I, II> && it) = default;
    
    /*!
     \brief Destructor
     */
    ~join_iterator_t() = default;
    
    /*!
     \brief Assignment operator
     \param it : iterator
     \post this is a copy of it
     \return this after assignment
     */
    tchecker::join_iterator_t<I, II> &
    operator= (tchecker::join_iterator_t<I, II> const & it) = default;
    
    /*!
     \brief Move assignment operator
     \param it : iterator
     \post it has been moved to this
     \return this after assignment
     */
    tchecker::join_iterator_t<I, II> &
    operator= (tchecker::join_iterator_t<I, II> && it) = default;
    
    /*!
     \brief Equality check
     \param it : join iterator
     \return true if this and it are equal, false otherwise
     */
    bool operator== (tchecker::join_iterator_t<I, II> const & it) const
    {
      return ((_it == it._it) &&
              (_end == it._end) &&
              (_range_it == it._range_it) &&
              (_range_end == it._range_end));
    }
    
    /*!
     \brief Disequality check
     \param it : join iterator
     \return negation of join_iterator_t::operator==
     */
    inline bool operator!= (tchecker::join_iterator_t<I, II> const & it) const
    {
      return (! (*this == it));
    }
    
    /*!
     \brief Fast end-of-range check
     \return true if iterator is past-the-end, false otherwise
     \note this is more efficient that checking equality w.r.t. past-the-end
     iterator
     */
    inline bool at_end() const
    {
      return (_it == _end);
    }
    
    /*!
     \brief Accessor (dereference)
     \pre this is not past-the-end (checked by assertion)
     \return current value
     */
    inline decltype(auto) operator* ()
    {
      assert( (_it != _end) && (_range_it != _range_end) );
      return (* _range_it);
    }
    
    /*!
     \brief Increment (next)
     \pre this is not past-the-end (checked by assertion)
     \post this has been incremented
     \return this after incrementation
     */
    tchecker::join_iterator_t<I, II> & operator++ ()
    {
      assert( (_it != _end) && (_range_it != _range_end) );
      
      // Advance range iterator
      ++ _range_it;
      
      // Ok if range iterator is not past-the-end
      if (_range_it != _range_end)
        return *this;
      
      // Otherwise, advance iterator
      ++ _it;
      advance_while_empty_range();
      return *this;
    }
  protected:
    /*!
     \brief Advance iterator
     \post this has been advanced till a non empty range is found, or 
     past-the-end if all ranges are empty. Does not move if current range is
     not empty
     */
    void advance_while_empty_range()
    {
      // Find first non-empty range, if any
      while (_it != _end) {
        //std::tie(_range_it, _range_end) = _it->iterators();
        std::tie(_range_it, _range_end) = _get_sub_range(_it);
        if (_range_it != _range_end)
          break;
        ++ _it;
      }
      if (_it == _end)
        _range_it = _range_end = II();
    }
    
    I _it;                                                       /*!< Iterator to first range (if any) */
    I _end;                                                      /*!< Past-the-end iterator */
    II _range_it;                                                /*!< Iterator in current range */
    II _range_end;                                               /*!< Past-the-end iterator in current range */
    std::function<std::tuple<II, II>(I const &)> _get_sub_range; /*!< Function to retrieve II range from an I iterator */
  };
  
} // end of namespace tchecker


/*!
 \brief Iterator traits for join_iterator_t
 */
template <class I, class II>
struct std::iterator_traits< tchecker::join_iterator_t<I, II> > {
  using difference_type = typename tchecker::join_iterator_t<I, II>::range_iterator_t::difference_type;
  
  using value_type = typename tchecker::join_iterator_t<I, II>::range_iterator_t::value_type;
  
  using pointer = typename tchecker::join_iterator_t<I, II>::range_iterator_t::pointer;
  
  using reference = typename tchecker::join_iterator_t<I, II>::range_iterator_t::reference;
  
  using iterator_category = std::forward_iterator_tag;
};




namespace tchecker{
  
  /*!
   \class cartesian_iterator_t
   \brief Iterator over a cartesian product of ranges
   \tparam I : type of ranges iterator
   */
  template <class I>
  class cartesian_iterator_t {
  public:
    /*!
     \brief Constructor
     \post this is an empty cartesian iterator
     */
    cartesian_iterator_t()
    : _nranges(0)
    {}
    
    /*!
     \brief Copy constructor
     \param it : cartesian iterator
     \post this is a copy of it
     */
    cartesian_iterator_t(tchecker::cartesian_iterator_t<I> const & it) = default;
    
    /*!
     \brief Move constructor
     \param it : cartesian iterator
     \post it has been moved to this
     */
    cartesian_iterator_t(tchecker::cartesian_iterator_t<I> && it) = default;
    
    /*!
     \brief Destructor
     */
    ~cartesian_iterator_t() = default;
    
    /*!
     \brief Assignment operator
     \param it : cartesian iterator
     \post this is a copy of it
     \return this after assignment
     */
    tchecker::cartesian_iterator_t<I> & operator= (tchecker::cartesian_iterator_t<I> const & it) = default;
    
    /*!
     \brief Move assignment operator
     \param it : cartesian iterator
     \post it has been moved to this
     \return this after assignment
     */
    tchecker::cartesian_iterator_t<I> & operator= (tchecker::cartesian_iterator_t<I> && it) = default;
    
    /*!
     \brief Clear the cartesian product
     \post this is an empty cartesian product
     \note clear invalidates iterations and ranges returned by operator*
     */
    inline void clear()
    {
      _nranges = 0;
      _begins.clear();
      _ends.clear();
      _its.clear();
      assert( _nranges == _begins.size() );
      assert( _nranges == _ends.size() );
      assert( _nranges == _its.size() );
    }
    
    /*!
     \brief Add a range to this cartesian product
     \param r : range
     \post r has been added to this
     \note push_back invalidates iterations and ranges returned by operator*
     */
    void push_back(tchecker::range_t<I> const & r)
    {
      ++ _nranges;
      _begins.push_back(r.begin());
      _ends.push_back(r.end());
      _its.push_back(r.begin());
      
      if (_begins.back() == _ends.back())  // detect empty cartesian product
        to_end();
      
      assert( _nranges == _begins.size() );
      assert( _nranges == _ends.size() );
      assert( _nranges == _its.size() );
    }
    
    /*!
     \brief Move a range to this cartesian product
     \param r : range
     \post r has been moved to this
     \note push_back invalidates iterations and ranges returned by operator*
     */
    void push_back(tchecker::range_t<I> && r)
    {
      ++ _nranges;
      _begins.emplace(_begins.end(), std::move(r.begin()));
      _ends.emplace(_ends.end(), std::move(r.end()));
      _its.emplace(_its.end(), std::move(r.begin()));
      
      if (_begins.back() == _ends.back())  // detect empty cartesian product
        to_end();
      
      assert( _nranges == _begins.size() );
      assert( _nranges == _ends.size() );
      assert( _nranges == _its.size() );
    }
    
    /*!
     \brief Equality check
     \param it : iterator
     \return true if it and this are equal, false otherwise
     */
    bool operator== (tchecker::cartesian_iterator_t<I> const & it) const
    {
      return ((_nranges == it._nranges) &&
              (_begins == it._begins) &&
              (_ends == it._ends) &&
              (_its == it._its));
    }
    
    /*!
     \brief Disequality check
     \param it : iterator
     \return true if it and this differ, false otherwise
     */
    bool operator!= (tchecker::cartesian_iterator_t<I> const & it) const
    {
      return (! (it == *this));
    }
    
    /*!
     \brief Accessor
     \return true if current element is past-the-end cartesian product, false
     otherwise
     \note Complexity is constant time
     */
    inline bool at_end() const
    {
      return ((_nranges == 0) || (_its[0] == _ends[0]));
    }
    
    /*!
     \class valeus_iterator_t
     \brief Iterator over a tuple of values in the cartesian product
     \note Makes a vector of iterator appear as the sequence of values pointed
     by those iterators. Could be achieved using boost::transform_iterator, but
     has typing issues
     */
    class values_iterator_t {
    public:
      /*!
       \brief Default constructor
       */
      values_iterator_t() = default;
      
      /*!
       \brief Constructor
       \param it : iterator over a a tuple of iterators
       \post this equals it
       */
      values_iterator_t(typename std::vector<I>::iterator it) : _it(it) {}
      
      /*!
       \brief Copy-constructor
       */
      values_iterator_t(values_iterator_t const &) = default;
      
      /*!
       \brief Move constructor
       */
      values_iterator_t(values_iterator_t &&) = default;
      
      /*!
       \brief Destructor
       */
      ~values_iterator_t() = default;
      
      /*!
       \brief Assignment operator
       */
      values_iterator_t & operator= (values_iterator_t const &) = default;
      
      /*!
       \brief Move assignment operator
       */
      values_iterator_t & operator= (values_iterator_t &&) = default;
      
      /*!
       \brief Equality check
       \param it : iterator
       \return true if this equals it, false otherwise
       */
      bool operator== (values_iterator_t const & it) const
      {
        return (_it == it._it);
      }
      
      /*!
       \brief Disequality check
       \param it : iterator
       \return true if this differs from it, false otherwise
       */
      bool operator!= (values_iterator_t const & it) const
      {
        return (! (*this == it));
      }
      
      /*!
       \brief Accessor
       \return value pointed by current pointed iterator
       */
      decltype(auto) operator* ()
      {
        return **_it;
      }
      
      /*!
       \brief Forward move
       \post this has been moved one step forward
       \return thsi after it has been moved
       */
      values_iterator_t & operator++ ()
      {
        ++ _it;
        return *this;
      }
    private:
      typename std::vector<I>::iterator _it;  /*!< Iterator */
    };
    
    /*!
     \brief Accessor
     \return current element
     \note the returned ranged is invalidated by calls to operator++
     */
    tchecker::range_t<tchecker::cartesian_iterator_t<I>::values_iterator_t> operator* ()
    {
      assert( _nranges != 0 );
      values_iterator_t begin(_its.begin()), end(_its.end());
      return tchecker::make_range(begin, end);
    }
    
    /*!
     \brief Move to past-the-end
     \note Complexity is constant time
     */
    inline void to_end()
    {
      if ( _nranges != 0 )
        _its[0] = _ends[0];
    }
    
    /*!
     \brief Move to next element
     \pre not at_end()  (checked by assertion)
     \post this points to nest element or past-the-end
     \note operator++ invalidates all ranges returned by operator*
     */
    tchecker::cartesian_iterator_t<I> & operator++ ()
    {
      assert( ! at_end() );
      
      ssize_t i = _nranges - 1;
      while (i >= 0) {
        ++ _its[i];
        if ( _its[i] != _ends[i] )   // range i not at end
          break;
        if (i > 0)                   // set range i to beginning if not done
          _its[i] = _begins[i];
        --i;
      }
      return *this;
    }
  private:
    std::size_t _nranges;    /*!< Number of ranges in the cartesian product */
    std::vector<I> _begins;  /*!< Iterators to first elements */
    std::vector<I> _ends;    /*!< Past-the-end-iterators */
    std::vector<I> _its;     /*!< Iterators on current elements */
  };
  
} // end of namespace tchecker


/*!
 \brief Iterator traits for cartesian_iterator_t
 */
template <class I>
struct std::iterator_traits< tchecker::cartesian_iterator_t<I> > {
  using difference_type = typename std::iterator_traits<I>::difference_type;
  
  using value_type = tchecker::range_t<typename tchecker::cartesian_iterator_t<I>::values_iterator_t>;
  
  using pointer = value_type *;
  
  using reference = value_type &;
  
  using iterator_category = std::forward_iterator_tag;
};

#endif // TCHECKER_ITERATOR_HH
