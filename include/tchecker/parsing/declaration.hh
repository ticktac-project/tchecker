/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_PARSING_DECLARATION_HH
#define TCHECKER_PARSING_DECLARATION_HH

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file declaration.hh
 \brief System declarations
 */

namespace tchecker {

namespace parsing {

/* Forward declaration */

class declaration_visitor_t;

/* Attributes */

/*!
 \class attr_parsing_position_t
 \brief Parsing postion for attributes
*/
class attr_parsing_position_t {
public:
  /*!
   \brief Default constructor
   \post Has empty key and value position
  */
  attr_parsing_position_t();

  /*!
   \brief Constructor
   \param key_position : parsing position of the attribute key
   \param value_position : parsing position of the attribute value
  */
  attr_parsing_position_t(std::string const & key_position, std::string const & value_position);

  /*!
   \brief Accessor
   \return Parsing position of the attribute key
  */
  inline std::string const & key_position() const { return _key_position; }

  /*!
   \brief Accessor
   \return Parsing position of the attribute value
  */
  inline std::string const & value_position() const { return _value_position; }

private:
  std::string _key_position;   /*!< Parsing position of the attribute key */
  std::string _value_position; /*!< Parsing position of the attribute value */
};

/*!
 \class attr_t
 \brief Attribute for declarations
 */
class attr_t {
public:
  /*!
   \brief Constructor
   \param key : attribute key
   \param value : attribute value
   \param parsing_position : parsing position of the attribute
   */
  attr_t(std::string const & key, std::string const & value,
         tchecker::parsing::attr_parsing_position_t const & parsing_position);

  /*!
   \brief Accessor
   \return Attribute key
   */
  inline std::string const & key() const { return _key; }

  /*!
   \brief Accessor
   \return Attribute value
   */
  inline std::string const & value() const { return _value; }

  /*!
   \brief Accessor
   \return Parsing position of the attribute
  */
  inline tchecker::parsing::attr_parsing_position_t const & parsing_position() const { return _parsing_position; }

private:
  std::string _key;                                             /*!< Key */
  std::string _value;                                           /*!< Value */
  tchecker::parsing::attr_parsing_position_t _parsing_position; /*!< Parsing position */
};

/*!
 \brief Output operator
 \param os : output stream
 \param attr : attribute
 \post attr has been output to os
 \return os after attr has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::parsing::attr_t const & attr);

/*!
 \class attributes_t
 \brief Attributes map for declarations
 */
class attributes_t {
  /*!
   \brief Type of attributes map
   */
  using map_t = std::unordered_multimap<std::string, std::shared_ptr<tchecker::parsing::attr_t>>;

public:
  /*!
   \brief Empties the map
   \post this map is empty
   */
  void clear();

  /*!
   \brief Emptiness check
   \return true if this map is empty, false otherwise
   */
  bool empty() const;

  /*!
   \brief Accessor
   \return size of the map
   */
  std::size_t size() const;

  /*!
   \brief Insert
   \param attr : attribute to insert in the map
   \post attr has been inserted
   \note this keeps a pointer on attr
   */
  void insert(std::shared_ptr<tchecker::parsing::attr_t> const & attr);

  /*!
   \class const_iterator_t
   \brief Const iterator on the map
   */
  class const_iterator_t : public map_t::const_iterator {
  public:
    /*!
     \brief Constructor
     \param it : map iterator
     \post this iterator point to it
     */
    const_iterator_t(map_t::const_iterator const & it);

    /*!
     \brief Accessor
     \return attribute at pointed map entry
     */
    inline tchecker::parsing::attr_t const & operator*() const { return *(*this)->second; }
  };

  /*!
   \brief Accessor
   \return iterator to the first entry of the map (if any)
   */
  const_iterator_t begin() const;

  /*!
   \brief Accessor
   \return iteratot to the past-the-end "entry" of the map
   */
  const_iterator_t end() const;

  /*!
   \brief Accessor
   \return range of attributes in this map
   */
  tchecker::range_t<const_iterator_t> attributes() const;

