/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYNCPROD_LABEL_HH
#define TCHECKER_SYNCPROD_LABEL_HH

#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/index.hh"

/*!
 \file label.hh
 \brief Labels of locations for a synchronized product of processes
 */

namespace tchecker {

namespace syncprod {

/*!
 \class labels_t
 \brief Labels of locations for a synchronized product of processes
 */
class labels_t {
public:
  /*!
   \brief Add a label
   \param name : label name
   \pre label name is not declared yet
   \post label name has been added
   \throw std::invalid_argument : if label name is already declared
   */
  void add_label(std::string const & name);

  /*!
   \brief Accessor
   \return number of labels
   \note 0..labels_count()-1 is the range of label identifiers
   */
  inline std::size_t labels_count() const { return _labels_index.size(); }

  /*!
   \brief Type of range of labels identifiers
   */
  using labels_identifiers_range_t = tchecker::integer_range_t<tchecker::label_id_t>;

  /*!
   \brief Accessor
   \return range of labels identifiers 0..labels_count()-1
   */
  labels_identifiers_range_t labels_identifiers() const;

  /*!
   \brief Accessor
   \param name : label name
   \return identifier of label name
   \throw std::invalid_argument : if name is not a label
   */
  inline tchecker::label_id_t label_id(std::string const & name) const { return _labels_index.value(name); }

  /*!
   \brief Accessor
   \param id : label identifier
   \return name of label id
   \throw std::invalid_argument : if id is not a label
   */
  inline std::string const & label_name(tchecker::label_id_t id) const { return _labels_index.key(id); }

  /*!
   \brief Checks validity of label identifier
   \param id : label identifier
   \return true if id is a declared label identifier, false othwerise
   */
  bool is_label(tchecker::label_id_t id) const;

  /*!
   \brief Check validity of label name
   \param name : label name
   \return true if name is a declared label, false otherwise
   */
  bool is_label(std::string const & name) const;

private:
  tchecker::index_t<std::string, tchecker::label_id_t> _labels_index; /*!< Map label name <-> label identifier */
};

} // namespace syncprod

} // end of namespace tchecker

#endif // TCHECKER_SYNCPROD_LABEL_HH
