/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VARIABLES_HH
#define TCHECKER_VARIABLES_HH

#include <sstream>
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
     \pre there is no declared variables with id or name
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
  
} // end of namespace tchecker

#endif // TCHECKER_VARIABLES_HH