  /*!
   \brief Accessor
   \param key : key of attribute
   \return range of attributes with key in this map
   */
  tchecker::range_t<const_iterator_t> attributes(std::string const & key) const;

private:
  map_t _attr; /*!< Attributes */
};

/*!
 \brief Output operator
 \param os : output stream
 \param attr : attributes map
 \post attr has been output to os
 \return os after attr has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::parsing::attributes_t const & attr);

/*!
 \class declaration_t
 \brief Declaration from input file
 \note Abstract base class
 */
class declaration_t {
public:
  /*!
   \brief Default constructor
   */
  declaration_t();

  /*!
   \brief Constructor
   \param attr : attributes
   \param context : contextual information for declaration (position in input file, etc)
   */
  explicit declaration_t(tchecker::parsing::attributes_t const & attr, std::string const & context);

  /*!
   \brief Copy constructor
   */
  declaration_t(tchecker::parsing::declaration_t const &);

  /*!
   \brief Move constructor
   */
  declaration_t(tchecker::parsing::declaration_t &&);

  /*!
   \brief Destructor
   */
  virtual ~declaration_t();

  /*!
   \brief Assignment operator
   */
  tchecker::parsing::declaration_t & operator=(tchecker::parsing::declaration_t const &);

  /*!
   \brief Move assignment operator
   */
  tchecker::parsing::declaration_t & operator=(tchecker::parsing::declaration_t &&);

  /*!
   \brief Accessor
   \return Attributes
   */
  inline tchecker::parsing::attributes_t const & attributes() const { return _attr; }

  /*!
   \brief Accessor
   \return Contextual information
  */
  inline std::string const & context() const { return _context; }

  /*!
   \brief Visit
   \param v : visitor
   \post this has been visited by v
   */
  virtual void visit(tchecker::parsing::declaration_visitor_t & v) const = 0;

  /*!
   \brief Output the declaration
   \param os : output stream
   */
  virtual std::ostream & output(std::ostream & os) const = 0;

protected:
  tchecker::parsing::attributes_t _attr; /*!< Attributes */
  std::string _context;                  /*!< Contextual information */

  friend std::ostream & operator<<(std::ostream & os, tchecker::parsing::declaration_t const & decl);
};

/*!
 \brief Output declaration
 \param os : output stream
 \param decl : declaration
 \post decl has been output to os
 \return os after outputing decl has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::parsing::declaration_t const & decl);

/*!
 \class inner_declaration_t
 \brief Declarations that appear below a `system` declaration
 */
class inner_declaration_t : public tchecker::parsing::declaration_t {
public:
  using tchecker::parsing::declaration_t::declaration_t;

  /*!
   \brief Copy constructor
   */
  inner_declaration_t(tchecker::parsing::inner_declaration_t const &);

  /*!
   \brief Move constructor
   */
  inner_declaration_t(tchecker::parsing::inner_declaration_t &&);

  /*!
   \brief Destructor
   */
  virtual ~inner_declaration_t();

  /*!
   \brief Assignment operator
   */
  tchecker::parsing::inner_declaration_t & operator=(tchecker::parsing::inner_declaration_t const &);

  /*!
   \brief Move-assignment operator
   */
  tchecker::parsing::inner_declaration_t & operator=(tchecker::parsing::inner_declaration_t &&);
};

/*!
 \class clock_declaration_t
 \brief Declaration of an (array of) clock variable
 */
class clock_declaration_t final : public tchecker::parsing::inner_declaration_t {
public:
  /*!
   \brief Constructor
   \param name : clock name
   \param size : size of array
   \param attr : attributes
   \param context : contextual information for declaration (position in input file, etc)
   \pre name is not empty and size >= 1
   \throw std::invalid_argument : if name is empty or size < 1
   */
  clock_declaration_t(std::string const & name, unsigned int size, tchecker::parsing::attributes_t const & attr,
                      std::string const & context);

  /*!
   \brief Copy-constructor
   */
  clock_declaration_t(tchecker::parsing::clock_declaration_t const &);

