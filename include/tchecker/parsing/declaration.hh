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
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/core/noncopyable.hpp>

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
     \class attr_t
     \brief Attribute for declarations
     */
    class attr_t {
    public:
      /*!
       \brief Constructor
       \param key : attribute key
       \param value : attribute value
       \param key_context : contextual information for key (position in input file, etc)
       \param value_context : contextual information for value (position in input file, etc)
       */
      attr_t(std::string const & key,
             std::string const & value,
             std::string const & key_context,
             std::string const & value_context);
      
      /*!
       \brief Copy constructor
       \param attr : attribute
       \post this is a copy of attr
       */
      attr_t(tchecker::parsing::attr_t const & attr) = default;
      
      /*!
       \brief Move constructor
       \param attr : attribute
       \post attr has been moved to this
       */
      attr_t(tchecker::parsing::attr_t && attr) = default;
      
      /*!
       \brief Destructor
       */
      ~attr_t() = default;
      
      /*!
       \brief Assignment operator
       \param attr : attribute
       \post this is a copy of attr
       */
      tchecker::parsing::attr_t & operator= (tchecker::parsing::attr_t const & attr) = default;
      
      /*!
       \brief Move assignment operator
       \param attr : attribute
       \post attr has been moved to this
       */
      tchecker::parsing::attr_t & operator= (tchecker::parsing::attr_t && attr) = default;
      
      /*!
       \brief Accessor
       \return Attribute key
       */
      inline std::string const & key() const
      {
        return _key;
      }
      
      /*!
       \brief Accessor
       \return Attribute value
       */
      inline std::string const & value() const
      {
        return _value;
      }
      
      /*!
       \brief Accessor
       \return Contextual information for key
       */
      inline std::string const & key_context() const
      {
        return _key_context;
      }
      
      /*!
       \brief Accessor
       \return Contextual information for value
       */
      inline std::string const & value_context() const
      {
        return _value_context;
      }
    private:
      std::string _key;            /*!< Key */
      std::string _value;          /*!< Value */
      std::string _key_context;    /*!< Contextual information for key */
      std::string _value_context;  /*!< Contextual information for value */
    };
    
    
    /*!
     \brief Output operator
     \param os : output stream
     \param attr : attribute
     \post attr has been output to os
     \return os after attr has been output
     */
    std::ostream & operator<< (std::ostream & os, tchecker::parsing::attr_t const & attr);
    
    
    
    
    /*!
     \class attributes_t
     \brief Attributes map for declarations
     */
    class attributes_t {
    public:
      /*!
       \brief Type of attributes map
       */
      using map_t = std::unordered_map<std::string, tchecker::parsing::attr_t const *>;
      
      /*!
       \brief Constructor
       \post this is an empty attributes map
       */
      attributes_t();
      
      /*!
       \brief Copy constructor
       \param attr : attributes to copy from
       \post this is a copy of attr
       */
      attributes_t(attributes_t const & attr);
      
      /*!
       \brief Move constructor
       \param attr : attributes to move
       \post attr has been moved to this
       */
      attributes_t(attributes_t && attr);
      
      /*!
       \brief Destructor
       \post All attributes in the map have been deleted
       */
      ~attributes_t();
      
      /*!
       \brief Assignement operator
       \param attr : attributes to assign from
       \post this is a copy of attr
       \return this
       */
      attributes_t & operator= (attributes_t const & attr);
      
      /*!
       \brief Move assignement operator
       \param attr : attributes to move
       \post attr has been moved to this
       \return this
       */
      attributes_t & operator= (attributes_t && attr);
      
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
       \return true if attr has been inserted in the map, false if an attribute with the same key already exists
       \note this takes ownership on attr
       */
      bool insert(tchecker::parsing::attr_t const * attr);
      
      
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
        inline tchecker::parsing::attr_t const & operator*() const
        {
          return *(*this)->second;
        }
      };
      
      /*!
       \brief Accessor
       \param key : key of attribute to access
       \return attribute at key in the map if any
       \throw std::out_of_range if there is no attibute with
       key in map
       */
      tchecker::parsing::attr_t const & at(std::string const & key) const;
      
      /*!
       \brief Accessor
       \param key : key of attribute to access
       \return iterator on atribute at key if any, end()
       otherwise
       */
      const_iterator_t find(std::string const & key) const;
      
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
    private:
      map_t _attr;  /*!< Attributes */
    };
    
    
    /*!
     \brief Output operator
     \param os : output stream
     \param attr : attributes map
     \post attr has been output to os
     \return os after attr has been output
     */
    std::ostream & operator<< (std::ostream & os, tchecker::parsing::attributes_t const & attr);
    
    
    
    
    
    
    /*!
     \class declaration_t
     \brief Declaration from input file
     \note Abstract base class
     */
    class declaration_t : private boost::noncopyable {
    public:
      /*!
       \brief Default constructor
       */
      declaration_t() = default;
      
      /*!
       \brief Constructor
       \param attr : attributes
       \post attr has been moved to this
       */
      explicit declaration_t(tchecker::parsing::attributes_t && attr);
            
      /*!
       \brief Destructor
       */
      virtual ~declaration_t() = default;
      
      /*!
       \brief Assignment operator (DELETED)
       */
      tchecker::parsing::declaration_t & operator= (tchecker::parsing::declaration_t const &) = delete;
      
      /*!
       \brief Move assignment operator (DELETED)
       */
      tchecker::parsing::declaration_t & operator= (tchecker::parsing::declaration_t &&) = delete;
    
      /*!
       \brief Clone
       \return Returns a clone of this
       */
      tchecker::parsing::declaration_t * clone() const;
      
      /*!
       \brief Visit
       \param v : visitor
       \post this has been visited by v
       */
      void visit(tchecker::parsing::declaration_visitor_t & v) const;
      
      /*!
       \brief Accessor
       \return Attributes
       */
      inline tchecker::parsing::attributes_t const & attributes() const
      {
        return _attr;
      }
    private:
      /*!
       \brief Clone
       */
      virtual tchecker::parsing::declaration_t * do_clone() const = 0;
      
      /*!
       \brief Visit
       */
      virtual void do_visit(tchecker::parsing::declaration_visitor_t & v) const = 0;
      
      /*!
       \brief Output the declaration
       \param os : output stream
       */
      virtual std::ostream & do_output(std::ostream & os) const = 0;
      
      friend std::ostream & operator<< (std::ostream & os, tchecker::parsing::declaration_t const & decl);
    protected:
      tchecker::parsing::attributes_t const _attr;                /*!< Attributes */
    };
    
    
    /*!
     \brief Output declaration
     \param os : output stream
     \param decl : declaration
     \post decl has been output to os
     \return os after outputing decl has been output
     */
    inline std::ostream & operator<< (std::ostream & os, tchecker::parsing::declaration_t const & decl)
    {
      return decl.do_output(os);
    }
    
    
    
    
    /*!
     \class inner_declaration_t
     */
    class inner_declaration_t : public tchecker::parsing::declaration_t {
    public:
      using tchecker::parsing::declaration_t::declaration_t;
      
      /*!
       \brief Destructor
       */
      virtual ~inner_declaration_t() = default;
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
       \pre name is not empty and size >= 1
       \throw std::invalid_argument : if name is empty or size < 1
       */
      clock_declaration_t(std::string const & name,
                          unsigned int size,
                          tchecker::parsing::attributes_t && attr);
      
      /*!
       \brief Destructor
       */
      virtual ~clock_declaration_t() = default;
      
      /*!
       \brief Accessor
       \return Name
       */
      inline std::string const & name() const
      {
        return _name;
      }
      
      /*!
       \brief Accessor
       \return Size
       */
      inline unsigned int size() const
      {
        return _size;
      }
    private:
      /*!
       \brief Clone
       \return A clone of this
       */
      virtual tchecker::parsing::declaration_t * do_clone() const;
      
      /*!
       \brief Visit
       \post this has been visited by v
       */
      virtual void do_visit(tchecker::parsing::declaration_visitor_t & v) const;
      
      /*!
       \brief Output the declaration
       \param os : output stream
       \post this declaration has been output to os
       \return os after this declaration has been output
       */
      virtual std::ostream & do_output(std::ostream & os) const;
      
      std::string const _name;    /*!< Name */
      unsigned int const _size;   /*!< Size */
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
       \pre name is not empty, size >= 1, min <= init <= max
       \throws std::invalid_argument : name is empty, or if size < 1, or
       not (min <= init <= max)
       */
      int_declaration_t(std::string const & name,
                        unsigned int size,
                        tchecker::integer_t min,
                        tchecker::integer_t max,
                        tchecker::integer_t init,
                        tchecker::parsing::attributes_t && attr);
      
      /*!
       \brief Destructor
       */
      virtual ~int_declaration_t() = default;
      
      /*!
       \brief Accessor
       \return Name
       */
      inline std::string const & name() const
      {
        return _name;
      }
      
      /*!
       \brief Accessor
       \return Size
       */
      inline unsigned int size() const
      {
        return _size;
      }
      
      /*!
       \brief Accessor
       \return Min value
       */
      inline tchecker::integer_t min() const
      {
        return _min;
      }
      
      /*!
       \brief Accessor
       \return Max value
       */
      inline tchecker::integer_t max() const
      {
        return _max;
      }
      
      /*!
       \brief Accessor
       \return Initial value
       */
      inline tchecker::integer_t init() const
      {
        return _init;
      }
    private:
      /*!
       \brief Clone
       \returns A clone of this
       */
      virtual tchecker::parsing::declaration_t * do_clone() const;
      
      /*!
       \brief Visit
       \post this has been visited by v
       */
      virtual void do_visit(tchecker::parsing::declaration_visitor_t & v) const;
      
      /*!
       \brief Output the declaration
       \param os : output stream
       \post this declaration has been output to os
       \return os after this declaration has been output
       */
      virtual std::ostream & do_output(std::ostream & os) const;
      
      std::string const _name;           /*!< Name */
      unsigned int const _size;          /*!< Size */
      tchecker::integer_t const _min;    /*!< Min value */
      tchecker::integer_t const _max;    /*!< Max value */
      tchecker::integer_t const _init;   /*!< Initial value */
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
       \pre name is not empty
       \throws std::invalid_argument : if name is empty
       */
      process_declaration_t(std::string const & name, tchecker::parsing::attributes_t && attr);
      
      /*!
       \brief Destructor
       */
      virtual ~process_declaration_t() = default;
      
      /*!
       \brief Accessor
       \return Name
       */
      inline std::string const & name() const
      {
        return _name;
      }
    private:
      /*!
       \brief Clone
       \return A clone of this
       */
      virtual tchecker::parsing::declaration_t * do_clone() const;
      
      /*!
       \brief Visit
       \post this has been visited by v
       */
      virtual void do_visit(tchecker::parsing::declaration_visitor_t & v) const;
      
      /*!
       \brief Output the declaration
       \param os : output stream
       \post this declaration has been output to os
       \return os after this declaration has been output
       */
      virtual std::ostream & do_output(std::ostream & os) const;
      
      std::string const _name;  /*!< Process name */
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
       \pre name is not empty
       \throws std::invalid_argument : if name is empty
       */
      event_declaration_t(std::string const & name, tchecker::parsing::attributes_t && attr);
      
      /*!
       \brief Destructor
       */
      virtual ~event_declaration_t() = default;
      
      /*!
       \brief Accessor
       \return Name
       */
      inline std::string const & name() const
      {
        return _name;
      }
    private:
      /*!
       \brief Clone
       \return A clone of this
       */
      virtual tchecker::parsing::declaration_t * do_clone() const;
      
      /*!
       \brief Visit
       \post this has been visited by v
       */
      virtual void do_visit(tchecker::parsing::declaration_visitor_t & v) const;
      
      /*!
       \brief Output the declaration
       \param os : output stream
       \post this declaration has been output to os
       \return os after this declaration has been output
       */
      virtual std::ostream & do_output(std::ostream & os) const;
      
      std::string const _name;   /*!< Event name */
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
       \pre name is not empty
       \throws std::invalid_argument : if name is empty
       \note attr is moved to this.
       */
      location_declaration_t(std::string const & name,
                             tchecker::parsing::process_declaration_t const & process,
                             tchecker::parsing::attributes_t && attr);
      
      /*!
       \brief Destructor
       */
      virtual ~location_declaration_t() = default;
      
      /*!
       \brief Accessor
       \return Name
       */
      inline std::string const & name() const
      {
        return _name;
      }
      
      /*!
       \brief Accessor
       \return Location process
       */
      inline tchecker::parsing::process_declaration_t const & process() const
      {
        return _process;
      }
    private:
      /*!
       \brief Clone
       \return A clone of this
       \note The clone shares pointers with this
       */
      virtual tchecker::parsing::declaration_t * do_clone() const;
      
      /*!
       \brief Visit
       \post this has been visited by v
       */
      virtual void do_visit(tchecker::parsing::declaration_visitor_t & v) const;
      
      /*!
       \brief Output the declaration
       \param os : output stream
       \post this declaration has been output to os
       \return os after this declaration has been output
       */
      virtual std::ostream & do_output(std::ostream & os) const;
      
      std::string const _name;                                    /*!< Name */
      tchecker::parsing::process_declaration_t const & _process;  /*!< Process */
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
       \pre src and tgt belong to process
       \throws std::invalid_argument : if src and tgt do not belong to process
       \note attr is moved to this
       */
      edge_declaration_t(tchecker::parsing::process_declaration_t const & process,
                         tchecker::parsing::location_declaration_t const & src,
                         tchecker::parsing::location_declaration_t const & tgt,
                         tchecker::parsing::event_declaration_t const & event,
                         tchecker::parsing::attributes_t && attr);
      
      /*!
       \brief Destructor
       */
      virtual ~edge_declaration_t() = default;
      
      /*!
       \brief Accessor
       \return Process
       */
      inline tchecker::parsing::process_declaration_t const & process() const
      {
        return _process;
      }
      
      /*!
       \brief Accessor
       \return Source location
       */
      inline tchecker::parsing::location_declaration_t const & src() const
      {
        return _src;
      }
      
      /*!
       \brief Accessor
       \return Target location
       */
      inline tchecker::parsing::location_declaration_t const & tgt() const
      {
        return _tgt;
      }
      
      /*!
       \brief Accessor
       \return Event
       */
      inline tchecker::parsing::event_declaration_t const & event() const
      {
        return _event;
      }
    private:
      /*!
       \brief Clone
       \return A clone of this
       \note The clone shares pointers with this
       */
      virtual tchecker::parsing::declaration_t * do_clone() const;
      
      /*!
       \brief Visit
       \post this has been visited by v
       */
      virtual void do_visit(tchecker::parsing::declaration_visitor_t & v) const;
      
      /*!
       \brief Output the declaration
       \param os : output stream
       \post this declaration has been output to os
       \return os after this declaration has been output
       */
      virtual std::ostream & do_output(std::ostream & os) const;
      
      tchecker::parsing::process_declaration_t const & _process;  /*!< Process */
      tchecker::parsing::location_declaration_t const & _src;     /*!< Source loc */
      tchecker::parsing::location_declaration_t const & _tgt;     /*!< Target loc */
      tchecker::parsing::event_declaration_t const & _event;      /*!< Event */
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
      sync_constraint_t(tchecker::parsing::process_declaration_t const & process,
                        tchecker::parsing::event_declaration_t const & event,
                        enum tchecker::sync_strength_t strength);
      
      /*!
       \brief Copy constructor
       \param sync : synchronization constraint
       \post this is copy of sync
       */
      sync_constraint_t(tchecker::parsing::sync_constraint_t const & sync) = default;
      
      /*!
       \brief Move constructor
       \param sync : synchronization constraint
       \post sync has been moved to this
       */
      sync_constraint_t(tchecker::parsing::sync_constraint_t && sync) = default;
      
      /*!
       \brief Destructor
       */
      ~sync_constraint_t() = default;
      
      /*!
       \brief Assignment operator (DELETED)
       */
      tchecker::parsing::sync_constraint_t & operator= (tchecker::parsing::sync_constraint_t const &) = delete;
      
      /*!
       \brief Move assignment operator (DELETED)
       */
      tchecker::parsing::sync_constraint_t & operator= (tchecker::parsing::sync_constraint_t &&) = delete;
      
      /*!
       \brief Clone
       \return A clone of this
       */
      sync_constraint_t * clone() const;
      
      /*!
       \brief Accessor
       \return Process
       */
      inline tchecker::parsing::process_declaration_t const & process() const
      {
        return _process;
      }
      
      /*!
       \brief Accessor
       \return Event
       */
      inline tchecker::parsing::event_declaration_t const & event() const
      {
        return _event;
      }
      
      /*!
       \brief Accessor
       \return Strength
       */
      inline enum tchecker::sync_strength_t strength() const
      {
        return _strength;
      }
    private:
      tchecker::parsing::process_declaration_t const & _process;   /*!< Process */
      tchecker::parsing::event_declaration_t const & _event;       /*!< Event */
      enum tchecker::sync_strength_t const _strength;              /*!< Strength */
      
      friend std::ostream & operator<< (std::ostream & os, tchecker::parsing::sync_constraint_t const & c);
    };
    
    
    /*!
     \brief Output operator
     \param os : output stream
     \param c : synchronization constraint
     \return os after c has been output
     */
    std::ostream & operator<< (std::ostream & os, tchecker::parsing::sync_constraint_t const & c);
    
    
    
    
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
       \pre syncs has size > 0, syncs does not contain nullptr, and there
       is no two events with the same process in syncs
       \throws std::invalid_argument : if syncs size == 0, or if syncs
       contains nullptr, or two events in syncs have the same process
       \note syncs has been moved to this
       */
      sync_declaration_t(std::vector<tchecker::parsing::sync_constraint_t const *> && syncs,
                         tchecker::parsing::attributes_t && attr);
      
      /*!
       \brief Destructor
       */
      virtual ~sync_declaration_t();
      
      /*!
       \brief Iterator over attributes
       */
      using const_iterator_t = std::vector<tchecker::parsing::sync_constraint_t const *>::const_iterator;
      
      /*!
       \brief Accessor
       \return Range [begin;end) of process events
       */
      inline tchecker::range_t<const_iterator_t> sync_constraints() const
      {
        return tchecker::make_range(_syncs.begin(), _syncs.end());
      }
    private:
      /*!
       \brief Clone
       \return A clone of this
       \note The clone shares pointers with this
       */
      virtual tchecker::parsing::declaration_t * do_clone() const;
      
      /*!
       \brief Visit
       \post this has been visited by v
       */
      virtual void do_visit(tchecker::parsing::declaration_visitor_t & v) const;
      
      /*!
       \brief Output the declaration
       \param os : output stream
       \post this declaration has been output to os
       \return os after this declaration has been output
       */
      virtual std::ostream & do_output(std::ostream & os) const;
      
      
      std::vector<tchecker::parsing::sync_constraint_t const *> const _syncs;  /*!< Synchronization vector */
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
       \pre name is not empty
       \throw std::invalid_argument : if name is empty
       */
      system_declaration_t(std::string const & name, tchecker::parsing::attributes_t && attr);
      
      /*!
       \brief Destructor
       \post
       */
      virtual ~system_declaration_t();
      
      /*!
       \brief Accessor
       \return Name
       */
      inline std::string const & name() const
      {
        return _name;
      }
      
      /*!
       \brief Type of iterator on declarations
       */
      using const_iterator_t = std::vector<tchecker::parsing::inner_declaration_t const *>::const_iterator;
      
      /*!
       \brief Accessor
       \return Range [begin;end) of declarations
       */
      inline tchecker::range_t<const_iterator_t> declarations() const
      {
        return tchecker::make_range(_decl.begin(), _decl.end());
      }
      
      /*!
       \brief Accessor
       \param name : name of declaration
       \return int declaration with name if any, nullptr otherwise
       */
      inline tchecker::parsing::int_declaration_t const * get_int_declaration(std::string const & name) const
      {
        return get_declaration(name, _ints);
      }
      
      /*!
       \brief Insert an int declaration
       \param d : int declaration
       \pre d != nullptr (checked by assertion)
       \return true if d has been inserted, false otherwise
       \note this takes owenship on d if inserted
       */
      inline bool insert_int_declaration(tchecker::parsing::int_declaration_t const * d)
      {
        assert(d != nullptr);
        return insert_declaration(d->name(), d, _ints);
      }
      
      /*!
       \brief Accessor
       \param name : name of declaration
       \return clock declaration with name if any, nullptr otherwise
       */
      inline tchecker::parsing::clock_declaration_t const * get_clock_declaration(std::string const & name) const
      {
        return get_declaration(name, _clocks);
      }
      
      /*!
       \brief Insert a clock declaration
       \param d : clock declaration
       \pre d != nullptr (checked by assertion)
       \return true if d has been inserted, false otherwise
       \note this takes ownership on d if inserted
       */
      inline bool insert_clock_declaration(tchecker::parsing::clock_declaration_t const * d)
      {
        assert(d != nullptr);
        return insert_declaration(d->name(), d, _clocks);
      }
      
      /*!
       \brief Accessor
       \param name : name of declaration
       \return process declaration with name if any, nullptr otherwise
       */
      inline tchecker::parsing::process_declaration_t const * get_process_declaration(std::string const & name) const
      {
        return get_declaration(name, _procs);
      }
      
      /*!
       \brief Insert a process declaration
       \param d : process declaration
       \pre d != nullptr (checked by assertion)
       \return true if d has been inserted, false otherwise
       \note this takes iwnership on d if inserted
       */
      inline bool insert_process_declaration(tchecker::parsing::process_declaration_t const * d)
      {
        assert(d != nullptr);
        return insert_declaration(d->name(), d, _procs);
      }
      
      /*!
       \brief Accessor
       \param name : name of declaration
       \return event declaration with name if any, nullptr otherwise
       */
      inline tchecker::parsing::event_declaration_t const * get_event_declaration(std::string const & name) const
      {
        return get_declaration(name, _events);
      }
      
      /*!
       \brief Insert an event declaration
       \param d : event declaration
       \pre d != nullptr (checked by assertion)
       \return true if d has been inserted, false otherwise
       \note this takes ownership on d if inserted
       */
      inline bool insert_event_declaration(tchecker::parsing::event_declaration_t const * d)
      {
        assert(d != nullptr);
        return insert_declaration(d->name(), d, _events);
      }
      
      /*!
       \brief Accessor
       \param proc : process name
       \param name : location name
       \return location declaration with process proc and name if any, nullptr
       otherwise
       */
      inline tchecker::parsing::location_declaration_t const * get_location_declaration(std::string const & proc,
                                                                                        std::string const & name) const
      {
        return get_declaration(location_map_key(proc, name), _locs);
      }
      
      /*!
       \brief Insert a location declaration
       \param d : location declaration
       \pre d != nullptr (checked by assertion)
       \return true if d has been inserted, false otherwise
       \note this takes ownership on d if inserted
       */
      inline bool insert_location_declaration(tchecker::parsing::location_declaration_t const * d)
      {
        assert(d != nullptr);
        auto key = location_map_key(d->process().name(), d->name());
        return insert_declaration(key, d, _locs);
      }
      
      /*!
       \brief Insert an edge declaration
       \param d : edge declaration
       \pre d != nullptr (checked by assertion)
       \return true if d has been inserted, false otherwise
       \note this takes ownership on d if inserted
       */
      inline bool insert_edge_declaration(tchecker::parsing::edge_declaration_t const * d)
      {
        assert(d != nullptr);
        _decl.push_back(d);
        return true;
      }
      
      /*!
       \brief Insert a synchronization declaration
       \param d : synchronization declaration
       \pre d != nullptr (checked by assertion)
       \return true if d has been inserted, false otherwise
       \note this takes ownership on d if inserted
       */
      inline bool insert_sync_declaration(tchecker::parsing::sync_declaration_t const * d)
      {
        assert(d != nullptr);
        _decl.push_back(d);
        return true;
      }
    private:
      /*!
       \brief Clone
       \return A clone of this
       */
      virtual tchecker::parsing::declaration_t * do_clone() const;
      
      /*!
       \brief Visit
       \post this has been visited by v
       */
      virtual void do_visit(tchecker::parsing::declaration_visitor_t & v) const;
      
      /*!
       \brief Output the declaration
       \param os : output stream
       \post this declaration has been output to os
       \return os after this declaration has been output
       */
      virtual std::ostream & do_output(std::ostream & os) const;
      
      
      /*!
       \brief Type of declarations index
       */
      template <class T>
      using declaration_map_t = std::unordered_map<std::string, T const *>;
      
      
      /*!
       \brief Accessor
       \tparam T : type of declaration
       \param name : name of declaration
       \param m : declaration map
       \return declaration of type T with key name in map m, nullptr if no such
       declaration in m
       */
      template <class T>
      T const * get_declaration(std::string const & name, declaration_map_t<T> const & m) const
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
      template <class T>
      bool insert_declaration(std::string const & name, T const * d, declaration_map_t<T> & m)
      {
        assert( d != nullptr );
        if ( ! m.insert({name, d}).second )
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
      
      
      std::string const _name;                                             /*!< Name */
      std::vector<inner_declaration_t const *> _decl;                      /*!< Declarations */
      declaration_map_t<tchecker::parsing::int_declaration_t> _ints;       /*!< int declarations index */
      declaration_map_t<tchecker::parsing::clock_declaration_t> _clocks;   /*!< clock declaration index */
      declaration_map_t<tchecker::parsing::process_declaration_t> _procs;  /*!< process declaration index */
      declaration_map_t<tchecker::parsing::event_declaration_t> _events;   /*!< event declaration index */
      declaration_map_t<tchecker::parsing::location_declaration_t> _locs;  /*!< location declaration index */
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
      tchecker::parsing::declaration_visitor_t & operator= (tchecker::parsing::declaration_visitor_t const &) = default;
      
      /*!
       \brief Move assignment operator
       */
      tchecker::parsing::declaration_visitor_t & operator= (tchecker::parsing::declaration_visitor_t &&) = default;
      
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
