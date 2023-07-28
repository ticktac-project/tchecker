/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_PATH_HH
#define TCHECKER_TS_PATH_HH

#include <map>
#include <memory>
#include <string>

#include "tchecker/graph/path.hh"

/*!
 \file path.hh
 \brief Graph representation of paths in a transition system
*/

namespace tchecker {

namespace ts {

/*!
 \class finite_path_t
 \brief Finite path in a transition system
 \tparam TS : type of transition system
 \tparam NODE : type of nodes on the path
 \tparam EDGE : type of edges on the path
*/
template <class TS, class NODE, class EDGE> class finite_path_t : public tchecker::graph::finite_path_t<NODE, EDGE> {
public:
  /*!
   \brief Constructor
   \param ts : a transition system
   \post this is an empty path
   \note this keeps a pointer to ts
   \note all nodes and edges added to this path shall be built from states and transitions in ts
  */
  finite_path_t(std::shared_ptr<TS> const & ts) : _ts(ts) {}

  /*!
   \brief Copy constructor
  */
  finite_path_t(tchecker::ts::finite_path_t<TS, NODE, EDGE> const &) = delete;

  /*!
   \brief Move constructor
  */
  finite_path_t(tchecker::ts::finite_path_t<TS, NODE, EDGE> &&) = delete;

  /*!
   \brief Destructor
  */
  ~finite_path_t() { tchecker::graph::finite_path_t<NODE, EDGE>::clear(); }

  /*!
   \brief Assignment operator
  */
  tchecker::ts::finite_path_t<TS, NODE, EDGE> & operator=(tchecker::ts::finite_path_t<TS, NODE, EDGE> const &) = delete;

  /*!
    \brief Move-assignment operator
   */
  tchecker::ts::finite_path_t<TS, NODE, EDGE> & operator=(tchecker::ts::finite_path_t<TS, NODE, EDGE> &&) = delete;

  using tchecker::graph::finite_path_t<NODE, EDGE>::attributes;

  /*!
   \brief Accessor
   \return transition system
  */
  inline TS & ts() const { return *_ts; }

  /*!
   \brief Accessor
   \return shared pointer to transition system
  */
  inline std::shared_ptr<TS> ts_ptr() const { return _ts; }

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(NODE const & n, std::map<std::string, std::string> & m) const { _ts->attributes(n.state_ptr(), m); }

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(EDGE const & e, std::map<std::string, std::string> & m) const
  {
    _ts->attributes(e.transition_ptr(), m);
  }

private:
  std::shared_ptr<TS> _ts; /*!< Transition system */
};

} // namespace ts

} // namespace tchecker

#endif // TCHECKER_REFZG_PATH_HH