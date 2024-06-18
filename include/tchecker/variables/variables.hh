/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VARIABLES_HH
#define TCHECKER_VARIABLES_HH

#include <functional>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>

#include <boost/container/flat_map.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/index.hh"
#include "tchecker/utils/iterator.hh"

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
   \brief Size type
   */
  using size_t = unsigned int;

  /*!
   \brief Constructor
   \param size : variable size
   \pre size > 0
   \throw std::invalid_argument: if the precondition is violated
   */
  size_info_t(tchecker::size_info_t::size_t size);

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
  tchecker::size_info_t & operator=(tchecker::size_info_t const &) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::size_info_t & operator=(tchecker::size_info_t &&) = default;

  /*!
   \brief Accessor
   \return size of the variable
   */
  inline constexpr tchecker::size_info_t::size_t size() const { return _size; }

  /*!
   \brief Flatten variable information
   \post size has value 1
   */
  inline constexpr void flatten() { _size = 1; }

protected:
  tchecker::size_info_t::size_t _size; /*!< Variable size */
};

/*!
 \class variables_t
 \brief Definition of variables (of unspecified type)
 \tparam ID : type of variable identifiers, should be an integer type
 \tparam INFO : type of variable informations
 \tparam INDEX : type of variable index, should derive from tchecker::index_t
 */
template <class ID, class INFO, class INDEX> class variables_t {

  static_assert(std::is_base_of<tchecker::index_t<ID, std::string>, INDEX>::value, "Bad template parameter INDEX");
  static_assert(std::numeric_limits<ID>::is_integer, "ID musr be an integer type");

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
  tchecker::variables_t<ID, INFO, INDEX> & operator=(tchecker::variables_t<ID, INFO, INDEX> const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::variables_t<ID, INFO, INDEX> & operator=(tchecker::variables_t<ID, INFO, INDEX> &&) = default;

  /*!
   \brief Accessor
   \return index of variables
   */
  inline constexpr INDEX const & index() const { return _index; }

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
   \brief Checks if a variable exists
   \param name : variable name
   \return true if the variable exists
   */
  inline bool is_variable(std::string const & name) const { return _index.find_value(name) != _index.end_value_map(); }

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
  inline std::size_t size() const { return _index.size(); }

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
      auto && ok = _info.insert(std::make_pair(id, info)).second;
      if (!ok) {
        _index.erase(id);
        throw std::invalid_argument("Variable " + name + " cannot be declared");
      }
    }
    catch (std::exception const & e) {
      throw std::invalid_argument("Variable " + name + " cannot be declared");
    }
  }

  /*!
   \brief Type of range over variable identifiers
   */
  using identifiers_range_t = typename INDEX::keys_range_t;

  /*!
   \brief Accessor
   \return range of variable identifiers
   */
  identifiers_range_t identifiers() const { return _index.keys(); }

protected:
  INDEX _index;     /*!< Index of variables */
  info_map_t _info; /*!< Map variable ID -> informations */
};

/*!
 \brief Declare flattened variables corresponding to a variable with size
 \tparam ID : type of variable identifier
 \tparam INFO : type of variable information, should derive from tchecker::size_info_t
 \param id : variable identifier
 \param name : variable name
 \param info : variable informations
 \param declare : flat variable declaration function
 \pre same preconditions as function `declare`
 \post info.size() consecutive flat variables have been declared from identifier `id`.
 Each flat variable has information `info`, except for `size` which has been set to 1.
 Variables have been declared using function `declare`
 \throw as function declare
 */
template <class ID, class INFO>
void declare_flattened_variable(ID id, std::string const & name, INFO const & info,
                                std::function<void(ID, std::string const &, INFO const &)> declare)
{
  static_assert(std::is_base_of<tchecker::size_info_t, INFO>::value, "INFO should provide variable size");

  typename INFO::size_t size = info.size();

  if (size == 1)
    declare(id, name, info);
  else {
    INFO flat_info{info};
    flat_info.flatten();

    for (typename INFO::size_t i = 0; i < size; ++i) {
      std::stringstream ss;
      ss << name << "[" << i << "]";
      declare(id + i, ss.str(), flat_info);
    }
  }
}

/*!
 \class size_variables_t
 \brief Definition of variables with a size (arrays)
 \tparam ID : type of variable identifiers
 \tparam INFO : type of variable informations, should derive from tchecker::size_info_t
 \tparam INDEX : type of variable index, should derive from tchecker::index_t
 \note Variables identifiers take into account the size of the variables: when a variable is declared
 all identifiers needed by the variable are reserved
 */