  /*!
   \brief Move constructor
   */
  clock_declaration_t(tchecker::parsing::clock_declaration_t &&);

  /*!
   \brief Destructor
   */
  virtual ~clock_declaration_t();

  /*!
   \brief Assignment operator
   */
  tchecker::parsing::clock_declaration_t & operator=(tchecker::parsing::clock_declaration_t const &);

  /*!
   \brief Move-assignment operator
   */
  tchecker::parsing::clock_declaration_t & operator=(tchecker::parsing::clock_declaration_t &&);

  /*!
   \brief Accessor
   \return Name
   */
  inline std::string const & name() const { return _name; }

  /*!
   \brief Accessor
   \return Size
   */
  inline unsigned int size() const { return _size; }

  /*!
   \brief Visit
   \post this has been visited by v
   */
  virtual void visit(tchecker::parsing::declaration_visitor_t & v) const override;

  /*!
   \brief Output the declaration
   \param os : output stream
   \post this declaration has been output to os
   \return os after this declaration has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

private:
  std::string _name;  /*!< Name */
  unsigned int _size; /*!< Size */
};

/*!
 \class int_declaration_t
 \brief Declaration of an (array of) int variable
 */
class int_declaration_t final : public tchecker::parsing::inner_declaration_t {
public:
  /*!
   \brief Constructor
   \param name : int name
   \param size : size of array
   \param min : minimal value
   \param max : maximal value
   \param init : initial value
   \param attr : attributes
   \param context : contextual information for declaration (position in input file, etc)
   \pre name is not empty, size >= 1, min <= init <= max
   \throws std::invalid_argument : name is empty, or if size < 1, or
   not (min <= init <= max)
   */
  int_declaration_t(std::string const & name, unsigned int size, tchecker::integer_t min, tchecker::integer_t max,
                    tchecker::integer_t init, tchecker::parsing::attributes_t const & attr, std::string const & context);

  /*!
   \brief Copy constructor
   */
  int_declaration_t(tchecker::parsing::int_declaration_t const &);

  /*!
   \brief Move constructor
   */
  int_declaration_t(tchecker::parsing::int_declaration_t &&);

  /*!
   \brief Destructor
   */
  virtual ~int_declaration_t();

  /*!
   \brief Assignment operator
   */
  tchecker::parsing::int_declaration_t & operator=(tchecker::parsing::int_declaration_t const &);

  /*!
   \brief Move-assignment operator
   */
  tchecker::parsing::int_declaration_t & operator=(tchecker::parsing::int_declaration_t &&);

  /*!
   \brief Accessor
   \return Name
   */
  inline std::string const & name() const { return _name; }

  /*!
   \brief Accessor
   \return Size
   */
  inline unsigned int size() const { return _size; }

  /*!
   \brief Accessor
   \return Min value
   */
  inline tchecker::integer_t min() const { return _min; }

  /*!
   \brief Accessor
   \return Max value
   */
  inline tchecker::integer_t max() const { return _max; }

  /*!
   \brief Accessor
   \return Initial value
   */
  inline tchecker::integer_t init() const { return _init; }

  /*!
   \brief Visit
   \post this has been visited by v
   */
  virtual void visit(tchecker::parsing::declaration_visitor_t & v) const override;

  /*!
   \brief Output the declaration
   \param os : output stream
   \post this declaration has been output to os
   \return os after this declaration has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

private:
  std::string _name;         /*!< Name */
  unsigned int _size;        /*!< Size */
  tchecker::integer_t _min;  /*!< Min value */
  tchecker::integer_t _max;  /*!< Max value */
  tchecker::integer_t _init; /*!< Initial value */
};

/*
 \class process_declaration_t
 \brief Declaration of a process
 */
class process_declaration_t final : public tchecker::parsing::inner_declaration_t {
public:
  /*!
   \brief Constructor
   \param name : process name
   \param attr : attributes
   \param context : contextual information for declaration (position in input file, etc)
   \pre name is not empty
   \throws std::invalid_argument : if name is empty
   */
  process_declaration_t(std::string const & name, tchecker::parsing::attributes_t const & attr, std::string const & context);

