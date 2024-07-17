/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_STATE_SPACE_HH
#define TCHECKER_TS_STATE_SPACE_HH

#include <memory>

/*!
 \file state_space.hh
 \brief State-space for transition systems with memory management
 */

namespace tchecker {

namespace ts {

/*!
 \class state_space_t
 \tparam TS : type of transition system
 \tparam R : type of state-space representation
 \brief This class owns a transition system (TS) and a representation of its state-space (R), and
 ensures that the state-space is destroyed before the transition system.
 \note This is necessary when the state-space representation uses memory that is allocated by the
 transition system, as is the case for most algorithms in TChecker that build graphs from transition
 systems, where nodes and edges store (parts of) the states and transitions.
 */
template <class TS, class R> class state_space_t {
public:
  /*!
   \brief Constructor
   \param ts : transition system
   \param args : arguments to a constructor of R
   \post this keeps a pointer on ts and owns an instance of R built from args
   \throw std::invalid_argument if ts is nullptr or if building R from args returns nullptr
   */
  template <class... ARGS>
  state_space_t(std::shared_ptr<TS> const & ts, ARGS &&... args)
      : _ts(ts), _ss(std::make_unique<R>(std::forward<ARGS>(args)...))
  {
    if (_ts.get() == nullptr)
      throw std::invalid_argument("state_space_t: nullptr ts is not allowed");
    if (_ss.get() == nullptr)
      throw std::invalid_argument("state_space_t: cannot build state-space representation from args");
  }

  /*!
   \brief Copy constructor (deleted)
   */
  state_space_t(tchecker::ts::state_space_t<TS, R> const &) = delete;

  /*!
   \brief Move constructor
   */
  state_space_t(tchecker::ts::state_space_t<TS, R> &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~state_space_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::ts::state_space_t<TS, R> & operator=(tchecker::ts::state_space_t<TS, R> const &) = delete;

  /*!
   \brief Move-assigment operator
   */
  tchecker::ts::state_space_t<TS, R> & operator=(tchecker::ts::state_space_t<TS, R> &&) = default;

  /*!
   \brief Accessor
   \return Reference to transition system
   */
  inline TS & ts() { return *_ts; }

  /*!
   \brief Accessor
   \return Const reference to transition system
   */
  inline TS const & ts() const { return *_ts; }

  /*!
   \brief Accessor
   \return Shared pointer to transition system
   */
  inline std::shared_ptr<TS> const & ts_ptr() { return _ts; }

  /*!
   \brief Accessor
   \return Reference to state-space representation
   */
  inline R & state_space() { return *_ss; }

  /*!
   \brief Accessor
   \return Const referene to state-space representation
   */
  inline R const & state_space() const { return *_ss; }

protected:
  std::shared_ptr<TS> _ts; /*!< Transition system */
private:
  std::unique_ptr<R> _ss; /*!< State-space representation */
};

} // namespace ts

} // namespace tchecker

#endif // TCHECKER_TS_STATE_SPACE_HH