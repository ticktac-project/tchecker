/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_HASHTABLE_HH
#define TCHECKER_HASHTABLE_HH

/*!
 \file hashtable.hh
 \brief Hashtable of shared objects
 */

#include <vector>

#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/shared_objects.hh"

namespace tchecker {

// Forward declaration
template <class SPTR, class HASH> class collision_table_t;
template <class SPTR, class HASH, class EQUAL> class hashtable_t;

/*!
 \brief Type of position in a collision table
 */
using collision_table_position_t = unsigned int;

/*!
 \brief Placeholder position for objects which are not stored
*/
extern tchecker::collision_table_position_t const COLLISION_TABLE_NOT_STORED;

/*!
 \class collision_table_object_t
 \brief Object that can be stored in a collision table
 \note Stores the position of the object in the collision table for fast removal
 */
class collision_table_object_t {
public:
  /*!
   \brief Constructor
   \post this object is not stored
   */
  collision_table_object_t();

  /*!
   \brief Copy constructor
   \throw std::invalid_argument if object is stored
   */
  collision_table_object_t(tchecker::collision_table_object_t const &);

  /*!
   \brief Move constructor
   */
  collision_table_object_t(tchecker::collision_table_object_t &&) = default;

  /*!
   \brief Destructor
   */
  ~collision_table_object_t() = default;

  /*!
   \brief Assignment operator
   \throw std::runtime_error : if this object is stored in a collision table
   \throw std::invalid_argument : if parameter object is stored
   */
  tchecker::collision_table_object_t & operator=(tchecker::collision_table_object_t &);

  /*!
   \brief Move-assignment operator
   \throw std::runtime_error : if this object is stored in a collision table
   */
  tchecker::collision_table_object_t & operator=(tchecker::collision_table_object_t &&);

private:
  template <class SPTR, class HASH> friend class tchecker::collision_table_t;
  template <class SPTR, class HASH, class EQUAL> friend class hashtable_t;

  /*!
   \brief Accessor
   \return position of this object in the collision table
   */
  inline tchecker::collision_table_position_t position_in_table() const { return _position_in_table; }

  /*!
   \brief Accessor
   \return position of this object in the collision list
   */
  inline tchecker::collision_table_position_t position_in_collision_list() const { return _position_in_collision_list; }

  /*!
   \brief Setter
   \param position_in_table : position in table
   \param position_in_collision_list : position in collision list
   \pre position_in_table != tchecker::COLLISION_TABLE_NOT_STORED
   and position_in_collision_list != tchecker::COLLISION_TABLE_NOT_STORED
   \post the position of this node has been set to (position_in_table,
   position_in_collision_table)
   \throw std::runtime_error : if this object is stored
   \throw std::invalid_argument : if precondition is violated
   */
  void set_position(tchecker::collision_table_position_t position_in_table,
                    tchecker::collision_table_position_t position_in_collision_list);

  /*!
   \brief Clear
   \post this object position has been cleared
   */
  void clear_position();

  /*!
   \brief Check if this object is stored in a collision table based on its positions
   \return true if this object is stored in a collision table, false otherwise
   */
  bool is_stored() const;

