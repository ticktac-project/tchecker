/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <fstream>
#include <iterator>

#include <boost/tokenizer.hpp>

#include "tchecker/algorithms/explore/options.hh"

namespace tchecker {
  
  namespace explore {
    
    options_t::options_t(tchecker::explore::options_t && options)
    : _output_format(std::move(options._output_format)),
    _explored_model(std::move(options._explored_model)),
    _os(options._os),
    _search_order(std::move(options._search_order)),
    _block_size(std::move(options._block_size))
    {
      options._os = nullptr;
    }
    
    
    options_t::~options_t()
    {
      _os->flush();
      if (_os != &std::cout)
        delete _os;
    }
    
    
    tchecker::explore::options_t & options_t::operator= (tchecker::explore::options_t && options)
    {
      if (this != &options) {
        _output_format = std::move(options._output_format);
        _explored_model = std::move(options._explored_model);
        _os = options._os;
        options._os = nullptr;
        _search_order = std::move(options._search_order);
        _block_size = options._block_size;
      }
      return *this;
    }
    
    
    enum tchecker::explore::options_t::output_format_t options_t::output_format() const
    {
      return _output_format;
    }
    
    
    enum tchecker::explore::options_t::explored_model_t options_t::explored_model() const
    {
      return _explored_model;
    }
    
    
    std::ostream & options_t::output_stream() const
    {
      return *_os;
    }
    
    
    enum tchecker::explore::options_t::search_order_t options_t::search_order() const
    {
      return _search_order;
    }
    
    
    std::size_t options_t::block_size() const
    {
      return _block_size;
    }
    
    
    void options_t::set_option(std::string const & key, std::string const & value, tchecker::log_t & log)
    {
      if (key == "f")
        set_output_format(value, log);
      else if (key == "h") {
      }
      else if (key == "m")
        set_explored_model(value, log);
      else if (key == "o")
        set_output_file(value, log);
      else if (key == "s")
        set_search_order(value, log);
      else if (key == "block-size")
        set_block_size(value, log);
      else
        log.warning("Unknown command line option " + key);
    }
    
    
    void options_t::set_output_format(std::string const & value, tchecker::log_t & log)
    {
      if (value == "dot")
        _output_format = tchecker::explore::options_t::DOT;
      else if (value == "raw")
        _output_format = tchecker::explore::options_t::RAW;
      else
        log.error("Unkown output format: " + value + " for commande line parameter -f");
    }
    
    
    void options_t::set_explored_model(std::string const & value, tchecker::log_t & log)
    {
      if (value == "fsm")
        _explored_model = tchecker::explore::options_t::FSM;
      else if (value == "ta")
        _explored_model = tchecker::explore::options_t::TA;
      else {
        boost::char_separator<char> sep(":");
        boost::tokenizer<boost::char_separator<char> > tokenizer(value, sep);
        auto begin = tokenizer.begin(), end = tokenizer.end();
        
        if (*begin == "zg") {
          if (std::distance(begin, end) != 3) {
            log.error("Unknown model: " + value + " for command line parameter -m");
            return;
          }
          
          std::string graph = *(begin++);
          std::string semantics = *(begin++);
          std::string extrapolation = *(begin++);
          
          set_explored_model_zg(semantics, extrapolation, log);
        }
        else if (*begin == "async_zg") {
          if (std::distance(begin, end) != 2) {
            log.error("unknown model: " + value + " for command line parameter -m");
            return;
          }
          
          std::string graph = *(begin++);
          std::string semantics = *(begin++);
          
          set_explored_model_async_zg(semantics, log);
        }
        else
          log.error("Unknown model: " + value + " for command line parameter -m");
      }
    }
    
    
    void options_t::set_explored_model_zg(std::string const & semantics, std::string const & extrapolation, tchecker::log_t & log)
    {
      if (semantics == "elapsed") {
        if (extrapolation == "NOextra")
          _explored_model = tchecker::explore::options_t::ZG_ELAPSED_NOEXTRA;
        else if (extrapolation == "extraMg")
          _explored_model = tchecker::explore::options_t::ZG_ELAPSED_EXTRAM_G;
        else if (extrapolation == "extraMl")
          _explored_model = tchecker::explore::options_t::ZG_ELAPSED_EXTRAM_L;
        else if (extrapolation == "extraM+g")
          _explored_model = tchecker::explore::options_t::ZG_ELAPSED_EXTRAM_PLUS_G;
        else if (extrapolation == "extraM+l")
          _explored_model = tchecker::explore::options_t::ZG_ELAPSED_EXTRAM_PLUS_L;
        else if (extrapolation == "extraLUg")
          _explored_model = tchecker::explore::options_t::ZG_ELAPSED_EXTRALU_G;
        else if (extrapolation == "extraLUl")
          _explored_model = tchecker::explore::options_t::ZG_ELAPSED_EXTRALU_L;
        else if (extrapolation == "extraLU+g")
          _explored_model = tchecker::explore::options_t::ZG_ELAPSED_EXTRALU_PLUS_G;
        else if (extrapolation == "extraLU+l")
          _explored_model = tchecker::explore::options_t::ZG_ELAPSED_EXTRALU_PLUS_L;
        else
          log.error("Unknown extrapolation: " + extrapolation + " for command line parameter -m");
      }
      else if (semantics == "non-elapsed") {
        if (extrapolation == "NOextra")
          _explored_model = tchecker::explore::options_t::ZG_NON_ELAPSED_NOEXTRA;
        else if (extrapolation == "extraMg")
          _explored_model = tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRAM_G;
        else if (extrapolation == "extraMl")
          _explored_model = tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRAM_L;
        else if (extrapolation == "extraM+g")
          _explored_model = tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRAM_PLUS_G;
        else if (extrapolation == "extraM+l")
          _explored_model = tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRAM_PLUS_L;
        else if (extrapolation == "extraLUg")
          _explored_model = tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRALU_G;
        else if (extrapolation == "extraLUl")
          _explored_model = tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRALU_L;
        else if (extrapolation == "extraLU+g")
          _explored_model = tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRALU_PLUS_G;
        else if (extrapolation == "extraLU+l")
          _explored_model = tchecker::explore::options_t::ZG_NON_ELAPSED_EXTRALU_PLUS_L;
        else
          log.error("Unknown extrapolation: " + extrapolation + " for command line parameter -m");
      }
      else
        log.error("Unknown semantics: " + semantics + " for command line parameter -m");
    }
    
    
    void options_t::set_explored_model_async_zg(std::string const & semantics, tchecker::log_t & log)
    {
      if (semantics == "elapsed")
        _explored_model = tchecker::explore::options_t::ASYNC_ZG_ELAPSED_EXTRALU_PLUS_L;
      else if (semantics == "non-elapsed")
        _explored_model = tchecker::explore::options_t::ASYNC_ZG_NON_ELAPSED_EXTRALU_PLUS_L;
      else
        log.error("Unknown semantics: " + semantics + " for command line parameter -m");
    }
    
    
    void options_t::set_output_file(std::string const & filename, tchecker::log_t & log)
    {
      if (_os != &std::cout)
        delete _os;
      _os = new std::ofstream(filename, std::ios::out);
      if (_os->fail()) {
        log.error("Unable to open file: " + filename + " for command line parameter -o");
        return;
      }
    }
    
    
    void options_t::set_search_order(std::string const & value, tchecker::log_t & log)
    {
      if (value == "bfs")
        _search_order = tchecker::explore::options_t::BFS;
      else if (value == "dfs")
        _search_order = tchecker::explore::options_t::DFS;
      else
        log.error("Unknown search order: " + value + " for command line option -s");
    }
    
    
    void options_t::set_block_size(std::string const & value, tchecker::log_t & log)
    {
      for (auto c : value)
        if (! isdigit(c)) {
          log.error("Invalid value: " + value + " for command line option --block-size, expecting an unsigned integer");
          return;
        }
      
      _block_size = std::stoul(value);
    }
    
    
    void options_t::check_mandatory_options(tchecker::log_t & log) const
    {
      if (_explored_model == UNKNOWN)
        log.error("explored model must be set, use -m command line option");
    }
    
    
    std::ostream & options_t::describe(std::ostream & os)
    {
      os << "-f (dot|raw)     output format (graphviz DOT format or raw format)" << std::endl;
      os << "-h               this help screen" << std::endl;
      os << "-m model         where model is one of the following:" << std::endl;
      os << "                 fsm                          finite-state machine" << std::endl;
      os << "                 ta                           timed automaton" << std::endl;
      os << "                 zg:semantics:extrapolation   zone graph with:" << std::endl;
      os << "                   semantics:      elapsed        time-elapsed semantics" << std::endl;
      os << "                                   non-elapsed    non time-elapsed semantics" << std::endl;
      os << "                   extrapolation:  NOextra   no zone extrapolation" << std::endl;
      os << "                                   extraMg   ExtraM with global clock bounds" << std::endl;
      os << "                                   extraMl   ExtraM with local clock bounds" << std::endl;
      os << "                                   extraM+g  ExtraM+ with global clock bounds" << std::endl;
      os << "                                   extraM+l  ExtraM+ with local clock bounds" << std::endl;
      os << "                                   extraLUg  ExtraLU with global clock bounds" << std::endl;
      os << "                                   extraLUl  ExtraLU with local clock bounds" << std::endl;
      os << "                                   extraLU+g ExtraLU+ with global clock bounds" << std::endl;
      os << "                                   extraLU+l ExtraLU+ with local clock bounds" << std::endl;
      os << "                 async_zg:semantics            asynchronous zone graph with:" << std::endl;
      os << "                   semantics:      elapsed         time-elapsed semantics"    << std::endl;
      os << "                                   non-elapsed     non time-elapsed semantics" << std::endl;
      os << "-o filename      output graph to filename" << std::endl;
      os << "-s (bfs|dfs)     search order (breadth-first search or depth-first search)" << std::endl;
      os << "--block-size n   size of an allocation block (number of allocated objects)" << std::endl;
      os << std::endl;
      os << "Default parameters: -f raw -s dfs --block-size 10000, output to standard output" << std::endl;
      os << "                    -m must be specified" << std::endl;
      return os;
    }
    
  } // end of namespace explore
  
} // end of namespace tchecker
