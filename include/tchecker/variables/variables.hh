/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VARIABLES_HH
#define TCHECKER_VARIABLES_HH

#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>

#include <boost/container/flat_map.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/index.hh"

/*!
 \file variables.hh
 \brief Definition of variables
 */

namespace tchecker {
  
  /*!
   \class size_info_t
   \brief Information providing variable size
   */
  class size_info_t {
  public:
    /*!
     \brief Constructor
     \param size : variable size
     \pre size > 0
     \throw std::invalid_argument: if the precondition is violated
     */
    size_info_t(unsigned int size);
    
    /*!
     \brief Copy constructor
     */
    size_info_t(tchecker::size_info_t const &) = default;
    
    /*!
     \brief Move constructor
     */
    size_info_t(tchecker::size_info_t &&) = default;
    
    /*!
     \brief Destructor
     */
    ~size_info_t() = default;
    
    /*!
     \brief Assignment operator
     */
    tchecker::size_info_t & operator= (tchecker::size_info_t const &) = default;
    
    /*!
     \brief Move-assignment operator
     */
    tchecker::size_info_t & operator= (tchecker::size_info_t &&) = default;
    
    /*!
     \brief Accessor
     \return size of the variable
     */
    inline constexpr unsigned int size() const
    {
      return _size;
    }
    
