/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_LOG_HH
#define TCHECKER_LOG_HH

#include <iostream>
#include <string>

/*!
 \file log.hh
 \brief Logging facility
 */

namespace tchecker {
  
  /*!
   \class log_t
   \brief Logging facility
   */
  class log_t {
  public:
    /*!
     \brief Default onstructor
     \post this log is empty. It does not output any message
     but its counts warnings and errors
     */
    log_t();
    
    /*!
     \brief Constructor
     \param os : output stream
     \pre os != nullptr
     \post this log is empty and writes messages to os
     \note this keeps a pointer to os
     \throws std::invalid_argument if os is nullptr
     */
    log_t(std::ostream * os);
    
    /*!
     \brief Copy constructor
     \param log : logging facility
     \post this is a copy of log
     */
    log_t(log_t const & log) = default;
    
    /*!
     \brief Move constructor
     \param log : logging facility
     \post log has been moved to this
     */
    log_t(log_t && log) = default;
    
    /*!
     \brief Destructor
     */
    ~log_t() = default;
    
    /*!
     \brief Assignment
     \param log : logging facility
     \post this is a copy of log
     */
    log_t & operator= (log_t const & log) = default;
    
    /*!
     \brief Move assignment
     \param log : logging facility
     \post log has been moved to this
     */
    log_t & operator= (log_t && log) = default;
    
    /*!
     \brief Write an error to the log
     \tparam LOCATION : type of locations, should have operator
     std::ostream & operator<< (std::ostream &, LOCATION const &)
     \param l : location of error
     \param msg : error message
     \post l and msg have been written to this log and an error has been
     counted
     */
    template <class LOCATION>
    inline void error(LOCATION const & l, std::string const & msg)
    {
      message(l, "ERROR, " + msg);
      ++ _error_count;
    }
    
    /*!
     \brief Write an error to the log
     \param msg : error message
     \post msg has been written to this log and an error has been
     counted
     */
    inline void error(std::string const & msg)
    {
      message("ERROR, " + msg);
      ++ _error_count;
    }
    
    /*!
     \brief Write a warning to the log
     \tparam LOCATION : type of locations, should have operator
     std::ostream & operator<< (std::ostream &, LOCATION const &)
     \param l : location of warning
     \param msg : warning message
     \post l and msg have been written to this log and a warning has been
     counted
     */
    template <class LOCATION>
    void warning(LOCATION const & l, std::string const & msg)
    {
      message(l, "WARNING, " + msg);
      ++ _warning_count;
    }
    
    /*!
     \brief Write a warning to the log
     \param msg : warning message
     \post msg has been written to this log and a warning has been
     counted
     */
    void warning(std::string const & msg)
    {
      message("WARNING, " + msg);
      ++ _warning_count;
    }
    
    /*!
     \brief Accessor
     \return Number of error messages in this log
     */
    inline constexpr std::size_t error_count() const
    {
      return _error_count;
    }
    
    /*!
     \brief Accessor
     \return Number of warning messages in this log
     */
    inline constexpr std::size_t warning_count() const
    {
      return _warning_count;
    }
    
    /*!
     \brief Display error and warning counts
     \post The numbers of errors and warnings in this log have been displayed
     to os
     */
    void display_counts() const;
    
    /*!
     \brief Reset
     \post Counters have been reset
     */
    void reset();
  private:
    /*!
     \brief Write a message to the log
     \tparam LOCATION : type of locations, should have operator
     std::ostream & operator<< (std::ostream &, LOCATION const &)
     \param l : location of the message
     \param msg : message to write
     \post l and msg have been written to this log
     */
    template <class LOCATION>
    void message(LOCATION const & l, std::string const & msg)
    {
      if (_os != nullptr) {
        *_os << l << ": ";
        message(msg);
      }
    }
    
    /*!
     \brief Write a message to the log
     \param msg : message to write
     \post msg have been written to this log
     */
    void message(std::string const & msg)
    {
      if (_os != nullptr)
        *_os << msg << std::endl;
    }
    
    std::ostream * _os;           /*!< Output stream */
    std::size_t _error_count;     /*!< Number of error messages */
    std::size_t _warning_count;   /*!< Number of warning messages */
  };
  
  
} // end of namespace tchecker

#endif // TCHECKER_LOG_HH
