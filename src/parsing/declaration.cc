/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <ostream>
#include <stdexcept>

#include "tchecker/parsing/declaration.hh"

namespace tchecker {
  
  namespace parsing {
    
    
    /* declaration_t */
    
    tchecker::parsing::declaration_t * declaration_t::clone() const
    {
      return this->do_clone();
    }
    
    
    
    void
    declaration_t::visit(tchecker::parsing::declaration_visitor_t & v) const
    {
      this->do_visit(v);
    }
    
    
    
    
    /* clock_declaration_t */
    
    clock_declaration_t::clock_declaration_t(std::string const & name, unsigned int size)
    : _name(name),
    _size(size)
    {
      if (_name.empty())
        throw std::invalid_argument("clock declaration has empty name");
      
      if (_size < 1)
        throw std::invalid_argument("clock declaration has size < 1");
    }
    
    
    
    tchecker::parsing::declaration_t * clock_declaration_t::do_clone() const
    {
      return new tchecker::parsing::clock_declaration_t(_name, _size);
    }
    
    
    
    void clock_declaration_t::do_visit(tchecker::parsing::declaration_visitor_t & v) const
    {
      v.visit(*this);
    }
    
    
    
    std::ostream & clock_declaration_t::do_output(std::ostream & os) const
    {
      os << "clock:" << _size << ":" << _name;
      return os;
    }
    
    
    
    
    /* int_declaration_t */
    
    int_declaration_t::int_declaration_t(std::string const & name,
                                         unsigned int size,
                                         tchecker::integer_t min,
                                         tchecker::integer_t max,
                                         tchecker::integer_t init)
    : _name(name),
    _size(size),
    _min(min),
    _max(max),
    _init(init)
    {
      if (_name.empty())
        throw std::invalid_argument("int declaration has empty name");
      if (_size < 1)
        throw std::invalid_argument("int declaration has size < 1");
      if (_min > _max)
        throw std::invalid_argument("int declaration has min > max");
      if (_init < _min)
        throw std::invalid_argument("int declaration has init < min");
      if (_init > _max)
        throw std::invalid_argument("int declaration has init > max");
    }
    
    
    
    tchecker::parsing::declaration_t * int_declaration_t::do_clone() const
    {
      return new tchecker::parsing::int_declaration_t(_name, _size, _min, _max, _init);
    }
    
    
    
    void int_declaration_t::do_visit(tchecker::parsing::declaration_visitor_t & v) const
    {
      v.visit(*this);
    }
    
    
    
    std::ostream & int_declaration_t::do_output(std::ostream & os) const
    {
      os << "int:" << _size << ":" << _min << ":" << _max << ":";
      os << _init << ":" << _name;
      return os;
    }
    
    
    
    
    /* process_declaration_t */
    
    process_declaration_t::process_declaration_t(std::string const & name)
    : _name(name)
    {
      if (_name.empty())
        throw std::invalid_argument("process declaration has empty name");
    }
    
    
    
    tchecker::parsing::declaration_t * process_declaration_t::do_clone() const
    {
      return new tchecker::parsing::process_declaration_t(_name);
    }
    
    
    
    void process_declaration_t::do_visit(tchecker::parsing::declaration_visitor_t & v) const
    {
      v.visit(*this);
    }
    
    
    
    std::ostream & process_declaration_t::do_output(std::ostream & os) const
    {
      os << "process:" << _name;
      return os;
    }
    
    
    
    
    /* event_declaration_t */
    
    event_declaration_t::event_declaration_t(std::string const & name)
    : _name(name)
    {
      if (_name.empty())
        throw std::invalid_argument("event declaration has empty name");
    }
    
    
    
    tchecker::parsing::declaration_t * event_declaration_t::do_clone() const
    {
      return new tchecker::parsing::event_declaration_t(_name);
    }
    
    
    
    void event_declaration_t::do_visit(tchecker::parsing::declaration_visitor_t & v) const
    {
      v.visit(*this);
    }
    
    
    
    std::ostream & event_declaration_t::do_output(std::ostream & os) const
    {
      os << "event:" << _name;
      return os;
    }
    
    
    
    
    /* attr_t */
    