    /*!
     \brief Flatten variable information
     \post size has value 1
     */
    inline constexpr void flatten()
    {
      _size = 1;
    }
  protected:
    unsigned int _size;  /*!< Variable size */
  };

  
  
  
  /*!
   \class variables_t
   \brief Definition of variables (of unspecified type)
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
    inline constexpr INDEX const & index() const
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
    inline constexpr ID id(std::string const & name) const
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
    inline constexpr std::string const & name(ID id) const
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
    inline constexpr INFO const & info(ID id) const
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
    inline constexpr std::size_t size() const
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
      try {
        _index.add(id, name);
        auto && [it, ok] = _info.insert(std::make_pair(id, info));
        if (! ok) {
          _index.erase(id);
          throw;
        }
      }
      catch (std::exception const & e) {
        throw std::invalid_argument("Variable cannot declared");
      }
    }
  protected:
    INDEX _index;      /*!< Index of variables */
    info_map_t _info;  /*!< Map variable ID -> informations */
  };
  
  
  
  
  /*!
   \class size_variables_t
   \brief Definition of variables with a size (arrays)
   \tparam ID : type of variable identifiers
   \tparam INFO : type of variable informations, should derive from tchecker::size_info_t
   \tparam INDEX : type of variable index, should derive from tchecker::index_t
   */
  template <class ID, class INFO, class INDEX>
  class size_variables_t : public tchecker::variables_t<ID, INFO, INDEX> {
    
    static_assert(std::is_base_of<tchecker::size_info_t, INFO>::value,
                  "INFO must provide variable size");
    
    static_assert(std::numeric_limits<ID>::min() <= std::numeric_limits<unsigned int>::min(),
                  "ID type is too small");
    
    static_assert(std::numeric_limits<ID>::max() >= std::numeric_limits<unsigned int>::max(),
                  "ID type is too small");
    
  public:
    /*!
     \brief Constructor
     \post the first variable identifier is 0
     */
    size_variables_t() : _next_id(0)
    {}
    
    /*!
     \brief Copy constructor
     */
    size_variables_t(tchecker::size_variables_t<ID, INFO, INDEX> const &) = default;
    
    /*!
     \brief Move constructor
     */
    size_variables_t(tchecker::size_variables_t<ID, INFO, INDEX> &&) = default;
    
    /*!
     \brief Destructor
     */
    ~size_variables_t() = default;
    
    /*!
     \brief Assignment operator
     */
    tchecker::size_variables_t<ID, INFO, INDEX> &
    operator= (tchecker::size_variables_t<ID, INFO, INDEX> const &) = default;
    
    /*!
     \brief Move-assignment operator
     */
    tchecker::size_variables_t<ID, INFO, INDEX> &
    operator= (tchecker::size_variables_t<ID, INFO, INDEX> &&) = default;
    
    /*!
     \brief Declare a variable
     \param name : variable name
     \param info : variable informations
     \pre there is no declared variable with base name `name`
     there are enough variable identifiers left to declare info.size() variables
     \post a variable with base name `name` of dimension info.size(), with information `info` has
     been declared. This variable has been given the first available index (i.e. declared variables
     use consecutive indices)
     The info.size() identifiers starting from this variable identifier have been reserved for
     this variable.
     \throw std::invalid_argument : if the precondition is violated
     */
    void declare(std::string const & name, INFO const & info)
    {
      declare(_next_id, name, info);
    }
    
    /*!
     \brief Declare a variable
     \param id : variable identifier
     \param name : variable name
     \param info : variable informations
     \pre there is no declared variable with base name `name`
     `id` is greater or equal to the next available variable identifier
     there are enough variable identifiers left to declare info.size() variables
     \post a variable with base name `name` of dimension info.size(), with information `info` has
     been declared. This variable has identifier `id`.
     The info.size() identifiers starting from this variable identifier have been reserved for
     this variable.
     \throw std::invalid_argument : if the precondition is violated
     */
    void declare(ID id, std::string const & name, INFO const & info)
    {
      if (id < _next_id)
        throw std::invalid_argument("Variable identifier is already used");
      
      unsigned int size = info.size();
      
      if (id + size < id) // overflow
        throw std::invalid_argument("Not enough variable identifiers left");
      
      tchecker::variables_t<ID, INFO, INDEX>::declare(id, name, info);
      _next_id = id + size;
    }
  protected:
    using tchecker::variables_t<ID, INFO, INDEX>::declare;
    
    ID _next_id;  /*!< Next available variable identifier */
  };
  
  
  
  
  /*!
   \class flat_variables_t
   \brief Definition of variables with a size (arrays)
   \tparam ID : type of variable identifiers
   \tparam INFO : type of variable informations, should derive from tchecker::size_info_t
   \tparam INDEX : type of variable index, should derive from tchecker::index_t
   */
  template <class ID, class INFO, class INDEX>
  class flat_variables_t : public tchecker::size_variables_t<ID, INFO, INDEX> {
  public:
    /*!
     \brief Default constructor
     */
    flat_variables_t() = default;
    
    /*!
     \brief Constructor
     \param v : variables
     \pre variables in v do not overlap
     \post this has been built from flattening every variable in v. Each variable in v
     has been transformed in v's size consecutive variables of size 1 with same information
     as in v (except for the size). The variable indices start from the index of the corresponding
     base variable in v
     \throw std::invalid_argument : if variables in v overlap
     */
    flat_variables_t(tchecker::variables_t<ID, INFO, INDEX> const & v)
    {
      for (auto && [id, name] : v.index()) {
        INFO flat_info = v.info(id);
        unsigned int size = flat_info.size();
        
        if (size == 1)
          tchecker::size_variables_t<ID, INFO, INDEX>::declare(id, name, flat_info);
        else {
          flat_info.flatten();
          
          for (unsigned int i = 0; i < size; ++i) {
            std::stringstream ss;
            ss << name << "[" << i << "]";
            tchecker::size_variables_t<ID, INFO, INDEX>::declare(id + i, ss.str(), flat_info);
          }
        }
      }
    }
    
    /*!
     \brief Copy constructor
     */
    flat_variables_t(tchecker::flat_variables_t<ID, INFO, INDEX> const &) = default;
    
    /*!
     \brief Move constructor
     */
    flat_variables_t(tchecker::flat_variables_t<ID, INFO, INDEX> &&) = default;
    
    /*!
     \brief Destructor
     */
    ~flat_variables_t() = default;
    
    /*!
     \brief Assignment operator
     */
    tchecker::flat_variables_t<ID, INFO, INDEX> &
    operator= (tchecker::flat_variables_t<ID, INFO, INDEX> const &) = default;
    
    /*!
     \brief Move-assignment operator
     */
    tchecker::flat_variables_t<ID, INFO, INDEX> &
    operator= (tchecker::flat_variables_t<ID, INFO, INDEX> &&) = default;
    

    /*!
     \brief Declare a variable
     \param name : variable name
     \param info : variable informations
     \pre there is no declared variable with base name `name`
     info.size() == 1
     there is at least one variable identifier left
     \post a variable with base name `name` and information `info` has been declared.
     This variable has been allocatd the next available identifier
     \throw std::invalid_argument : if the precondition is violated
     */
    void declare(std::string const & name, INFO const & info)
    {
      if (info.size() != 1)
        throw std::invalid_argument("Variable should have size 1");
      tchecker::size_variables_t<ID, INFO, INDEX>::declare(name, info);
    }
    
    /*!
     \brief Declare a variable
     \param id : variable identifier
     \param name : variable name
     \param info : variable informations
     \pre there is no declared variable with base name `name`
     `id` is greater or equal to the next available variable identifier
      info.size() == 1
     there are enough variable identifiers left to declare info.size() variables
     \post a variable with base name `name` of dimension info.size(), with information `info` has
     been declared. This variable has identifier `id`.
     The info.size() identifiers starting from this variable identifier have been reserved for
     this variable.
     \throw std::invalid_argument : if the precondition is violated
     */
    void declare(ID id, std::string const & name, INFO const & info)
    {
      if (info.size() != 1)
        throw std::invalid_argument("Variable should have size 1");
      tchecker::size_variables_t<ID, INFO, INDEX>::declare(id, name, info);
    }
  private:
    using tchecker::size_variables_t<ID, INFO, INDEX>::declare;
  };
  
} // end of namespace tchecker

#endif // TCHECKER_VARIABLES_HH
