/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ASYNC_ZG_DETAILS_MODEL_HH
#define TCHECKER_ASYNC_ZG_DETAILS_MODEL_HH

#include <cstring>
#include <limits>
#include <string>
#include <unordered_set>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/model.hh"
#include "tchecker/expression/static_analysis.hh"
#include "tchecker/statement/static_analysis.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file model.hh
 \brief Model for asynchronous zone graph (details)
 */

namespace tchecker {
  
  namespace async_zg {
    
    namespace details {
      
      /*!
       \class model_t
       \brief Model for asynchronous zone graph
       \tparam SYSTEM : type of system, see tchecker::ta::details::model_t
       \tparam VARIABLES : type of model variables, should inherit from tchecker::async_zg::details::variables_t
       \note see tchecker::ta::details::model_t for why instances cannot be constructed.
       */
      template <class SYSTEM, class VARIABLES>
      class model_t
      : public tchecker::clockbounds::model_t<SYSTEM, VARIABLES> {
      public:
        /*!
         \brief Copy constructor
         */
        model_t(tchecker::async_zg::details::model_t<SYSTEM, VARIABLES> const & model)
        : tchecker::clockbounds::model_t<SYSTEM, VARIABLES>(model),
        _offset_dimension(model._offset_dimension),
        _refcount(model._refcount),
        _refmap(nullptr),
        _offset_clock_index(model._clock_index)
        {
          _refmap = new tchecker::clock_id_t[_offset_dimension];
          std::memcpy(_refmap, model._refmap, _offset_dimension * sizeof(*_refmap));
        }
        
        /*!
         \brief Move constructor
         */
        model_t(tchecker::async_zg::details::model_t<SYSTEM, VARIABLES> && model)
        : tchecker::clockbounds::model_t<SYSTEM, VARIABLES>(std::move(model)),
        _offset_dimension(model._offset_dimension),
        _refcount(model._refcount),
        _refmap(model._refmap),
        _offset_clock_index(std::move(model._offset_clock_index))
        {
          model._refmap = nullptr;
        }
        
        /*!
         \brief Destructor
         */
        virtual ~model_t()
        {
          delete[] _refmap;
        }
        
        /*!
         \brief Assignement operator
         \param model : a model
         \post this is a copy of model
         \return this after assignment
         */
        tchecker::async_zg::details::model_t<SYSTEM, VARIABLES> &
        operator= (tchecker::async_zg::details::model_t<SYSTEM, VARIABLES> const & model)
        {
          if (this != &model) {
            tchecker::clockbounds::model_t<SYSTEM, VARIABLES>::operator=(model);
            _offset_dimension = model._dimension;
            _refcount = model._refcount;
            delete[] _refmap;
            _refmap = new tchecker::clock_id_t[_offset_dimension];
            std::memcpy(_refmap, model._refmap, _offset_dimension * sizeof(*_refmap));
            _offset_clock_index = model._offset_clock_index;
          }
          return *this;
        }
        
        /*!
         \brief Move-assignment operator
         \param model : a model
         \post model has been moved to this
         \return this after assignment
         */
        tchecker::async_zg::details::model_t<SYSTEM, VARIABLES> &
        operator= (tchecker::async_zg::details::model_t<SYSTEM, VARIABLES> && model)
        {
          if (this != &model) {
            tchecker::clockbounds::model_t<SYSTEM, VARIABLES>::operator=(std::move(model));
            _offset_dimension = model._dimension;
            _refcount = model._refcount;
            _refmap = model._refmap;
            model._refmap = nullptr;
            _offset_clock_index = std::move(model._offset_clock_index);
          }
          return *this;
        }
        
        /*!
         \brief Accessor
         \return offset dimension (number of offset clocks including reference clocks)
         */
        inline tchecker::clock_id_t offset_dimension() const
        {
          return _offset_dimension;
        }
        
        /*!
         \brief Accessor
         \return number of reference clocks
         */
        inline tchecker::clock_id_t refcount() const
        {
          return _refcount;
        }
        
        /*!
         \brief Accessor
         \return map that associates a reference clock to every offset clocks,
         to every reference clock and to every process ID.
         The domain of reference map is: 0...refcount-1 (for reference clocks)
         then refcount...dimension-1 (for offset clocks)
         \note Reference clocks are mapped to themselves.
         Process IDs are mapped to their reference clock.
         \note the returned pointer should not be deleted
         */
        inline tchecker::clock_id_t const * refmap() const
        {
          return _refmap;
        }
        
        /*!
         \brief Accessor
         \return index of offset clocks (including reference clocks)
         */
        inline tchecker::clock_index_t const & offset_clock_index() const
        {
          return _offset_clock_index;
        }
      protected:
        /*!
         \brief Constructor
         \param system : a system
         \param log : logging facility
         \note see tchecker::ta::details::model_t
         \throw std::invalid_argument : if system has shared clocks
         \throw std::invalid_argument : if the number of reference/offset variables exceeds the maximum
         value that can be represented by type tchecker::clock_id_t
         */
        model_t(SYSTEM * system, tchecker::log_t & log)
        : tchecker::clockbounds::model_t<SYSTEM, VARIABLES>(system, log)
        {
          // clock number excluding the zero clock
          tchecker::clock_id_t clock_nb
          = tchecker::clockbounds::model_t<SYSTEM, VARIABLES>::flattened_clock_variables().size() - 1;
          tchecker::process_id_t proc_nb = system->processes_count();
          
          if (proc_nb > std::numeric_limits<tchecker::clock_id_t>::max())
            throw std::invalid_argument("too many processes");
          
          unsigned long long offset_dim = proc_nb + clock_nb;
          if (offset_dim > std::numeric_limits<tchecker::clock_id_t>::max())
            throw std::invalid_argument("too many offset variables");
          
          _offset_dimension = proc_nb + clock_nb;
          _refcount = proc_nb;
          _refmap = new tchecker::clock_id_t[_offset_dimension];
          try {
            compute_refmap(system);
          }
          catch (...) {
            delete[] _refmap;
            throw;
          }
          
          tchecker::clock_id_t id = 0;
          for (id = 0; id < _refcount; ++id)
            _offset_clock_index.add(id, "$" + std::to_string(id));
          tchecker::clock_index_t const & clock_index
          = tchecker::clockbounds::model_t<SYSTEM, VARIABLES>::flattened_clock_variables().index();
          auto it = clock_index.begin(), end = clock_index.end();
          for (++it; it != end; ++it, ++id)
            _offset_clock_index.add(id, "$" + clock_index.value(*it));
        }
        
        /*!
         \brief Computer refmap
         \param system : a system
         \pre _refmap is an allocated of size _dimension
         _refcount is the number of reference clocks in system
         system has no shared clock
         \post _refmap maps every reference clock to itself, every offset clock to the
         reference clock of its process, and every process ID to its reference clock
         \throw std::invalid_argument : if system has a shared clock
         \throw std::invalid_argument : if the reference clock cannot be determined for some clock in system (ex: clock that
         does not appear in any guard/statement/invairnat)
         */
        void compute_refmap(SYSTEM * system)
        {
          // Initialisation
          tchecker::clock_id_t const NOREF = _refcount;
          for (tchecker::clock_id_t i = 0; i < _refcount; ++i)
            _refmap[i] = i;
          for (tchecker::clock_id_t i = _refcount; i < _offset_dimension; ++i)
            _refmap[i] = NOREF;
          
          // Compute _refmap from locations and edges
          for (auto const * loc : system->locations())
            compute_refmap_from_expression(this->typed_invariant(loc->id()), loc->pid(), NOREF);
          
          for (auto const * edge : system->edges()) {
            compute_refmap_from_expression(this->typed_guard(edge->id()), edge->pid(), NOREF);
            compute_refmap_from_statement(this->typed_statement(edge->id()), edge->pid(), NOREF);
          }
          
          // Check that every clock has a reference
          for (tchecker::clock_id_t i = _refcount; i < _offset_dimension; ++i)
            if (_refmap[i] == NOREF)
              throw std::invalid_argument("Reference clocks cannot be computed");
        }
        
        /*!
         \brief Compute refmap from expression
         \param expr : expression
         \param pid : process identifier
         \param NOREF : special value
         \pre for every clock x in stmt, _refmap associates NOREF or pid to x
         \post every clock occurring in expr has reference clock of process pid in _refmap.
         \throw std::invalid_argument : if a clock in expr already has a reference clock
         */
        void compute_refmap_from_expression(tchecker::typed_expression_t const & expr,
                                            tchecker::process_id_t pid,
                                            tchecker::clock_id_t NOREF)
        {
          std::unordered_set<tchecker::clock_id_t> clocks;
          std::unordered_set<tchecker::intvar_id_t> intvars;
          tchecker::extract_variables(expr, clocks, intvars);
          for (tchecker::clock_id_t id : clocks) {
            if (refmap(id) == NOREF)
              refmap(id) = pid;
            else if (refmap(id) != pid)
              throw std::invalid_argument("asynchronous zone graph does not handle shared clocks");
          }
        }
        
        /*!
         \brief Compute refmap from statement
         \param stmt : statement
         \param pid : process identifier
         \param NOREF : special value
         \pre for every clock x in stmt, _refmap associates NOREF or pid to x
         \post every clock occurring in stmt has reference clock of process pid in _refmap.
         \throw std::invalid_argument : if a clock in stmt already has a reference clock (i.e. _refmap
         associates a value different from NOREF to x)
         */
        void compute_refmap_from_statement(tchecker::typed_statement_t const & stmt,
                                           tchecker::process_id_t pid,
                                           tchecker::clock_id_t NOREF)
        {
          std::unordered_set<tchecker::clock_id_t> clocks;
          std::unordered_set<tchecker::intvar_id_t> intvars;
          tchecker::extract_read_variables(stmt, clocks, intvars);
          tchecker::extract_written_variables(stmt, clocks, intvars);
          for (tchecker::clock_id_t id : clocks) {
            if (refmap(id) == NOREF)
              refmap(id) = pid;
            else if (refmap(id) != pid)
              throw std::invalid_argument("asynchronous zone graph does not handle shared clocks");
          }
        }
        
        /*!
         \brief Accessor
         \param id : automaton clock id (0 is zero clock)
         \return reference to refmap entry for id
         */
        constexpr tchecker::clock_id_t & refmap(tchecker::clock_id_t id)
        {
          return _refmap[_refcount + id - 1];
        }
        
        tchecker::clock_id_t _offset_dimension;       /*!< Offset dimension (number of offset clocks) */
        tchecker::clock_id_t _refcount;               /*!< Number of reference clocks */
        tchecker::clock_id_t * _refmap;               /*!< Map : offset clocks or process ID -> reference clocks */
        tchecker::clock_index_t _offset_clock_index;  /*!< Offset clocks index */
      };
      
    } // end of namespace details
    
  } // end of namespace async_zg
  
} // end of namespace tchecker

#endif // TCHECKER_ASYNC_ZG_DETAILS_MODEL_HH



