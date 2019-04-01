/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_EXPLORE_OPTIONS_HH
#define TCHECKER_ALGORITHMS_EXPLORE_OPTIONS_HH

#include <getopt.h>
#include <iostream>
#include <string>

#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/log.hh"

/*!
 \file options.hh
 \brief Options for explore algorithm
 */

namespace tchecker {
  
  namespace explore {
    
    /*!
     \class options_t
     \brief Options for explore algorithm
     */
    class options_t {
    public:
      /*!
       \brief Type of output format
       */
      enum output_format_t {
        DOT,
        RAW,
      };
      
      /*!
       \brief Type of model to explore
       */
      enum explored_model_t {
        UNKNOWN,
        FSM,
        TA,
        ZG_ELAPSED_NOEXTRA,
        ZG_ELAPSED_EXTRAM_G,
        ZG_ELAPSED_EXTRAM_L,
        ZG_ELAPSED_EXTRAM_PLUS_G,
        ZG_ELAPSED_EXTRAM_PLUS_L,
        ZG_ELAPSED_EXTRALU_G,
        ZG_ELAPSED_EXTRALU_L,
        ZG_ELAPSED_EXTRALU_PLUS_G,
        ZG_ELAPSED_EXTRALU_PLUS_L,
        ZG_NON_ELAPSED_NOEXTRA,
        ZG_NON_ELAPSED_EXTRAM_G,
        ZG_NON_ELAPSED_EXTRAM_L,
        ZG_NON_ELAPSED_EXTRAM_PLUS_G,
        ZG_NON_ELAPSED_EXTRAM_PLUS_L,
        ZG_NON_ELAPSED_EXTRALU_G,
        ZG_NON_ELAPSED_EXTRALU_L,
        ZG_NON_ELAPSED_EXTRALU_PLUS_G,
        ZG_NON_ELAPSED_EXTRALU_PLUS_L,
        ASYNC_ZG_ELAPSED_EXTRALU_PLUS_L,
        ASYNC_ZG_NON_ELAPSED_EXTRALU_PLUS_L,
      };
      
      /*!
       \brief Type of search order
       */
      enum search_order_t {
        BFS,
        DFS,
      };
      
      /*!
       \brief Constructor
       \tparam MAP_ITERATOR : iterator on a map std::string -> std::string,
       should dereference to a pair of std::string (key, value)
       \param range : range (begin, end) of map iterators
       \param log : logging facility
       \post this has been built from range, and all errors/warnings have been
       reported to log
       */
      template <class MAP_ITERATOR>
      options_t(tchecker::range_t<MAP_ITERATOR> const & range, tchecker::log_t & log)
      : _output_format(tchecker::explore::options_t::RAW),
      _explored_model(tchecker::explore::options_t::UNKNOWN),
      _os(&std::cout),
      _search_order(tchecker::explore::options_t::DFS),
      _block_size(10000)
      {
        auto it = range.begin(), end = range.end();
        for ( ; it != end; ++it )
          set_option(it->first, it->second, log);
        check_mandatory_options(log);
      }
      
      /*!
       \brief Copy constructor (deleted)
       */
      options_t(tchecker::explore::options_t const & options) = delete;
      
      /*!
       \brief Move constructor
       */
      options_t(tchecker::explore::options_t && options);
      
      /*!
       \brief Destructor
       */
      ~options_t();
      
      /*!
       \brief Assignment operator (deleted)
       */
      tchecker::explore::options_t & operator= (tchecker::explore::options_t const &) = delete;
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::explore::options_t & operator= (tchecker::explore::options_t &&);
      
      /*!
       \brief Accessor
       \return output format
       */
      enum tchecker::explore::options_t::output_format_t output_format() const;
      
      /*!
       \brief Accessor
       \return explored model
       */
      enum tchecker::explore::options_t::explored_model_t explored_model() const;
      
      /*!
       \brief Accessor
       \return output stream
       */
      std::ostream & output_stream() const;
      