  /*!
   \brief Copy constructor
   */
  process_declaration_t(tchecker::parsing::process_declaration_t const &);

  /*!
   \brief Move constructor
   */
  process_declaration_t(tchecker::parsing::process_declaration_t &&);

  /*!
   \brief Destructor
   */
  virtual ~process_declaration_t();

  /*!
   \brief Assignment operator
   */
  tchecker::parsing::process_declaration_t & operator=(tchecker::parsing::process_declaration_t const &);

  /*!
   \brief Move-assignment operator
   */
  tchecker::parsing::process_declaration_t & operator=(tchecker::parsing::process_declaration_t &&);

  /*!
   \brief Accessor
   \return Name
   */
  inline std::string const & name() const { return _name; }

  /*!
   \brief Visit
   \post this has been visited by v
   */
  virtual void visit(tchecker::parsing::declaration_visitor_t & v) const override;

  /*!
   \brief Output the declaration
   \param os : output stream
   \post this declaration has been output to os
   \return os after this declaration has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

private:
  std::string _name; /*!< Process name */
};

/*!
 \class event_declaration_t
 \brief Declaration of an event
 */
class event_declaration_t final : public tchecker::parsing::inner_declaration_t {
public:
  /*!
   \brief Constructor
   \param name : event name
   \param attr : attributes
   \param context : contextual information for declaration (position in input file, etc)
   \pre name is not empty
   \throws std::invalid_argument : if name is empty
   */
  event_declaration_t(std::string const & name, tchecker::parsing::attributes_t const & attr, std::string const & context);

  /*!
   \brief Copy constructor
   */
  event_declaration_t(tchecker::parsing::event_declaration_t const &);

  /*!
   \brief Move constructor
   */
  event_declaration_t(tchecker::parsing::event_declaration_t &&);

  /*!
   \brief Destructor
   */
  virtual ~event_declaration_t();

  /*!
   \brief Assignment operator
   */
  tchecker::parsing::event_declaration_t & operator=(tchecker::parsing::event_declaration_t const &);

  /*!
   \brief Move-assignement operator
   */
  tchecker::parsing::event_declaration_t & operator=(tchecker::parsing::event_declaration_t &&);

  /*!
   \brief Accessor
   \return Name
   */
  inline std::string const & name() const { return _name; }

  /*!
   \brief Visit
   \post this has been visited by v
   */
  virtual void visit(tchecker::parsing::declaration_visitor_t & v) const override;

  /*!
   \brief Output the declaration
   \param os : output stream
   \post this declaration has been output to os
   \return os after this declaration has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

private:
  std::string _name; /*!< Event name */
};

/*!
 \class location_declaration_t
 \brief Declaration of a location
 */
class location_declaration_t final : public tchecker::parsing::inner_declaration_t {
public:
  /*!
   \brief Constructor
   \param name : location name
   \param process : location process
   \param attr : attributes
   \param context : contextual information for declaration (position in input file, etc)
   \pre name is not empty
   \throws std::invalid_argument : if name is empty
   \note attr is moved to this.
   */
  location_declaration_t(std::string const & name,
                         std::shared_ptr<tchecker::parsing::process_declaration_t const> const & process,
                         tchecker::parsing::attributes_t const & attr, std::string const & context);

  /*!
   \brief Copy constructor
   */
  location_declaration_t(tchecker::parsing::location_declaration_t const &);

  /*!
   \brief Move constructor
   */
  location_declaration_t(tchecker::parsing::location_declaration_t &&);

  /*!
   \brief Destructor
   */
  virtual ~location_declaration_t();

  /*!
   \brief Assignment
   */
  tchecker::parsing::location_declaration_t & operator=(tchecker::parsing::location_declaration_t const &);

  /*!
   \brief Move assignment
   */
  tchecker::parsing::location_declaration_t & operator=(tchecker::parsing::location_declaration_t &&);

  /*!
   \brief Accessor
   \return Name
   */
  inline std::string const & name() const { return _name; }