    attr_t::attr_t(std::string const & key, std::string const & value, std::string const & key_context,
                   std::string const & value_context)
    : _key(key),
    _value(value),
    _key_context(key_context),
    _value_context(value_context)
    {}
    
    
    std::ostream & operator<< (std::ostream & os, tchecker::parsing::attr_t const & attr)
    {
      os << attr.key() << ": " << attr.value();
      return os;
    }
    
    
    
    
    /* attributes_t */
    
    attributes_t::attributes_t(tchecker::parsing::attributes_t const & attr)
    {
      for (tchecker::parsing::attr_t const & p : attr)
        insert(new tchecker::parsing::attr_t(p));
    }
    
    
    
    attributes_t::attributes_t(tchecker::parsing::attributes_t && attr)
    : _attr(std::move(attr._attr))
    {}
    
    
    
    attributes_t::~attributes_t()
    {
      clear();
    }
    
    
    
    tchecker::parsing::attributes_t & attributes_t::operator= (tchecker::parsing::attributes_t const & attr)
    {
      if (this != &attr) {
        for (tchecker::parsing::attr_t const & p : attr)
          insert(new attr_t(p));
      }
      return *this;
    }
    
    
    
    tchecker::parsing::attributes_t & attributes_t::operator= (tchecker::parsing::attributes_t && attr)
    {
      if (this != &attr)
        _attr = std::move(attr._attr);
      return *this;
    }
    
    
    
    void attributes_t::clear()
    {
      for (auto & p : _attr)
        delete p.second;
      _attr.clear();
    }
    
    
    
    bool attributes_t::empty() const
    {
      return _attr.empty();
    }
    
    
    
    std::size_t attributes_t::size() const
    {
      return _attr.size();
    }
    
    
    
    bool attributes_t::insert(tchecker::parsing::attr_t const * attr)
    {
      return _attr.insert({attr->key(), attr}).second;
    }
    
    
    
    tchecker::parsing::attr_t const & attributes_t::at(std::string const & key) const
    {
      return * _attr.at(key);
    }
    
    
    
    tchecker::parsing::attributes_t::const_iterator_t attributes_t::find(std::string const & key) const
    {
      return tchecker::parsing::attributes_t::const_iterator_t(_attr.find(key));
    }
    
    
    
    tchecker::parsing::attributes_t::const_iterator_t::const_iterator_t
    (tchecker::parsing::attributes_t::map_t::const_iterator const & it)
    : tchecker::parsing::attributes_t::map_t::const_iterator(it)
    {}
    
    
    
    tchecker::parsing::attributes_t::const_iterator_t attributes_t::begin() const
    {
      return tchecker::parsing::attributes_t::const_iterator_t(_attr.begin());
    }
    
    
    
    tchecker::parsing::attributes_t::const_iterator_t attributes_t::end() const
    {
      return tchecker::parsing::attributes_t::const_iterator_t(_attr.end());
    }
    
    
    
    std::ostream & operator<< (std::ostream & os, tchecker::parsing::attributes_t const & attr)
    {
      os << "{";
      for (auto it = attr.begin(); it != attr.end(); ++it) {
        if (it != attr.begin())
          os << " : ";
        os << *it;
      }
      os << "}";
      return os;
    }
    
    
    
    
    /* location_declaration_t */
    
    location_declaration_t::location_declaration_t(std::string const & name,
                                                   tchecker::parsing::process_declaration_t const & process,
                                                   tchecker::parsing::attributes_t && attr)
    : _name(name),
    _process(process),
    _attr(std::move(attr))
    {
      if (_name.empty())
        throw std::invalid_argument("location declaration has empty name");
    }
    
    
    
    tchecker::parsing::declaration_t * location_declaration_t::do_clone() const
    {
      tchecker::parsing::attributes_t attr(_attr);
      return new tchecker::parsing::location_declaration_t(_name, _process, std::move(attr));
    }
    
    
    
    void location_declaration_t::do_visit(tchecker::parsing::declaration_visitor_t & v) const
    {
      v.visit(*this);
    }
    
    
    
    std::ostream & location_declaration_t::do_output(std::ostream & os) const
    {
      os << "location:" << _process.name() << ":" << _name << _attr;
      return os;
    }
    
    
    
    
    /* edge_declaration_t */
    