      /*!
       \brief Accessor
       \return search order
       */
      enum tchecker::explore::options_t::search_order_t search_order() const;
      
      /*!
       \brief Accessor
       \return allocation block size
       */
      std::size_t block_size() const;
      
      /*!
       \brief Check that mandatory options have been set
       \param log : a logging facility
       \post All errors and warnings have been reported to log
       */
      void check_mandatory_options(tchecker::log_t & log) const;
      
      /*!
       \brief Short options string (getopt_long format)
       */
      static constexpr char const * const getopt_long_options = "f:hm:o:s:";
      
      /*!
       \brief Long options (getopt_long format)
       */
      static constexpr struct option const getopt_long_options_long[]
      = {
        {"format",       required_argument, 0, 'f'},
        {"help",         no_argument,       0, 'h'},
        {"model",        required_argument, 0, 'm'},
        {"output",       required_argument, 0, 'o'},
        {"search-order", required_argument, 0, 's'},
        {"block-size",   required_argument, 0, 0},
        {0, 0, 0, 0}
      };
      
      /*!
       \brief Options description
       \param os : output stream
       \post A description of options for explore algorithm has been output to os
       \return os after output
       */
      static std::ostream & describe(std::ostream & os);
    protected:
      /*!
       \brief Set option
       \param key : option key
       \param value : option value
       \param log : logging facility
       \post option key has been set to value if key is a known option with expected value.
       An error has been reported to log if value is not admissible.
       A warning has been reported to log if key is not an option name.
       */
      void set_option(std::string const & key, std::string const & value, tchecker::log_t & log);
      
      /*!
       \brief Set output format
       \param value : option value
       \param log : logging facility
       \post output format has been set to value.
       An error has been reported to log if value is not admissible.
       */
      void set_output_format(std::string const & value, tchecker::log_t & log);
      
      /*!
       \brief Set explored model
       \param value : option value
       \param log : logging facility
       \post explored model has been set to value.
       An error has been reported to log if value is not admissible.
       */
      void set_explored_model(std::string const & value, tchecker::log_t & log);
      
      /*!
       \brief Set explored model for zone graphs
       \param semantics : a semantics
       \param extrapolation : an extrapolation
       \param log : logging facility
       \post explored model has been set to zone graph with semantics and
       extrapolation/
       An error has been reported to log if semantics or extrapolation is not
       admissible
       */
      void set_explored_model_zg(std::string const & semantics, std::string const & extrapolation, tchecker::log_t & log);
      
      /*!
       \brief Set explored model for asynchronous zone graphs
       \param semantics : a semantics
       \param log : logging facility
       \post explored model has been set to asynchrononous zone graph with semantics
       An error has been reported to log if semantics is not admissible
       */
      void set_explored_model_async_zg(std::string const & semantics, tchecker::log_t & log);
      
      /*!
       \brief Set output file
       \param filename : a file name
       \param log : logging facility
       \post output file has been set to filename
       An error has been reported to log if filename cannot be opened
       */
      void set_output_file(std::string const & filename, tchecker::log_t & log);
      
      /*!
       \brief Set search order
       \param value : option value
       \param log : logging facility
       \post search order has been set to value.
       An error has been reported to log if value is not admissible.
       */
      void set_search_order(std::string const & value, tchecker::log_t & log);
      
      /*!
       \brief Set block size
       \param value : option value
       \param log : logging facility
       \post block size has been set to value.
       An error has been reported to log if value is not admissible.
       */
      void set_block_size(std::string const & value, tchecker::log_t & log);
      
      enum output_format_t _output_format;    /*!< Output format */
      enum explored_model_t _explored_model;  /*!< Explored model */
      std::ostream * _os;                     /*!< Output stream */
      enum search_order_t _search_order;      /*!< Search order */
      std::size_t _block_size;                /*!< Size of allocation blocks */
    };
    
  } // end of namespace explore
  
} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_EXPLORE_OPTIONS_HH