  /*!
   \brief Accessor
   \return Location process
   */
  inline tchecker::parsing::process_declaration_t const & process() const { return *_process; }

  /*!
   \brief Visit
   \post this has been visited by v
   */
  virtual void visit(tchecker::parsing::declaration_visitor_t & v) const override;

  /*!
   \brief Output the declaration
   \param os : output stream
   \post this declaration has been output to os
   \return os after this declaration has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

private:
  std::string _name;                                                        /*!< Name */
  std::shared_ptr<tchecker::parsing::process_declaration_t const> _process; /*!< Process */
};

/*!
 \class edge_declaration_t
 \brief Declaration of an edge
 */
class edge_declaration_t final : public tchecker::parsing::inner_declaration_t {
public:
  /*!
   \brief Constructor
   \param process : process
   \param src : source location
   \param tgt : target location
   \param event : edge event
   \param attr : attributes
   \param context : contextual information for declaration (position in input file, etc)
   \pre src and tgt belong to process
   \throws std::invalid_argument : if src and tgt do not belong to process
   \note attr is moved to this
   */
  edge_declaration_t(std::shared_ptr<tchecker::parsing::process_declaration_t const> const & process,
                     std::shared_ptr<tchecker::parsing::location_declaration_t const> const & src,
                     std::shared_ptr<tchecker::parsing::location_declaration_t const> const & tgt,
                     std::shared_ptr<tchecker::parsing::event_declaration_t const> const & event,
                     tchecker::parsing::attributes_t const & attr, std::string const & context);

  /*!
   \brief Copy constructor
   */
  edge_declaration_t(tchecker::parsing::edge_declaration_t const &);

  /*!
   \brief Move constructor
   */
  edge_declaration_t(tchecker::parsing::edge_declaration_t &&);

  /*!
   \brief Destructor
   */
  virtual ~edge_declaration_t();

  /*!
   \brief Assignment
   */
  tchecker::parsing::edge_declaration_t & operator=(tchecker::parsing::edge_declaration_t const &);

  /*!
   \brief Move assignment
   */
  tchecker::parsing::edge_declaration_t & operator=(tchecker::parsing::edge_declaration_t &&);

  /*!
   \brief Accessor
   \return Process
   */
  inline tchecker::parsing::process_declaration_t const & process() const { return *_process; }

  /*!
   \brief Accessor
   \return Source location
   */
  inline tchecker::parsing::location_declaration_t const & src() const { return *_src; }

  /*!
   \brief Accessor
   \return Target location
   */
  inline tchecker::parsing::location_declaration_t const & tgt() const { return *_tgt; }

  /*!
   \brief Accessor
   \return Event
   */
  inline tchecker::parsing::event_declaration_t const & event() const { return *_event; }

  /*!
   \brief Visit
   \post this has been visited by v
   */
  virtual void visit(tchecker::parsing::declaration_visitor_t & v) const override;

  /*!
   \brief Output the declaration
   \param os : output stream
   \post this declaration has been output to os
   \return os after this declaration has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

private:
  std::shared_ptr<tchecker::parsing::process_declaration_t const> _process; /*!< Process */
  std::shared_ptr<tchecker::parsing::location_declaration_t const> _src;    /*!< Source loc */
  std::shared_ptr<tchecker::parsing::location_declaration_t const> _tgt;    /*!< Target loc */
  std::shared_ptr<tchecker::parsing::event_declaration_t const> _event;     /*!< Event */
};

/*!
 \class sync_constraint_t
 \brief Synchronization constraint
 */
class sync_constraint_t {
public:
  /*!
   \brief Constructor
   \param process : synchronized process
   \param event : synchronized event
   \param strength : strength of synchronization
   */
  sync_constraint_t(std::shared_ptr<tchecker::parsing::process_declaration_t const> const & process,
                    std::shared_ptr<tchecker::parsing::event_declaration_t const> const & event,
                    enum tchecker::sync_strength_t strength);

  /*!
   \brief Accessor
   \return Process
   */
  inline tchecker::parsing::process_declaration_t const & process() const { return *_process; }