    edge_declaration_t::edge_declaration_t(tchecker::parsing::process_declaration_t const & process,
                                           tchecker::parsing::location_declaration_t const & src,
                                           tchecker::parsing::location_declaration_t const & tgt,
                                           tchecker::parsing::event_declaration_t const & event,
                                           tchecker::parsing::attributes_t && attr)
    : _process(process),
    _src(src),
    _tgt(tgt),
    _event(event),
    _attr(std::move(attr))
    {
      if (&_src.process() != &_process)
        throw std::invalid_argument("error, edge declaration has source location that does not belong to process");
      
      if (&_tgt.process() != &_process)
        throw std::invalid_argument("error, edge declaration has target location does not belong to process");
    }
    
    
    
    tchecker::parsing::declaration_t * edge_declaration_t::do_clone() const
    {
      tchecker::parsing::attributes_t attr(_attr);
      return new tchecker::parsing::edge_declaration_t(_process, _src, _tgt, _event, std::move(attr));
    }
    
    
    
    void edge_declaration_t::do_visit(tchecker::parsing::declaration_visitor_t & v) const
    {
      v.visit(*this);
    }
    
    
    
    std::ostream & edge_declaration_t::do_output(std::ostream &os) const
    {
      os << "edge:" << _process.name() << ":" << _src.name() << ":" << _tgt.name() << ":" << _event.name() << _attr;
      return os;
    }
    
    
    
    
    /* sync_constraint_t */
    
    sync_constraint_t::sync_constraint_t(tchecker::parsing::process_declaration_t const & process,
                                         tchecker::parsing::event_declaration_t const & event,
                                         enum tchecker::sync_strength_t strength)
    : _process(process),
    _event(event),
    _strength(strength)
    {}
    
    
    tchecker::parsing::sync_constraint_t * sync_constraint_t::clone() const
    {
      return new tchecker::parsing::sync_constraint_t(*this);
    }
    
    
    std::ostream & operator<< (std::ostream & os, tchecker::parsing::sync_constraint_t const & c)
    {
      return os << c._process.name() << "@" << c._event.name() << c._strength;
    }
    
    
    
    
    /* sync_declaration_t */
    
    sync_declaration_t::sync_declaration_t(std::vector<tchecker::parsing::sync_constraint_t const *> && syncs)
    : _syncs(std::move(syncs))
    {
      if (_syncs.size() == 0)
        throw std::invalid_argument("synchronization declaration is empty");
      
      for (tchecker::parsing::sync_constraint_t const * c : _syncs) {
        if (c == nullptr)
          throw std::invalid_argument("synchronization has nullptr constraint");
      }
      
      for (auto it1 = _syncs.begin(); it1 != _syncs.end(); ++it1) {
        auto it2 = it1;
        ++it2;
        for (; it2 != _syncs.end(); ++it2) {
          if (&(*it1)->process() == &(*it2)->process())
            throw std::invalid_argument("same process synchronized multiple times");
        }
      }
    }
    
    
    
    sync_declaration_t::~sync_declaration_t()
    {
      for (tchecker::parsing::sync_constraint_t const * c : _syncs)
        delete c;
    }
    
    
    
    tchecker::parsing::declaration_t * sync_declaration_t::do_clone() const
    {
      std::vector<tchecker::parsing::sync_constraint_t const *> syncs;
      for (tchecker::parsing::sync_constraint_t const * c : _syncs)
        syncs.push_back(c->clone());
      return new tchecker::parsing::sync_declaration_t(std::move(syncs));
    }
    
    
    
    void sync_declaration_t::do_visit(tchecker::parsing::declaration_visitor_t & v) const
    {
      v.visit(*this);
    }
    
    
    
    std::ostream & sync_declaration_t::do_output(std::ostream & os) const
    {
      os << "sync:";
      for (auto it = _syncs.begin(); it != _syncs.end(); ++it) {
        if (it != _syncs.begin())
          os << " : ";
        os << **it;
      }
      
      return os;
    }
    
    
    
    
    /* system_declaration_t */
    
    system_declaration_t::system_declaration_t(std::string const & name)
    : _name(name)
    {
      if (name.empty())
        throw std::invalid_argument("system declaration has empty name");
    }
    
    
    