template <class ID, class INFO, class INDEX> class size_variables_t : public tchecker::variables_t<ID, INFO, INDEX> {

  static_assert(std::is_base_of<tchecker::size_info_t, INFO>::value, "INFO must provide variable size");
  static_assert(std::numeric_limits<ID>::min() <= std::numeric_limits<typename INFO::size_t>::min(), "ID type is too small");
  static_assert(std::numeric_limits<ID>::max() >= std::numeric_limits<typename INFO::size_t>::max(), "ID type is too small");

public:
  /*!
   \brief Constructor
   \post the first variable identifier is 0
   */
  size_variables_t() : _first_id(0), _next_id(0) {}

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
  tchecker::size_variables_t<ID, INFO, INDEX> & operator=(tchecker::size_variables_t<ID, INFO, INDEX> const &) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::size_variables_t<ID, INFO, INDEX> & operator=(tchecker::size_variables_t<ID, INFO, INDEX> &&) = default;

  /*!
   \brief Declare a variable
   \param name : variable name
   \param info : variable informations
   \pre there is no declared variable with base name `name`
   there are enough variable identifiers left to declare info.size() variables
   \post a variable with base name `name` of dimension info.size(), with information `info` has
   been declared. This variable has been given the next available identifier (i.e. declared variables
   use consecutive identifiers)
   The info.size() identifiers starting from this variable identifier have been reserved for
   this variable.
   \return the identifier associated to the declared variable
   \throw std::invalid_argument : if the precondition is violated
   */
  ID declare(std::string const & name, INFO const & info)
  {
    ID id = _next_id;
    declare(id, name, info);
    return id;
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

    typename INFO::size_t size = info.size();

    if (id + size < id) // overflow
      throw std::invalid_argument("Not enough variable identifiers left");

    tchecker::variables_t<ID, INFO, INDEX>::declare(id, name, info);
    if ((id < _first_id) || (_first_id == _next_id))
      _first_id = id;
    _next_id = id + size;
  }

protected:
  using tchecker::variables_t<ID, INFO, INDEX>::declare;

  ID _first_id; /*!< First used variable identifier */
  ID _next_id;  /*!< Next available variable identifier */
};

/*!
 \class flat_variables_t
 \brief Definition of flat variables (i.e. variables of size 1)
 \tparam ID : type of variable identifiers
 \tparam INFO : type of variable informations, should derive from tchecker::size_info_t
 \tparam INDEX : type of variable index, should derive from tchecker::index_t
 */