  /*!
   \brief Accessor
   \return Event
   */
  inline tchecker::parsing::event_declaration_t const & event() const { return *_event; }

  /*!
   \brief Accessor
   \return Strength
   */
  inline enum tchecker::sync_strength_t strength() const { return _strength; }

private:
  std::shared_ptr<tchecker::parsing::process_declaration_t const> _process; /*!< Process */
  std::shared_ptr<tchecker::parsing::event_declaration_t const> _event;     /*!< Event */
  enum tchecker::sync_strength_t _strength;                                 /*!< Strength */

  friend std::ostream & operator<<(std::ostream & os, tchecker::parsing::sync_constraint_t const & c);
};

/*!
 \brief Output operator
 \param os : output stream
 \param c : synchronization constraint
 \return os after c has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::parsing::sync_constraint_t const & c);

/*!
 \class sync_declaration_t
 \brief Declaration of synchronized process events
 */
class sync_declaration_t final : public tchecker::parsing::inner_declaration_t {
public:
  /*!
   \brief Constructor
   \param syncs : synchronization constraints
   \param attr : attributes
   \param context : contextual information for declaration (position in input file, etc)
   \pre syncs has size > 0
   \pre there is no two events with the same process in syncs
   \post this keeps pointers on the synchronisation constraints in syncs
   \throws std::invalid_argument : if syncs size == 0, or if two events in syncs have the same process
   */
  sync_declaration_t(std::vector<std::shared_ptr<tchecker::parsing::sync_constraint_t>> const & syncs,
                     tchecker::parsing::attributes_t const & attr, std::string const & context);

  /*!
   \brief Copy constructor
   */
  sync_declaration_t(tchecker::parsing::sync_declaration_t const &);

  /*!
   \brief Move constructor
   */
  sync_declaration_t(tchecker::parsing::sync_declaration_t &&);

  /*!
   \brief Destructor
   */
  virtual ~sync_declaration_t();

  /*!
   \brief Assignment
   */
  tchecker::parsing::sync_declaration_t & operator=(tchecker::parsing::sync_declaration_t const &);

  /*!
   \brief Move assignment
   */
  tchecker::parsing::sync_declaration_t & operator=(tchecker::parsing::sync_declaration_t &&);

  /*!
   \brief Iterator over attributes
   */
  using const_iterator_t = std::vector<std::shared_ptr<tchecker::parsing::sync_constraint_t>>::const_iterator;

  /*!
   \brief Accessor
   \return Range [begin;end) of process events
   */
  inline tchecker::range_t<const_iterator_t> sync_constraints() const
  {
    return tchecker::make_range(_syncs.begin(), _syncs.end());
  }

  /*!
   \brief Visit
   \post this has been visited by v
   */
  virtual void visit(tchecker::parsing::declaration_visitor_t & v) const override;

  /*!
   \brief Output the declaration
   \param os : output stream
   \post this declaration has been output to os
   \return os after this declaration has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

private:
  std::vector<std::shared_ptr<tchecker::parsing::sync_constraint_t>> _syncs; /*!< Synchronization vector */
};

/*!
 \class system_declaration_t
 \brief System declaration
 */
class system_declaration_t final : public tchecker::parsing::declaration_t {
public:
  /*!
   \brief Constructor
   \param name : system name
   \param attr : attributes
   \param context : contextual information for declaration (position in input file, etc)
   \pre name is not empty
   \throw std::invalid_argument : if name is empty
   */
  system_declaration_t(std::string const & name, tchecker::parsing::attributes_t const & attr, std::string const & context);

  /*!
   \brief Copy constructor
   */
  system_declaration_t(tchecker::parsing::system_declaration_t const &);

  /*!
   \brief Move constructor
   */
  system_declaration_t(tchecker::parsing::system_declaration_t &&);

  /*!
   \brief Destructor
   \post
   */
  virtual ~system_declaration_t();

  /*!
   \brief Assignment
   */
  tchecker::parsing::system_declaration_t & operator=(tchecker::parsing::system_declaration_t const &);