    system_declaration_t::~system_declaration_t()
    {
      _ints.clear();
      _clocks.clear();
      _procs.clear();
      _events.clear();
      _locs.clear();
      for (inner_declaration_t const * d : _decl)
        delete d;
    }
    
    
    /*!
     \class system_dusplicator_t
     \brief Duplicates declarations in a system declaration
     */
    class system_duplicator_t : public tchecker::parsing::declaration_visitor_t {
    public:
      /*!
       \brief Constructor
       \param sysdecl : system declaration
       */
      system_duplicator_t(tchecker::parsing::system_declaration_t & sysdecl)
      : _sysdecl(sysdecl)
      {}
      
      
      /*!
       \brief Copy constructor
       \param dup : duplicator
       \post this is a copy of dup
       */
      system_duplicator_t(tchecker::parsing::system_duplicator_t const & dup) = default;
      
      
      /*!
       \brief Move constructor
       \param dup : duplicator
       \post dup has been moved to this
       */
      system_duplicator_t(tchecker::parsing::system_duplicator_t && dup) = default;
      
      
      /*!
       \brief Destructor
       */
      virtual ~system_duplicator_t() = default;
      
      
      /*!
       \brief Assignment operator (deleted)
       */
      tchecker::parsing::system_duplicator_t & operator= (tchecker::parsing::system_duplicator_t const & dup) = delete;
      
      
      /*!
       \brief Move assignment operator (deleted)
       */
      tchecker::parsing::system_duplicator_t & operator= (tchecker::parsing::system_duplicator_t && dup) = delete;
      
      
      /*!
       \brief Visitors
       \post All declarations have been cloned and inserted into sysdecl
       */
      virtual void visit(tchecker::parsing::system_declaration_t const & d)
      {
        for (auto const * decl : d.declarations())
          decl->visit(*this);
      }
      
      
      virtual void visit(tchecker::parsing::clock_declaration_t const & d)
      {
        auto const * d2 = dynamic_cast<tchecker::parsing::clock_declaration_t const *>(d.clone());
        _sysdecl.insert_clock_declaration(d2);
      }
      
      
      virtual void visit(int_declaration_t const & d)
      {
        auto const * d2 = dynamic_cast<tchecker::parsing::int_declaration_t const *>(d.clone());
        _sysdecl.insert_int_declaration(d2);
      }
      
      
      virtual void visit(process_declaration_t const & d)
      {
        auto const * d2 = dynamic_cast<tchecker::parsing::process_declaration_t const *>(d.clone());
        _sysdecl.insert_process_declaration(d2);
      }
      
      
      virtual void visit(event_declaration_t const & d)
      {
        auto const * d2 = dynamic_cast<tchecker::parsing::event_declaration_t const *>(d.clone());
        _sysdecl.insert_event_declaration(d2);
      }
      
      
      virtual void visit(location_declaration_t const & d)
      {
        auto const * d2 = dynamic_cast<tchecker::parsing::location_declaration_t const *>(d.clone());
        _sysdecl.insert_location_declaration(d2);
      }
      
      
      virtual void visit(edge_declaration_t const & d)
      {
        auto const * d2 = dynamic_cast<tchecker::parsing::edge_declaration_t const *>(d.clone());
        _sysdecl.insert_edge_declaration(d2);
      }
      
      
      virtual void visit(sync_declaration_t const & d)
      {
        auto const * d2 = dynamic_cast<tchecker::parsing::sync_declaration_t const *>(d.clone());
        _sysdecl.insert_sync_declaration(d2);
      }
    private:
      tchecker::parsing::system_declaration_t & _sysdecl;  /*!< System declaration */
    };
    
    
    
    tchecker::parsing::declaration_t * system_declaration_t::do_clone() const
    {
      tchecker::parsing::system_declaration_t * sysdecl = new system_declaration_t(_name);
      tchecker::parsing::system_duplicator_t duplicator(*sysdecl);
      this->visit(duplicator);
      return sysdecl;
    }
    
    
    
    void system_declaration_t::do_visit(tchecker::parsing::declaration_visitor_t & v) const
    {
      v.visit(*this);
    }
    
    
    
    std::ostream & system_declaration_t::do_output(std::ostream & os) const
    {
      os << "system:" << _name << std::endl;
      for (tchecker::parsing::inner_declaration_t const * d : _decl)
        os << *d << std::endl;
      return os;
    }
    
    
  } // end of namespace parsing
  
} // end of namespace tchecker