template <class ID, class INFO, class INDEX> class flat_variables_t : public tchecker::size_variables_t<ID, INFO, INDEX> {
public:
  /*!
   \brief Default constructor
   */
  flat_variables_t() = default;

  /*!
   \brief Constructor
   \param v : variables
   \pre variable identifiers in v do not overlap
   \post this has been built from flattening every variable in v. Each variable in v
   has been transformed in v's size consecutive variables of size 1 with same information
   as in v (except for the size). The variable indices start from the index of the corresponding
   base variable in v
   \throw std::invalid_argument : if variables in v overlap
   */
  flat_variables_t(tchecker::variables_t<ID, INFO, INDEX> const & v)
  {
    for (auto && [id, name] : v.index())
      tchecker::declare_flattened_variable<ID, INFO>(
          id, name, v.info(id), [&](ID flat_id, std::string const & flat_name, INFO const & flat_info) {
            tchecker::size_variables_t<ID, INFO, INDEX>::declare(flat_id, flat_name, flat_info);
          });
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
  tchecker::flat_variables_t<ID, INFO, INDEX> & operator=(tchecker::flat_variables_t<ID, INFO, INDEX> const &) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::flat_variables_t<ID, INFO, INDEX> & operator=(tchecker::flat_variables_t<ID, INFO, INDEX> &&) = default;

  /*!
   \brief Declare a variable
   \param name : variable name
   \param info : variable informations
   \pre there is no declared variable with base name `name`
   info.size() == 1
   there is at least one variable identifier left
   \post a variable with base name `name` and information `info` has been declared.
   This variable has been allocated the next available identifier
   \return identifier of the declared variable
   \throw std::invalid_argument : if the precondition is violated
   */
  ID declare(std::string const & name, INFO const & info)
  {
    if (info.size() != 1)
      throw std::invalid_argument("Variable " + name + " should have size 1");
    return tchecker::size_variables_t<ID, INFO, INDEX>::declare(name, info);
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

/*!
\brief Type of variable
*/
enum variable_kind_t {
  VK_DECLARED,  /*!< As declared */
  VK_FLATTENED, /*!< Flattedned */
};

/*!
 \class array_variables_t
 \brief Declaration of array variables and corresponding flattened variables
 \tparam ID : type of variable identifiers
 \tparam INFO : type of variable informations, should derive from tchecker::size_info_t
 \tparam INDEX : type of variable index, should derive from tchecker::index_t
 */
template <class ID, class INFO, class INDEX> class array_variables_t : public tchecker::size_variables_t<ID, INFO, INDEX> {
public:
  using tchecker::size_variables_t<ID, INFO, INDEX>::size_variables_t;

  /*!
   \brief Declare a variable
   \param id : variable identifier
   \param name : variable name
   \param info : variable informations
   \pre there is no declared variable with base name `name`
   `id` is greater or equal to the next available variable identifier
   info.size() >= 1
   there are enough variable identifiers left to declare info.size() variables
   \post a variable with base name `name` of dimension info.size(), with information `info` has
   been declared. This variable has identifier `id`.
   The info.size() identifiers starting from this variable identifier have been reserved for
   this variable.
   Corresponding info.size() consecutive flat variables starting at identifier `id` have been
   declared. The flat variables have information `info` except for size which ahs been set to 1.
   \throw std::invalid_argument : if the precondition is violated
   */
  void declare(ID id, std::string const & name, INFO const & info)
  {
    tchecker::size_variables_t<ID, INFO, INDEX>::declare(id, name, info);
    tchecker::declare_flattened_variable<ID, INFO>(id, name, info,
                                                   [&](ID flat_id, std::string const & flat_name, INFO const & flat_info) {
                                                     _flattened_variables.declare(flat_id, flat_name, flat_info);
                                                   });
  }

  /*!
   \brief Declare a variable
   \param name : variable name
   \param info : variable informations
   \pre there is no declared variable with base name `name`
   info.size() >= 1
   there are enough variable identifiers left to declare info.size() variables
   \post a variable with base name `name` and information `info` has been declared.
   This variable has been allocated the next available identifier
   Corresponding info.size() consecutive flat variables starting at the same identifier as
   variable `name` have been declared. The flat variables have information `info` except
   for size which ahs been set to 1.
   \return identifier of the declared variable
   \throw std::invalid_argument : if the precondition is violated
   */
  ID declare(std::string const & name, INFO const & info)
  {
    ID id = tchecker::size_variables_t<ID, INFO, INDEX>::declare(name, info);
    tchecker::declare_flattened_variable<ID, INFO>(id, name, info,
                                                   [&](ID flat_id, std::string const & flat_name, INFO const & flat_info) {
                                                     _flattened_variables.declare(flat_id, flat_name, flat_info);
                                                   });
    return id;
  }

  /*!
   \brief Accessor
   \param kind : kind of variables
   \return Number of declared variables if kind = tchecker::DECLARED,
    numer of flattened variables if kind = tchecker::FLATTENED
   */
  inline constexpr std::size_t size(enum tchecker::variable_kind_t kind) const
  {
    switch (kind) {
    case tchecker::VK_DECLARED:
      return tchecker::size_variables_t<ID, INFO, INDEX>::size();
    case tchecker::VK_FLATTENED:
      return _flattened_variables.size();
    default:
      throw std::runtime_error("Unknown variable kind");
    }
  }

  /*!
   \brief Type of range of identifiers
   */
  using identifiers_range_t = typename tchecker::array_variables_t<ID, INFO, INDEX>::identifiers_range_t;

  /*!
   \brief Accessor
   \param kind : kind of variables
   \return Range (begin, end) of variable identifiers, `begin` is the first used
   identifier and `end` immediately follows the last used identifier
   The returned range corresponds to the identifiers of the declared variables if kind = tchecker::DECLARED,
   and the identifiers of the flattened variables if kind = tchecker::FLATTENED
   \note The identifiers in the returned range are sorted, but they may not be consecutive
   */
  inline identifiers_range_t identifiers(enum tchecker::variable_kind_t kind) const
  {
    switch (kind) {
    case tchecker::VK_DECLARED:
      return tchecker::size_variables_t<ID, INFO, INDEX>::identifiers();
    case tchecker::VK_FLATTENED:
      return _flattened_variables.identifiers();
    default:
      throw std::runtime_error("Unknown variable kind");
    }
  }

  /*!
   \brief Accessor
   \return flattened variables
   */
  inline constexpr tchecker::flat_variables_t<ID, INFO, INDEX> const & flattened() const { return _flattened_variables; }

protected:
  using tchecker::size_variables_t<ID, INFO, INDEX>::declare;

  tchecker::flat_variables_t<ID, INFO, INDEX> _flattened_variables; /*!< Flattened variables */
};

} // end of namespace tchecker

#endif // TCHECKER_VARIABLES_HH