  /*!
   \brief Move assignment
   */
  tchecker::parsing::system_declaration_t & operator=(tchecker::parsing::system_declaration_t &&);

  /*!
   \brief Accessor
   \return Name
   */
  inline std::string const & name() const { return _name; }

  /*!
   \brief Type of iterator on declarations
   */
  using const_iterator_t = std::vector<std::shared_ptr<tchecker::parsing::inner_declaration_t>>::const_iterator;

  /*!
   \brief Accessor
   \return Range [begin;end) of declarations
   */
  tchecker::range_t<const_iterator_t> declarations() const;

  /*!
   \brief Accessor
   \param name : name of declaration
   \return int declaration with name if any, nullptr otherwise
   */
  std::shared_ptr<tchecker::parsing::int_declaration_t const> get_int_declaration(std::string const & name) const;

  /*!
   \brief Insert an int declaration
   \param d : int declaration
   \pre d != nullptr (checked by assertion)
   \return true if d has been inserted, false otherwise
   \note this keeps a pointer on d
   */
  bool insert_int_declaration(std::shared_ptr<tchecker::parsing::int_declaration_t> const & d);

  /*!
   \brief Accessor
   \param name : name of declaration
   \return clock declaration with name if any, nullptr otherwise
   */
  std::shared_ptr<tchecker::parsing::clock_declaration_t const> get_clock_declaration(std::string const & name) const;

  /*!
   \brief Insert a clock declaration
   \param d : clock declaration
   \pre d != nullptr (checked by assertion)
   \return true if d has been inserted, false otherwise
   \note this keeps a pointer on d
   */
  bool insert_clock_declaration(std::shared_ptr<tchecker::parsing::clock_declaration_t> const & d);

  /*!
   \brief Accessor
   \param name : name of declaration
   \return process declaration with name if any, nullptr otherwise
   */
  std::shared_ptr<tchecker::parsing::process_declaration_t const> get_process_declaration(std::string const & name) const;

  /*!
   \brief Insert a process declaration
   \param d : process declaration
   \pre d != nullptr (checked by assertion)
   \return true if d has been inserted, false otherwise
   \note this keeps a pointer on d
   */
  bool insert_process_declaration(std::shared_ptr<tchecker::parsing::process_declaration_t> const & d);

  /*!
   \brief Accessor
   \param name : name of declaration
   \return event declaration with name if any, nullptr otherwise
   */
  std::shared_ptr<tchecker::parsing::event_declaration_t const> get_event_declaration(std::string const & name) const;

  /*!
   \brief Insert an event declaration
   \param d : event declaration
   \pre d != nullptr (checked by assertion)
   \return true if d has been inserted, false otherwise
   \note this keeps a pointer on d
   */
  bool insert_event_declaration(std::shared_ptr<tchecker::parsing::event_declaration_t> const & d);

  /*!
   \brief Accessor
   \param proc : process name
   \param name : location name
   \return location declaration with process proc and name if any, nullptr
   otherwise
   */
  std::shared_ptr<tchecker::parsing::location_declaration_t const> get_location_declaration(std::string const & proc,
                                                                                            std::string const & name) const;

  /*!
   \brief Insert a location declaration
   \param d : location declaration
   \pre d != nullptr (checked by assertion)
   \return true if d has been inserted, false otherwise
   \note this keeps a pointer on d
   */
  bool insert_location_declaration(std::shared_ptr<tchecker::parsing::location_declaration_t> const & d);

  /*!
   \brief Insert an edge declaration
   \param d : edge declaration
   \pre d != nullptr (checked by assertion)
   \return true if d has been inserted, false otherwise
   \note this keeps a pointer on d
   */
  bool insert_edge_declaration(std::shared_ptr<tchecker::parsing::edge_declaration_t> const & d);

  /*!
   \brief Insert a synchronization declaration
   \param d : synchronization declaration
   \pre d != nullptr (checked by assertion)
   \return true if d has been inserted, false otherwise
   \note this keeps a pointer on d
   */
  bool insert_sync_declaration(std::shared_ptr<tchecker::parsing::sync_declaration_t> const & d);