  tchecker::collision_table_position_t _position_in_table;          /*!< Position in the table */
  tchecker::collision_table_position_t _position_in_collision_list; /*!< Position in the collision list */
};

/*!
  \class collision_table_t
  \brief Container of shared objects with hashing and collision lists, and fast remove
  \tparam SPTR : type of pointer to stored objects. Must be a shared
  pointer, e.g. tchecker::intrusive_shared_ptr_t<...> or std::shared_ptr<...>
  \tparam HASH : hash function over shared pointers of type SPTR
  \note stored objects should derive from tchecker::collision_table_object_t
  \note collision tables do not check for object equality: objects with same
  hash value are simply stored in the same collision list.
*/
template <class SPTR, class HASH> class collision_table_t {
protected:
  using collision_list_t = std::vector<SPTR>;

public:
  /*!
   \brief Type of shared pointer to stored objects
  */
  using object_sptr_t = SPTR;

  /*!
   \brief Constructor
   \param table_size : size of the table (number of collision lists)
   \param hash : hash function
   \pre table_size != tchecker::COLLISION_TABLE_NOT_STORED
   \throw std::invalid_argument : if the precondition is violated
   */
  collision_table_t(std::size_t table_size, HASH const & hash) : _table{table_size}, _hash(hash), _size(0)
  {
    if (table_size == tchecker::COLLISION_TABLE_NOT_STORED)
      throw std::invalid_argument("Collision table size is too big");
  }

  /*!
   \brief Copy constructor (deleted)
   */
  collision_table_t(tchecker::collision_table_t<SPTR, HASH> const &) = delete;

  /*!
   \brief Move constructor
   */
  collision_table_t(tchecker::collision_table_t<SPTR, HASH> &&) = default;

  /*!
   \brief Destructor
   */
  ~collision_table_t() { clear(); }

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::collision_table_t<SPTR, HASH> & operator=(tchecker::collision_table_t<SPTR, HASH> const &) = delete;

  /*!
   \brief Move-assignment operator
   */
  tchecker::collision_table_t<SPTR, HASH> & operator=(tchecker::collision_table_t<SPTR, HASH> &&) = default;

  /*!
   \brief Clear
   \post The collision table is empty
   \note Destructor called on shared pointers
   \note Invalidates iterators
   */
  void clear()
  {
    for (auto & collision_list : _table)
      clear(collision_list);
    _table.clear();
    _size = 0;
  }

  /*!
   \brief Add object to the collision table
   \param o : an object
   \pre o is not stored in a collision table
   \post o has been added to the collision table
   \throw std::invalid_argument : if o is already stored in a collision table
   \note Complexity : computation of the hash value of object o
   \note Invalidates iterators
   */
  void add(SPTR const & o)
  {
    if (o->is_stored())
      throw std::invalid_argument("Adding an object that is already stored in a collision table is not allowed");
    tchecker::collision_table_position_t position_in_table = compute_position_in_table(o);
    add(o, position_in_table);
  }

  /*!
   \brief Remove an object from a collision table
   \param o : an object
   \pre o is stored in this collision table
   \post o has been removed from this collision table
   \throw std::invalid_argument : if o is not stored in this collision table
   \note Contant-time complexity
   \note Invalidates iterators
   */
  void remove(SPTR const & o)
  {
    if (!o->is_stored())
      throw std::invalid_argument("Removing an object that is not stored");
    remove(o->position_in_table(), o->position_in_collision_list());
  }

  /*!
   \brief Accessor
   \return Number of objects in this collision table
   */
  inline std::size_t size() const { return _size; }

  /*!
   \class iterator_t
   \brief Type of iterator over the objects in the table
  */
  class iterator_t {
  public:
    /*!
     \brief Constructor
     \param table : pointer to iterated collision table
     \param position_in_table : position in table
     \pre position_in_table <= table->size() (checked by assertion)
     \post this keeps a pointer to table
     */
    iterator_t(std::vector<collision_list_t> * table, tchecker::collision_table_position_t position_in_table)
        : _table(table), _position_in_table(position_in_table), _position_in_collision_list(0)
    {
      assert(position_in_table <= table->size());
      advance_to_next_non_empty_collision_list();
    }

    /*!
     \brief Copy constructor
    */
    iterator_t(typename tchecker::collision_table_t<SPTR, HASH>::iterator_t const & it) = default;

    /*!
    \brief Move constructor
     */
    iterator_t(typename tchecker::collision_table_t<SPTR, HASH>::iterator_t && it) = default;

    /*!
     \brief Destructor
     */
    ~iterator_t() = default;

    /*!
     \brief Assignment operator
    */
    typename tchecker::collision_table_t<SPTR, HASH>::iterator_t &
    operator=(typename tchecker::collision_table_t<SPTR, HASH>::iterator_t const & it) = default;

    /*!
     \brief Move assignment operator
    */
    typename tchecker::collision_table_t<SPTR, HASH>::iterator_t &
    operator=(typename tchecker::collision_table_t<SPTR, HASH>::iterator_t && it) = default;

    /*!
     \brief Equality predicate
     \param it : an iterator
     \return true if this iterator is equal to it, false otherwise
     */
    bool operator==(typename tchecker::collision_table_t<SPTR, HASH>::iterator_t const & it) const
    {
      return (_table == it._table && _position_in_table == it._position_in_table &&
              _position_in_collision_list == it._position_in_collision_list);
    }

    /*!
     \brief Disequality predicate
     \param it : an iterator
     \return true if this iterator is different of it, false otherwise
     */
    bool operator!=(typename tchecker::collision_table_t<SPTR, HASH>::iterator_t const & it) const { return !(it == *this); }

    /*!
     \brief Dereference operator
     \return SPTR pointed by this iterator
     \pre this iterator is not be past-the-end (checked by assertion)
     */
    SPTR const & operator*() const
    {
      assert(dereferenceable());
      return (*_table)[_position_in_table][_position_in_collision_list];
    }

    /*!
     \brief Dereference operator
     \return SPTR pointed by this iterator
     \pre this iterator is not be past-the-end (checked by assertion)
     */
    SPTR & operator*()
    {
      assert(dereferenceable());
      return (*_table)[_position_in_table][_position_in_collision_list];
    }

    /*!
     \brief Moves iterator to next object in table
     \pre this iterator is not be past-the-end (checked by assertion)
     */
    typename tchecker::collision_table_t<SPTR, HASH>::iterator_t & operator++()
    {
      assert(_position_in_table < _table->size());

      ++_position_in_collision_list;
      if (_position_in_collision_list < (*_table)[_position_in_table].size())
        return *this;

      ++_position_in_table;
      advance_to_next_non_empty_collision_list();

      return *this;
    }

  private:
    template <class SP, class H> friend class collision_table_t;

    /*!
     \brief Moves iterator to the next non-empty collision list if any
    */
    void advance_to_next_non_empty_collision_list()
    {
      for (; _position_in_table < _table->size(); ++_position_in_table)
        if (!(*_table)[_position_in_table].empty())
          break;
      _position_in_collision_list = 0;
    }

    /*!
     \brief Checks if an iterator can be dereferenced
    */
    bool dereferenceable() const
    {
      return (_position_in_table < _table->size() && _position_in_collision_list < (*_table)[_position_in_table].size());
    }

    std::vector<collision_list_t> * _table;                           /*!< Pointer to iterated collision table */
    tchecker::collision_table_position_t _position_in_table;          /*!< Position in _table */
    tchecker::collision_table_position_t _position_in_collision_list; /*!< Position in _table[_position_in_table] */
    /* NB: implementation based on vector iterators would be more elegant,
     * however, iterators could be invalidated by the remove operation below (if
     * iterators are implemented as pointers and the vector gets reallocated for
     * instance)
     */
  };

  /*!
   \brief Accessor
   \return iterator pointing to the first object in the table, or past-the-end
   if the table is empty
   */
  tchecker::collision_table_t<SPTR, HASH>::iterator_t begin()
  {
    return tchecker::collision_table_t<SPTR, HASH>::iterator_t(&_table, 0);
  }

  /*!
   \brief Accessor
   \return const past-the-end iterator
   */
  tchecker::collision_table_t<SPTR, HASH>::iterator_t end()
  {
    return tchecker::collision_table_t<SPTR, HASH>::iterator_t(&_table, _table.size());
  }

  /*!
   \brief Remove object pointed by an iterator
   \param it : iterator
   \return iterator to the next object in this table
   \pre it can be dereferenced
   \note invalidates iterators
  */
  tchecker::collision_table_t<SPTR, HASH>::iterator_t remove(tchecker::collision_table_t<SPTR, HASH>::iterator_t const & it)
  {
    tchecker::collision_table_t<SPTR, HASH>::iterator_t it2(it);
    remove(*it2);
    if (!it2.dereferenceable())
      ++it2;
    return it2;
  }

  /*!
   \brief Type of const iterator over the object in the table
   */
  using const_iterator_t = tchecker::join_iterator_t<tchecker::range_t<typename std::vector<collision_list_t>::const_iterator>,
                                                     tchecker::range_t<typename collision_list_t::const_iterator>>;

  /*!
   \brief Accessor
   \return const iterator pointing to the first object in the table, or past-the-end
   if the table is empty
   */
  tchecker::collision_table_t<SPTR, HASH>::const_iterator_t begin() const
  {
    return tchecker::collision_table_t<SPTR, HASH>::const_iterator_t(
        _table.begin(), _table.end(), tchecker::collision_table_t<SPTR, HASH>::extract_collision_list_const_range);
  }

  /*!
   \brief Accessor
   \return const past-the-end iterator
   */
  tchecker::collision_table_t<SPTR, HASH>::const_iterator_t end() const
  {
    return tchecker::collision_table_t<SPTR, HASH>::const_iterator_t(
        _table.end(), _table.end(), tchecker::collision_table_t<SPTR, HASH>::extract_collision_list_const_range);
  }

  /*!
   \brief Accessor
   \return Range of objects in this collision list
  */
  tchecker::range_t<tchecker::collision_table_t<SPTR, HASH>::const_iterator_t> range() const
  {
    return tchecker::make_range(begin(), end());
  }

  /*!
   \brief Accessor
   \param o : an object
   \return The range of objects in the collision list of o
  */
  inline tchecker::range_t<typename tchecker::collision_table_t<SPTR, HASH>::collision_list_t::const_iterator>
  collision_range(SPTR const & o) const
  {
    tchecker::collision_table_position_t position_in_table = compute_position_in_table(o);
    return tchecker::make_range(_table[position_in_table].begin(), _table[position_in_table].end());
  }

protected:
  /*!
   \brief Computes object position in table
   \param o : an object
   \return The position than o should have in the table
  */
  inline tchecker::collision_table_position_t compute_position_in_table(SPTR const & o) const
  {
    return _hash(o) % _table.size();
  }

  /*!
   \brief Clear a collision list
   \param c : a collision list
   \post c is empty
   \note Destructor of the shared pointers in c have been called
   \note Invalidates iterators
  */
  void clear(collision_list_t & c)
  {
    for (SPTR const & o : c)
      o->clear_position();
    c.clear();
  }

  /*!
   \brief Add an objet o with given hash code
   \param o : an object
   \param h : hash code
   \pre h is the hash code of object o as computed by HASH (checked by
   assertion)
   \post o has been added to this hash table at position given by h
   \note invalidates iterators
  */
  void add(SPTR const & o, tchecker::collision_table_position_t h)
  {
    assert(h == compute_position_in_table(o));
    tchecker::collision_table_position_t position_in_collision_list = add(o, _table[h]);
    o->set_position(h, position_in_collision_list);
    ++_size;
  }

  /*!
   \brief Add an object to a collision list
   \param o : an object
   \param c : a collision list
   \pre o is not stored in a collision list (checked by assertion)
   \post o has been added to c
   \return The position of object o in the collision list c
   \note invalidates iterators
  */
  tchecker::collision_table_position_t add(SPTR const & o, collision_list_t & c)
  {
    assert(!o->is_stored());
    c.push_back(o);
    return c.size() - 1;
  }

  /*!
   \brief Remove an object
   \param position_in_table : position in this table
   \param position_in_collision_list : position in the collision list at position_in_table
   \pre (position_in_table, position_in_collision_list) is a valid position on
   this table
   \post the object at (position_in_table, position_in_collision_list) has been
   removed from this table
   \throw std::invalid_argument : if (position_in_table,
   position_in_collision_list) is not a valid position in this table
  */
  void remove(tchecker::collision_table_position_t position_in_table,
              tchecker::collision_table_position_t position_in_collision_list)
  {
    if (position_in_table >= _table.size())
      throw std::invalid_argument("Removing an object which is not stored in this collision table");

    collision_list_t & c = _table[position_in_table];

    if (position_in_collision_list >= c.size())
      throw std::invalid_argument("Removing an object that is not stored in this collision list");

    c[position_in_collision_list]->clear_position();

    if (position_in_collision_list < c.size() - 1) {
      SPTR back_object = c.back();
      back_object->clear_position();
      back_object->set_position(position_in_table, position_in_collision_list);
      c[position_in_collision_list] = back_object;
    }

    c.pop_back();
    --_size;
  }

  /*!
   \brief Accessor to range of objects in a collision list from a const iterator to a
   collision list
   \param it : a const iterator to a collision list
   \pre it can be dereferenced
   \return A range (begin, end) of objects in the collision list pointed by it
   */
  static tchecker::range_t<typename collision_list_t::const_iterator>
  extract_collision_list_const_range(typename std::vector<collision_list_t>::const_iterator const & it)
  {
    return tchecker::make_range(it->begin(), it->end());
  }

  std::vector<collision_list_t> _table; /*!< Table with collision lists */
  HASH _hash;                           /*!< Hash function */
  std::size_t _size;                    /*!< Number of stored objects */
};

/*!
 \class hashtable_object_t
 \brief Objects that can be stored in a hashtable
*/
class hashtable_object_t : public tchecker::collision_table_object_t {
public:
  using tchecker::collision_table_object_t::collision_table_object_t;
};

/*!
 \class hashtable_t
 \brief Hashtable with collision lists and fast removal
 \tparam SPTR : type of pointer to stored objects. Must be a shared
  pointer, e.g. tchecker::intrusive_shared_ptr_t<...> or std::shared_ptr<...>
 \tparam HASH : hash function over shared pointers of type SPTR
 \tparam EQUAL : equality predicate over shared pointers of type SPTR
 \note stored objects should derive from tchecker::hashtable_object_t
*/
template <class SPTR, class HASH, class EQUAL> class hashtable_t : public tchecker::collision_table_t<SPTR, HASH> {
public:
  /*!
   \brief Constructor
   \param table_size : size of the table (number of collision lists)
   \param hash : hash function
   \param equal : equality predicate
   \pre table_size != tchecker::COLLISION_TABLE_NOT_STORED
   \throw std::invalid_argument : if the precondition is violated
   */
  hashtable_t(std::size_t table_size, HASH const & hash, EQUAL const & equal)
      : tchecker::collision_table_t<SPTR, HASH>(table_size, hash), _equal(equal)
  {
  }

  /*!
   \brief Copy constructor (deleted)
  */
  hashtable_t(tchecker::hashtable_t<SPTR, HASH, EQUAL> const &) = delete;

  /*!
   \brief Move constructor
   */
  hashtable_t(tchecker::hashtable_t<SPTR, HASH, EQUAL> &&) = default;

  /*!
   \brief Destructor
   */
  ~hashtable_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::hashtable_t<SPTR, HASH, EQUAL> & operator=(tchecker::hashtable_t<SPTR, HASH, EQUAL> const &) = delete;

  /*!
   \brief Move-assignment operator
   */
  tchecker::hashtable_t<SPTR, HASH, EQUAL> & operator=(tchecker::hashtable_t<SPTR, HASH, EQUAL> &&) = default;

  /*!
   \brief Add object to the hashtable
   \param o : an object
   \pre o is not stored in a hash table
   \post o has been added to this hashtable if it does not contain any element
   equal to o w.r.t. EQUAL
   \return true if o has been added to this hashtable, false otherwise
   \throw std::invalid_argument : if o is already stored in a hashtable
   \note Complexity : computation of the hash value of object o
   \note Invalidates iterators
   */
  bool add(SPTR const & o)
  {
    if (o->is_stored())
      throw std::invalid_argument("Adding an object that is already stored in a hashtable is not allowed");
    tchecker::collision_table_position_t position_in_table = this->compute_position_in_table(o);
    auto && [found, p] = find(o, this->_table[position_in_table]);
    if (found)
      return false;
    tchecker::collision_table_t<SPTR, HASH>::add(o, position_in_table);
    return true;
  }

  /*!
   \brief Find an object in the hashtable
   \param o : an object
   \return a pair (found, p) where p is true if an object p equal to o has been
   found in this hashtable, otherwise found is false and p == o
  */
  std::tuple<bool, SPTR const> find(SPTR const & o) const
  {
    tchecker::collision_table_position_t position_in_table = this->compute_position_in_table(o);
    return find(o, this->_table[position_in_table]);
  }

  /*!
   \brief Add an object if it is not already in
   \param o : an object
   \post o has been added to this hashtable if it does not contain any object
   EQUAL to o
   \return an object in this hashtable that is EQUAL to o, in particular o
   itself if it has been added
  */
  SPTR find_else_add(SPTR const & o)
  {
    tchecker::collision_table_position_t position_in_table = this->compute_position_in_table(o);
    auto && [found, p] = find(o, this->_table[position_in_table]);
    if (found)
      return p;
    tchecker::collision_table_t<SPTR, HASH>::add(o, position_in_table);
    return o;
  }

protected:
  /*!
   \brief Check if an object belongs to a collision list
   \param o : an object
   \param c : a collision list
   \return a pair (found, p) where p is true if an object p equal to o has been
   found in this hashtable, otherwise found is false and p == o
  */
  std::tuple<bool, SPTR const> find(SPTR const & o,
                                    typename tchecker::collision_table_t<SPTR, HASH>::collision_list_t const & c) const
  {
    for (SPTR const & p : c)
      if (_equal(p, o))
        return std::make_tuple(true, p);
    return std::make_tuple(false, o);
  }

  EQUAL _equal; /*!< Equality predicate */
};

} // end of namespace tchecker

#endif // TCHECKER_HASHTABLE_HH
