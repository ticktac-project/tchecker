/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COVREACH_OPTIONS_HH
#define TCHECKER_ALGORITHMS_COVREACH_OPTIONS_HH

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/log.hh"

/*!
 \file options.hh
 \brief Options for covering reachability algorithm
 */

namespace tchecker {
  
  namespace covreach {
    
    /*!
     \class options_t
     \brief Options for covering reachability algorithm
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
       \brief Type of model
       */
      enum algorithm_model_t {
        UNKNOWN,
        ASYNC_ZG_ELAPSED_EXTRALU_PLUS_L,
        ASYNC_ZG_NON_ELAPSED_EXTRALU_PLUS_L,
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
      };
      
      /*!
       \brief Type of search order
       */
      enum search_order_t {
        BFS,
        DFS,
      };
      
      /*!
       \brief Type of node covering
       */
      enum node_covering_t {
        INCLUSION,
        ALU_G,
        ALU_L,
        AM_G,
        AM_L,
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
      : _node_covering(INCLUSION),
      _output_format(tchecker::covreach::options_t::RAW),
      _algorithm_model(tchecker::covreach::options_t::UNKNOWN),
      _os(&std::cout),
      _search_order(tchecker::covreach::options_t::DFS),
      _block_size(10000),
      _nodes_table_size(65536),
      _stats(0)
      {
        auto it = range.begin(), end = range.end();
        for ( ; it != end; ++it )
          set_option(it->first, it->second, log);
        check_mandatory_options(log);
      }
      
      /*!
       \brief Copy constructor (deleted)
       */
      options_t(tchecker::covreach::options_t const & options) = delete;
      
      /*!
       \brief Move constructor
       */
      options_t(tchecker::covreach::options_t && options);
      
      /*!
       \brief Destructor
       */
      ~options_t();
      
      /*!
       \brief Assignment operator (deleted)
       */
      tchecker::covreach::options_t & operator= (tchecker::covreach::options_t const &) = delete;
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::covreach::options_t & operator= (tchecker::covreach::options_t &&);
      
      /*!
       \brief Accessor
       \return node covering
       */
      enum tchecker::covreach::options_t::node_covering_t node_covering() const;
      
      /*!
       \brief Accessor
       \return output format
       */
      enum tchecker::covreach::options_t::output_format_t output_format() const;
      
      /*!
       \bief Type of range of accepting labels
       */
      using accepting_labels_range_t = tchecker::range_t<std::vector<std::string>::const_iterator>;
      
      /*!
       \brief Accessor
       \return accepting labels
       */
      tchecker::covreach::options_t::accepting_labels_range_t accepting_labels() const;
      
      /*!
       \brief Accessor
       \return algorithm model
       */
      enum tchecker::covreach::options_t::algorithm_model_t algorithm_model() const;
      
      /*!
       \brief Accessor
       \return output stream
       */
      std::ostream & output_stream() const;
      
      /*!
       \brief Accessor
       \return search order
       */
      enum tchecker::covreach::options_t::search_order_t search_order() const;
      
      /*!
       \brief Accessor
       \return allocation block size
       */
      std::size_t block_size() const;
      
      /*!
       \brief Accessor
       \return nodes table size
       */
      std::size_t nodes_table_size() const;
      
      /*!
       \brief Accessor
       \return true if stats should be output, false otherwise
       */
      bool stats() const;
      
      /*!
       \brief Check that mandatory options have been set
       \param log : a logging facility
       \post All errors and warnings have been reported to log
       */
      void check_mandatory_options(tchecker::log_t & log) const;
      
      /*!
       \brief Short options string (getopt_long format)
       */
      static constexpr char const * const getopt_long_options = "c:f:hl:m:o:s:S";
      
      /*!
       \brief Long options (getopt_long format)
       */
      static constexpr struct option const getopt_long_options_long[]
      = {
        {"cover",        required_argument, 0, 'c'},
        {"format",       required_argument, 0, 'f'},
        {"help",         no_argument,       0, 'h'},
        {"labels",       required_argument, 0, 'l'},
        {"model",        required_argument, 0, 'm'},
        {"output",       required_argument, 0, 'o'},
        {"search-order", required_argument, 0, 's'},
        {"stats",        no_argument,       0, 'S'},
        {"block-size",   required_argument, 0, 0},
        {"table-size",   required_argument, 0, 0},
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
       \brief Set node covering
       \param value : option value
       \param log : logging facility
       \post node covering has been set to value.
       An error has been reported to log if value is not admissible.
       */
      void set_node_covering(std::string const & value, tchecker::log_t & log);
      
      /*!
       \brief Set output format
       \param value : option value
       \param log : logging facility
       \post output format has been set to value.
       An error has been reported to log if value is not admissible.
       */
      void set_output_format(std::string const & value, tchecker::log_t & log);
      
      /*!
       \brief Set accepting labels
       \param value : option value
       \param log : logging facility
       \post accepting labels have been set to value.
       An error has been reported to log if value is not admissible.
       */
      void set_accepting_labels(std::string const & value, tchecker::log_t & log);
      
      /*!
       \brief Set algorithm model
       \param value : option value
       \param log : logging facility
       \post algorithm model has been set to value.
       An error has been reported to log if value is not admissible.
       */
      void set_algorithm_model(std::string const & value, tchecker::log_t & log);
      
      /*!
       \brief Set algorithm model for asynchronous zone graphs
       \param semantics : a semantics
       \param extrapolation : an extrapolation
       \param log : logging facility
       \post algorithm model has been set to asynchronous zone graph with semantics and
       extrapolation
       An error has been reported to log if semantics or extrapolation is not
       admissible
       */
      void set_algorithm_model_async_zg(std::string const & semantics, std::string const & extrapolation, tchecker::log_t & log);
      
      /*!
       \brief Set algorithm model for zone graphs
       \param semantics : a semantics
       \param extrapolation : an extrapolation
       \param log : logging facility
       \post algorithm model has been set to zone graph with semantics and
       extrapolation
       An error has been reported to log if semantics or extrapolation is not
       admissible
       */
      void set_algorithm_model_zg(std::string const & semantics, std::string const & extrapolation, tchecker::log_t & log);
      
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
      
      /*!
       \brief Set nodes table size
       \param value : option value
       \param log : logging facility
       \post nodes table size has been set to value.
       An error has been reported to log if value is not admissible.
       */
      void set_nodes_table_size(std::string const & value, tchecker::log_t & log);
      
      /*!
       \brief Set stats flag
       \param value : option value
       \param log : logging facility
       \post stats flag has been set
       */
      void set_stats(std::string const & value, tchecker::log_t & log);
      
      enum node_covering_t _node_covering;         /*!< Node covering */
      enum output_format_t _output_format;         /*!< Output format */
      std::vector<std::string> _accepting_labels;  /*!< Accepting labels */
      enum algorithm_model_t _algorithm_model;     /*!< Algorithm model */
      std::ostream * _os;                          /*!< Output stream */
      enum search_order_t _search_order;           /*!< Search order */
      std::size_t _block_size;                     /*!< Size of allocation blocks */
      std::size_t _nodes_table_size;               /*!< Size of nodes table */
      unsigned _stats : 1;                         /*!< Statistics */
    };
    
  } // end of namespace covreach
  
} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COVREACH_OPTIONS_HH