  /*!
   \brief Visit
   \post this has been visited by v
   */
  virtual void visit(tchecker::parsing::declaration_visitor_t & v) const override;

  /*!
   \brief Output the declaration
   \param os : output stream
   \post this declaration has been output to os
   \return os after this declaration has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

private:
  /*!
   \brief Type of declarations index
   */
  template <class T> using declaration_map_t = std::unordered_map<std::string, std::shared_ptr<T>>;

  /*!
   \brief Accessor
   \tparam T : type of declaration
   \param name : name of declaration
   \param m : declaration map
   \return declaration of type T with key name in map m, nullptr if no such
   declaration in m
   */
  template <class T> std::shared_ptr<T const> get_declaration(std::string const & name, declaration_map_t<T> const & m) const
  {
    auto it = m.find(name);
    if (it == m.end())
      return nullptr;
    return it->second;
  }

  /*!
   \brief Insert a declaration
   \tparam T : type of declaration
   \param name : name of declaration
   \param d : declaration
   \param m : declaration map
   \pre d is not nullptr (checked by assertion), m does not already contain
   a declaration with key name
   \return true if d has been inserted in m with key name, false otherwise
   (m already contains a declaration with key name)
   \post d has been inserted in m with key name and in the list of
   declarations if returned value is true
   */
  template <class T> bool insert_declaration(std::string const & name, std::shared_ptr<T> const & d, declaration_map_t<T> & m)
  {
    assert(d != nullptr);
    if (!m.insert({name, d}).second)
      return false;
    _decl.push_back(d);
    return true;
  }

  /*!
   \brief Accessor
   \param process_name : process name
   \param name : location name
   \return key to location map for location name in process process_name
   */
  inline static std::string location_map_key(std::string const & process_name, std::string const & name)
  {
    return (process_name + ":" + name);
  }

  std::string _name;                                                  /*!< Name */
  std::vector<std::shared_ptr<inner_declaration_t>> _decl;            /*!< Declarations */
  declaration_map_t<tchecker::parsing::int_declaration_t> _ints;      /*!< int declarations index */
  declaration_map_t<tchecker::parsing::clock_declaration_t> _clocks;  /*!< clock declaration index */
  declaration_map_t<tchecker::parsing::process_declaration_t> _procs; /*!< process declaration index */
  declaration_map_t<tchecker::parsing::event_declaration_t> _events;  /*!< event declaration index */
  declaration_map_t<tchecker::parsing::location_declaration_t> _locs; /*!< location declaration index */
};

/*!
 \class declaration_visitor_t
 \brief Visitor for declarations
 */
class declaration_visitor_t {
public:
  /*!
   \brief Constructor
   */
  declaration_visitor_t() = default;

  /*!
   \brief Copy constructor
   */
  declaration_visitor_t(tchecker::parsing::declaration_visitor_t const &) = default;

  /*!
   \brief Move constructor
   */
  declaration_visitor_t(tchecker::parsing::declaration_visitor_t &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~declaration_visitor_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::parsing::declaration_visitor_t & operator=(tchecker::parsing::declaration_visitor_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::parsing::declaration_visitor_t & operator=(tchecker::parsing::declaration_visitor_t &&) = default;

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::parsing::system_declaration_t const & d) = 0;
  virtual void visit(tchecker::parsing::clock_declaration_t const & d) = 0;
  virtual void visit(tchecker::parsing::int_declaration_t const & d) = 0;
  virtual void visit(tchecker::parsing::process_declaration_t const & d) = 0;
  virtual void visit(tchecker::parsing::event_declaration_t const & d) = 0;
  virtual void visit(tchecker::parsing::location_declaration_t const & d) = 0;
  virtual void visit(tchecker::parsing::edge_declaration_t const & d) = 0;
  virtual void visit(tchecker::parsing::sync_declaration_t const & d) = 0;
};

} // end of namespace parsing

} // end of namespace tchecker

#endif // TCHECKER_PARSING_DECLARATION_HH
