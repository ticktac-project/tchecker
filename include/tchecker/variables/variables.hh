/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VARIABLES_HH
#define TCHECKER_VARIABLES_HH

#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/container/flat_map.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/index.hh"

/*!
 \file variables.hh
 \brief Definition of variables
 */

namespace tchecker {
  
  /*!
   \class variables_t
   \brief Definition of variables (unspecified type)
   \tparam ID : type of variable identifiers
   \tparam INFO : type of variable informations
   \tparam INDEX : type of variable index, should derive from tchecker::index_t
   */
  template <class ID, class INFO, class INDEX>
  class variables_t {
    
    static_assert(std::is_base_of<tchecker::index_t<ID, std::string>, INDEX>::value,
                  "Bad template parameter INDEX");
    
  protected:
    /*!
     \brief Type of map ID -> info
     */
    using info_map_t = boost::container::flat_map<ID, INFO>;
  public:
    /*!
     \brief Type of identifier
     */
    using id_t = ID;
    
    /*!
     \brief Type of informations
     */
    using info_t = INFO;
    
    /*!
     \brief Type of index
     */
    using index_t = INDEX;
    
    
    /*!
     \brief Constructor
     */
    variables_t() = default;
    
    
    /*!
     \brief Copy constructor
     */
    variables_t(tchecker::variables_t<ID, INFO, INDEX> const &) = default;
    
    
    /*!
     \brief Move constructor
     */
    variables_t(tchecker::variables_t<ID, INFO, INDEX> &&) = default;
    
    
    /*!
     \brief Destructor
     */
    ~variables_t() = default;
    
    
    /*!
     \brief Assignment operator
     */
    tchecker::variables_t<ID, INFO, INDEX> & operator= (tchecker::variables_t<ID, INFO, INDEX> const &) = default;
    
    
    /*!
     \brief Move assignment operator
     */
    tchecker::variables_t<ID, INFO, INDEX> & operator= (tchecker::variables_t<ID, INFO, INDEX> &&) = default;
    
    
    /*!
     \brief Accessor
     \return index of variables
     */
    inline INDEX const & index() const
    {
      return _index;
    }
    
    
    /*!
     \brief Accessor
     \param name : variable name
     \return ID of name
     \pre name is a declared variable
     \throw std::invalid_argument if precondition is violated
     */
    inline ID id(std::string const & name) const
    {
      auto it = _index.find_value(name);
      if (it == _index.end_value_map())
        throw std::invalid_argument("unknown variable " + name);
      return _index.key(*it);
    }
    
    
    /*!
     \brief Accessor
     \param id : variable id
     \return name of variable id
     \pre id is a declared variable
     \throw std::invalid_argument id precondition is violated
     */
    inline std::string const & name(ID id) const
    {
      auto it = _index.find_key(id);
      if (it == _index.end_key_map())
        throw std::invalid_argument("unknown variable ID");
      return _index.value(*it);
    }
    
    
    /*!
     \brief Accessor
     \param id : variable ID
     \pre id is a declared variable
     \return informations for variable id
     \throw std::invalid_argument : if the precondition is violated
     */
    inline INFO const & info(ID id) const
    {
      auto it = _info.find(id);
      if (it == _info.end())
        throw std::invalid_argument("unknown variable ID");
      return it->second;
    }
    
    
    /*!
     \brief Accessor
     \return Number of variables
     */
    inline std::size_t size() const
    {
      return _index.size();
    }
    
    
    /*!
     \brief Declare a variable
     \param id : variable identifier
     \param name : variable name
     \param info : variable informations
     \pre there is no declared variable with id or name
     \throw std::invalid_argument : if the precondition is violated
     */
    void declare(ID id, std::string const & name, INFO const & info)
    {
      _index.add(id, name);
      _info.insert(std::make_pair(id, info));
    }
  protected:
    INDEX _index;      /*!< Index of variables */
    info_map_t _info;  /*!< Map variable ID -> informations */
  };
  
  
  
  
  /*!
   \class auto_id_variables_t
   \brief Definition of variables with automatic variable identfier computation.
   Guarantees that when an array `x` of dimension `dim` is declared, then all the
   `dim` variable identifiers from the identifier of `x` a reserved.
   \tparam ID : type of variable identifiers
   \tparam INFO : type of variable informations
   \tparam INDEX : type of variable index, should derive from tchecker::index_t
   */
  template <class ID, class INFO, class INDEX>
  class auto_id_variables_t : public tchecker::variables_t<ID, INFO, INDEX> {
  public:
    /*!
     \brief Constructor
     \post the first variable identifier is 0
     */
    auto_id_variables_t() : _next_id(0)
    {}
    
    /*!
     \brief Copy constructor
     */
    auto_id_variables_t(tchecker::auto_id_variables_t<ID, INFO, INDEX> const &) = default;
    
    /*!
     \brief Move constructor
     */
    auto_id_variables_t(tchecker::auto_id_variables_t<ID, INFO, INDEX> &&) = default;
    
    /*!
     \brief Destructor
     */
    ~auto_id_variables_t() = default;
    
    /*!
     \brief Assignment operator
     */
    tchecker::auto_id_variables_t<ID, INFO, INDEX> &
    operator= (tchecker::auto_id_variables_t<ID, INFO, INDEX> const &) = default;
    
    /*!
     \brief Move-assignment operator
     */
    tchecker::auto_id_variables_t<ID, INFO, INDEX> &
    operator= (tchecker::auto_id_variables_t<ID, INFO, INDEX> &&) = default;
    
    /*!
     \brief Declare a variable
     \param name : variable name
     \param dim : variable dimension (array)
     \param info : variable informations
     \pre there is no declared variable with base name `name`
     there are enough variable identifiers left to declare `dim` variables
     \post a variable with base name `name` of dimension `dim`, with information `info` has been
     declared.
     The `dim` variable identifiers starting from this variable identifier have been reserved for
     this variable.
     The next available variable identifier is equal to this variable identifier plus `dim`.
     \throw std::invalid_argument : if the precondition is violated
     */
    void declare(std::string const & name, ID dim, INFO const & info)
    {
      if (_next_id + dim < _next_id) // overflow
        throw std::invalid_argument("Not enough variable ID left");
      tchecker::variables_t<ID, INFO, INDEX>::declare(_next_id, name, info);
      _next_id += dim;
    }
  protected:
    using tchecker::variables_t<ID, INFO, INDEX>::declare;
    
    ID _next_id;  /*!< Next available variable identifier */
  };
  
} // end of namespace tchecker

#endif // TCHECKER_VARIABLES_HH
