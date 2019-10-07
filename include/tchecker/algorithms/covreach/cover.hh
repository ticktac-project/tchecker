/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COVREACH_COVER_HH
#define TCHECKER_ALGORITHMS_COVREACH_COVER_HH

#include <cassert>
#include <tuple>
#include <type_traits>

#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/clockbounds/model.hh"
#include "tchecker/clockbounds/vlocbounds.hh"
#include "tchecker/zone/zone.hh"

/*!
 \file cover.hh
 \brief Node predicates for covering reachability algorithm
 */

namespace tchecker {
  
  namespace covreach {
    
    namespace details {
      
      /*!
       \brief Validity predicate
       \tparam NODE_PTR : type of pointer to node
       \param n1 : a node
       \param n2 : a node
       \return true if checking covering n1 by n2 is valid, false otherwise
       */
      template <class NODE_PTR>
      inline bool valid_cover(NODE_PTR const & n1, NODE_PTR const & n2)
      {
        assert(n2->is_active());
        assert(n1->is_active());
        return ((n1 != n2) && !n1->is_protected());
      }
      
      
      
      
      /*!
       \class cover_zone_inclusion_t
       \brief node covering w.r.t. zone inclusion
       \tparam NODE_PTR : type of pointer to node, *NODE_PTR should derive from tchecker::zg::details::state_t
       */
      template <class NODE_PTR>
      class cover_zone_inclusion_t {
      public:
        /*!
         \brief Type of pointer to node
         */
        using node_ptr_t = NODE_PTR;
        
        /*!
         \brief Constructor
         \param model : clock bounds model
         \tparam MODEL : type of model
         */
        template <class MODEL>
        explicit cover_zone_inclusion_t(MODEL const & model)
        {}
        
        /*!
         \brief Copy constructor
         */
        cover_zone_inclusion_t(tchecker::covreach::details::cover_zone_inclusion_t<NODE_PTR> const &) = default;
        
        /*!
         \brief Move constructor
         */
        cover_zone_inclusion_t(tchecker::covreach::details::cover_zone_inclusion_t<NODE_PTR> &&) = default;
        
        /*!
         \brief Destructor
         */
        ~cover_zone_inclusion_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::covreach::details::cover_zone_inclusion_t<NODE_PTR> &
        operator= (tchecker::covreach::details::cover_zone_inclusion_t<NODE_PTR> const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::covreach::details::cover_zone_inclusion_t<NODE_PTR> &
        operator= (tchecker::covreach::details::cover_zone_inclusion_t<NODE_PTR>  &&) = default;
        
        /*!
         \brief Covering predicate
         \param n1 : a node
         \param n2 : a node
         \return true if the zone in n1 is included in the zone in n2, false otherwise
         */
        bool operator() (NODE_PTR const & n1, NODE_PTR const & n2)
        {
          return (n1->zone() <= n2->zone());
        }
      };
      
      
      
      
      /*!
       \class cover_zone_alu_global_t
       \brief node covering w.r.t. zone aLU-inclusion and global clock bounds
       \tparam NODE_PTR : type of pointer to node, *NODE_PTR should derive from tchecker::zg::details::state_t
       */
      template <class NODE_PTR>
      class cover_zone_alu_global_t {
      public:
        /*!
         \brief Type of pointer to node
         */
        using node_ptr_t = NODE_PTR;
        
        /*!
         \brief Constructor
         \param model : clock bounds model
         \tparam MODEL : type of model, should inherit from tchecker::clockbounds::model_t
         \note this keeps a reference to the L and U maps in model.global_lu_map()
         */
        template <class MODEL>
        explicit cover_zone_alu_global_t(MODEL const & model)
        : _L(model.global_lu_map().L()), _U(model.global_lu_map().U())
        {}
        
        /*!
         \brief Copy constructor
         */
        cover_zone_alu_global_t(tchecker::covreach::details::cover_zone_alu_global_t<NODE_PTR> const & c) = default;
        
        /*!
         \brief Move constructor
         */
        cover_zone_alu_global_t(tchecker::covreach::details::cover_zone_alu_global_t<NODE_PTR> && c) = default;
        
        /*!
         \brief Destructor
         */
        ~cover_zone_alu_global_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::covreach::details::cover_zone_alu_global_t<NODE_PTR> &
        operator= (tchecker::covreach::details::cover_zone_alu_global_t<NODE_PTR> const & c) = default;
        
        /*!
         \brief Move assignment operator
         */
        tchecker::covreach::details::cover_zone_alu_global_t<NODE_PTR> &
        operator= (tchecker::covreach::details::cover_zone_alu_global_t<NODE_PTR> && c) = default;
        
        /*!
         \brief Node covering predicate
         \param n1 : a node
         \param n2 : a node
         \return true if the zone in n1 is included into the aLU abstraction of the zone in n2, false
         otherwise
         */
        bool operator() (NODE_PTR const & n1, NODE_PTR const & n2)
        {
          return n1->zone().alu_le(n2->zone(), _L.get(), _U.get());
        }
      private:
        std::reference_wrapper<tchecker::clockbounds::map_t const> _L;  /*!< global L clock bounds map */
        std::reference_wrapper<tchecker::clockbounds::map_t const> _U;  /*!< global U clock bounds map */
      };
      
      
      
      
      /*!
       \class cover_zone_alu_local_t
       \brief node covering w.r.t. zone aLU-inclusion and local clock bounds
       \tparam NODE_PTR : type of pointer to node, *NODE_PTR should derive from tchecker::zg::details::state_t
       */
      template <class NODE_PTR>
      class cover_zone_alu_local_t {
      public:
        /*!
         \brief Type of pointer to node
         */
        using node_ptr_t = NODE_PTR;
        
        /*!
         \brief Constructor
         \param model : clock bounds model
         \tparam MODEL : type of model, should inherit from tchecker::clockbounds::model_t
         \note this keeps a reference on model.local_lu_map()
         */
        template <class MODEL>
        explicit cover_zone_alu_local_t(MODEL const & model)
        : _local_lu_map(model.local_lu_map())
        {
          _L = tchecker::clockbounds::allocate_map(_local_lu_map.get().clock_number());
          _U = tchecker::clockbounds::allocate_map(_local_lu_map.get().clock_number());
        }
        
        /*!
         \brief Copy constructor
         */
        cover_zone_alu_local_t(tchecker::covreach::details::cover_zone_alu_local_t<NODE_PTR> const & c)
        : _local_lu_map(c._local_lu_map)
        {
          _L = tchecker::clockbounds::clone_map(*c._L);
          _U = tchecker::clockbounds::clone_map(*c._U);
        }
        
        /*!
         \brief Move constructor
         */
        cover_zone_alu_local_t(tchecker::covreach::details::cover_zone_alu_local_t<NODE_PTR> && c)
        : _local_lu_map(std::move(c._local_lu_map)), _L(c._L), _U(c._U)
        {
          c._L = nullptr;
          c._U = nullptr;
        }
        
        /*!
         \brief Destructor
         */
        ~cover_zone_alu_local_t()
        {
          tchecker::clockbounds::deallocate_map(_L);
          tchecker::clockbounds::deallocate_map(_U);
        }
        
        /*!
         \brief Assignment operator
         */
        tchecker::covreach::details::cover_zone_alu_local_t<NODE_PTR> &
        operator= (tchecker::covreach::details::cover_zone_alu_local_t<NODE_PTR> const & c)
        {
          if (this != &c) {
            _local_lu_map = c._local_lu_map;
            delete _L;
            _L = tchecker::clockbounds::clone_map(*c._L);
            delete _U;
            _U = tchecker::clockbounds::clone_map(*c._U);
          }
          return *this;
        }
        
        /*!
         \brief Move assignment operator
         */
        tchecker::covreach::details::cover_zone_alu_local_t<NODE_PTR> &
        operator= (tchecker::covreach::details::cover_zone_alu_local_t<NODE_PTR> && c)
        {
          if (this != &c) {
            _local_lu_map = std::move(c._local_lu_map);
            delete _L;
            _L = c._L;
            c._L = nullptr;
            delete _U;
            _U = c._U;
            c._U = nullptr;
          }
          return *this;
        }
        
        /*!
         \brief Node covering predicate
         \param n1 : a node
         \param n2 : a node
         \return true if the zone in n1 is included in the aLU abstraction of the zone in n2 w.r.t. local
         clock bounds in n2
         */
        bool operator() (NODE_PTR const & n1, NODE_PTR const & n2)
        {
          tchecker::clockbounds::vloc_bounds(_local_lu_map.get(), n2->vloc(), *_L, *_U);
          return n1->zone().alu_le(n2->zone(), *_L, *_U);
        }
      private:
        std::reference_wrapper<tchecker::clockbounds::local_lu_map_t const> _local_lu_map; /*!< Local LU clockbounds map */
        tchecker::clockbounds::map_t * _L;                                                 /*!< L clock bounds map */
        tchecker::clockbounds::map_t * _U;                                                 /*!< U clock bounds map */
      };
      
      
      
      
      /*!
       \class cover_zone_am_global_t
       \brief node covering w.r.t. zone aM-inclusion and global clock bounds
       \tparam NODE_PTR : type of pointer to node, *NODE_PTR should derive from tchecker::zg::details::state_t
       */
      template <class NODE_PTR>
      class cover_zone_am_global_t {
      public:
        /*!
         \brief Type of pointer to node
         */
        using node_ptr_t = NODE_PTR;
        
        /*!
         \brief Constructor
         \param model : clock bounds model
         \tparam MODEL : type of model, should inherit from tchecker::clockbounds::model_t
         \note this keeps a reference to the M map in model.global_m_map()
         */
        template <class MODEL>
        explicit cover_zone_am_global_t(MODEL const & model)
        : _M(model.global_m_map().M())
        {}
        
        /*!
         \brief Copy constructor
         */
        cover_zone_am_global_t(tchecker::covreach::details::cover_zone_am_global_t<NODE_PTR> const & c) = default;
        
        /*!
         \brief Move constructor
         */
        cover_zone_am_global_t(tchecker::covreach::details::cover_zone_am_global_t<NODE_PTR> && c) = default;
        
        /*!
         \brief Destructor
         */
        ~cover_zone_am_global_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::covreach::details::cover_zone_am_global_t<NODE_PTR> &
        operator= (tchecker::covreach::details::cover_zone_am_global_t<NODE_PTR> const & c) = default;
        
        /*!
         \brief Move assignment operator
         */
        tchecker::covreach::details::cover_zone_am_global_t<NODE_PTR> &
        operator= (tchecker::covreach::details::cover_zone_am_global_t<NODE_PTR> && c) = default;
        
        /*!
         \brief Node covering predicate
         \param n1 : a node
         \param n2 : a node
         \return true if the zone in n1 is included into the aM abstraction of the zone in n2, false
         otherwise
         */
        bool operator() (NODE_PTR const & n1, NODE_PTR const & n2)
        {
          return n1->zone().am_le(n2->zone(), _M.get());
        }
      private:
        std::reference_wrapper<tchecker::clockbounds::map_t const> _M;  /*!< global M clock bounds map */
      };
      
      
      
      
      /*!
       \class cover_zone_am_local_t
       \brief node covering w.r.t. zone aM-inclusion and local clock bounds
       \tparam NODE_PTR : type of pointer to node, *NODE_PTR should derive from tchecker::zg::details::state_t
       */
      template <class NODE_PTR>
      class cover_zone_am_local_t {
      public:
        /*!
         \brief Type of pointer to node
         */
        using node_ptr_t = NODE_PTR;
        
        /*!
         \brief Constructor
         \param model : clock bounds model
         \tparam MODEL : type of model, should inherit from tchecker::clockbounds::model_t
         \note this keeps a reference on model.local_m_map()
         */
        template <class MODEL>
        explicit cover_zone_am_local_t(MODEL const & model)
        : _local_m_map(model.local_m_map())
        {
          _M = tchecker::clockbounds::allocate_map(_local_m_map.get().clock_number());
        }
        
        /*!
         \brief Copy constructor
         */
        cover_zone_am_local_t(tchecker::covreach::details::cover_zone_am_local_t<NODE_PTR> const & c)
        : _local_m_map(c._local_m_map)
        {
          _M = tchecker::clockbounds::clone_map(*c._M);
        }
        
        /*!
         \brief Move constructor
         */
        cover_zone_am_local_t(tchecker::covreach::details::cover_zone_am_local_t<NODE_PTR> && c)
        : _local_m_map(std::move(c._local_m_map)), _M(c._M)
        {
          c._M = nullptr;
        }
        
        /*!
         \brief Destructor
         */
        ~cover_zone_am_local_t()
        {
          tchecker::clockbounds::deallocate_map(_M);
        }
        
        /*!
         \brief Assignment operator
         */
        tchecker::covreach::details::cover_zone_am_local_t<NODE_PTR> &
        operator= (tchecker::covreach::details::cover_zone_am_local_t<NODE_PTR> const & c)
        {
          if (this != &c) {
            _local_m_map = c._local_m_map;
            delete _M;
            _M = tchecker::clockbounds::clone_map(*c._M);
          }
          return *this;
        }
        
        /*!
         \brief Move assignment operator
         */
        tchecker::covreach::details::cover_zone_am_local_t<NODE_PTR> &
        operator= (tchecker::covreach::details::cover_zone_am_local_t<NODE_PTR> && c)
        {
          if (this != &c) {
            _local_m_map = std::move(c._local_m_map);
            delete _M;
            _M = c._M;
            c._M = nullptr;
          }
          return *this;
        }
        
        /*!
         \brief Node covering predicate
         \param n1 : a node
         \param n2 : a node
         \return true if the zone in n1 is included in the aM abstraction of the zone in n2 w.r.t. local
         clock bounds in n2
         */
        bool operator() (NODE_PTR const & n1, NODE_PTR const & n2)
        {
          tchecker::clockbounds::vloc_bounds(_local_m_map.get(), n2->vloc(), *_M);
          return n1->zone().am_le(n2->zone(), *_M);
        }
      private:
        std::reference_wrapper<tchecker::clockbounds::local_m_map_t const> _local_m_map;  /*!< Local M clockbounds map */
        tchecker::clockbounds::map_t * _M;                                                /*!< M clock bounds map */
      };
      
      
      
      
      /*!
       \class cover_sync_zone_inclusion_t
       \brief node covering w.r.t. inclusion of synchronized zone
       \tparam NODE_PTR : type of pointer to node, *NODE_PTR should derive from tchecker::async_zg::details::state_t
       */
      template <class NODE_PTR>
      class cover_sync_zone_inclusion_t {
      public:
        /*!
         \brief Type of pointer to node
         */
        using node_ptr_t = NODE_PTR;
        
        /*!
         \brief Constructor
         \param model : a model
         \tparam MODEL : type of model
         */
        template <class MODEL>
        explicit cover_sync_zone_inclusion_t(MODEL const & model)
        {}
        
        /*!
         \brief Copy constructor
         */
        cover_sync_zone_inclusion_t(tchecker::covreach::details::cover_sync_zone_inclusion_t<NODE_PTR> const &) = default;
        
        /*!
         \brief Move constructor
         */
        cover_sync_zone_inclusion_t(tchecker::covreach::details::cover_sync_zone_inclusion_t<NODE_PTR> &&) = default;
        
        /*!
         \brief Destructor
         */
        ~cover_sync_zone_inclusion_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::covreach::details::cover_sync_zone_inclusion_t<NODE_PTR> &
        operator= (tchecker::covreach::details::cover_sync_zone_inclusion_t<NODE_PTR> const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::covreach::details::cover_sync_zone_inclusion_t<NODE_PTR> &
        operator= (tchecker::covreach::details::cover_sync_zone_inclusion_t<NODE_PTR>  &&) = default;
        
        /*!
         \brief Covering predicate
         \param n1 : a node
         \param n2 : a node
         \return true if the zone in n1 is included in the zone in n2, false otherwise
         */
        bool operator() (NODE_PTR const & n1, NODE_PTR const & n2)
        {
          return (n1->sync_zone() <= n2->sync_zone());
        }
      };
      
      
      
      
      /*!
       \class cover_node_t
       \brief Node covering w.r.t. state predicate and zone predicate
       \tparam NODE_PTR : type of pointer to node
       \tparam STATE_PREDICATE : type of predicate on states in nodes, should have pointer to node NODE_PTR
       \tparam ZONE_PREDICATE : type of predicate on zones in nodes, should have pointer to node NODE_PTR
       */
      template <class NODE_PTR, class STATE_PREDICATE, class ZONE_PREDICATE>
      class cover_node_t : private STATE_PREDICATE, private ZONE_PREDICATE {
        static_assert(std::is_same<NODE_PTR, typename STATE_PREDICATE::node_ptr_t>::value, "");
        static_assert(std::is_same<NODE_PTR, typename ZONE_PREDICATE::node_ptr_t>::value, "");
      public:
        /*!
         \brief Type of pointer to node
         */
        using node_ptr_t = NODE_PTR;
        
        /*!
         \brief Constructor
         \param sp_args : arguments to a constructor of STATE_PREDICATE
         \param zp_args : arguments to a constructor of ZONE_PREDICATE
         */
        template <class ... SP_ARGS, class ... ZP_ARGS>
        cover_node_t(std::tuple<SP_ARGS...> && sp_args, std::tuple<ZP_ARGS...> && zp_args)
        : STATE_PREDICATE(std::make_from_tuple<STATE_PREDICATE>(sp_args)),
        ZONE_PREDICATE(std::make_from_tuple<ZONE_PREDICATE>(zp_args))
        {}
        
        /*!
         \brief Copy constructor
         */
        cover_node_t(tchecker::covreach::details::cover_node_t<NODE_PTR, STATE_PREDICATE, ZONE_PREDICATE> const &) = default;
        
        /*!
         \brief Move constructor
         */
        cover_node_t(tchecker::covreach::details::cover_node_t<NODE_PTR, STATE_PREDICATE, ZONE_PREDICATE> &&) = default;
        
        /*!
         \brief Destructor
         */
        ~cover_node_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::covreach::details::cover_node_t<NODE_PTR, STATE_PREDICATE, ZONE_PREDICATE> &
        operator= (tchecker::covreach::details::cover_node_t<NODE_PTR, STATE_PREDICATE, ZONE_PREDICATE> const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::covreach::details::cover_node_t<NODE_PTR, STATE_PREDICATE, ZONE_PREDICATE> &
        operator= (tchecker::covreach::details::cover_node_t<NODE_PTR, STATE_PREDICATE, ZONE_PREDICATE> &&) = default;
        
        /*!
         \brief Cover predicate
         \param n1 : a node
         \param n2 : a node
         \return true if covering n1 by n2 is valid, and (n1, n2) satisfy the state predicate,
         and (n1, n2) satisfy the zone predicate, false otherwise
         */
        bool operator() (NODE_PTR const & n1, NODE_PTR const & n2)
        {
          return (tchecker::covreach::details::valid_cover(n1, n2)
                  && STATE_PREDICATE::operator()(n1, n2)
                  && ZONE_PREDICATE::operator()(n1, n2));
        }
      };
      
    } // end of namespace details
    
    
    
    
    /*!
     \brief Node covering w.r.t. zone inclusion
     \tparam NODE_PTR : type of pointer to node
     \tparam STATE_PREDICATE : type of predicate on states in nodes
     */
    template <class NODE_PTR, class STATE_PREDICATE>
    using cover_inclusion_t
    = tchecker::covreach::details::cover_node_t
    <NODE_PTR, STATE_PREDICATE, tchecker::covreach::details::cover_zone_inclusion_t<NODE_PTR>>;
    
    
    /*!
     \brief Node covering w.r.t. zone aLU-inclusion and global LU clock bounds
     \tparam NODE_PTR : type of pointer to node
     \tparam STATE_PREDICATE : type of predicate on states in nodes
     */
    template <class NODE_PTR, class STATE_PREDICATE>
    using cover_alu_global_t
    = tchecker::covreach::details::cover_node_t
    <NODE_PTR, STATE_PREDICATE, tchecker::covreach::details::cover_zone_alu_global_t<NODE_PTR>>;
    
    
    /*!
     \brief Node covering w.r.t. zone aLU-inclusion and local LU clock bounds
     \tparam NODE_PTR : type of pointer to node
     \tparam STATE_PREDICATE : type of predicate on states in nodes
     */
    template <class NODE_PTR, class STATE_PREDICATE>
    using cover_alu_local_t
    = tchecker::covreach::details::cover_node_t
    <NODE_PTR, STATE_PREDICATE, tchecker::covreach::details::cover_zone_alu_local_t<NODE_PTR>>;
    
    
    /*!
     \brief Node covering w.r.t. zone aM-inclusion and global M clock bounds
     \tparam NODE_PTR : type of pointer to node
     \tparam STATE_PREDICATE : type of predicate on states in nodes
     */
    template <class NODE_PTR, class STATE_PREDICATE>
    using cover_am_global_t
    = tchecker::covreach::details::cover_node_t
    <NODE_PTR, STATE_PREDICATE, tchecker::covreach::details::cover_zone_am_global_t<NODE_PTR>>;
    
    
    /*!
     \brief Node covering w.r.t. zone aM-inclusion and local M clock bounds
     \tparam NODE_PTR : type of pointer to node
     \tparam STATE_PREDICATE : type of predicate on states in nodes
     */
    template <class NODE_PTR, class STATE_PREDICATE>
    using cover_am_local_t
    = tchecker::covreach::details::cover_node_t
    <NODE_PTR, STATE_PREDICATE, tchecker::covreach::details::cover_zone_am_local_t<NODE_PTR>>;
    
    
    /*!
     \brief Node covering w.r.t. inclusion of synchronized zone
     \tparam NODE_PTR : type of pointer to node
     \tparam STATE_PREDICATE : type of predicate on states in nodes
     */
    template <class NODE_PTR, class STATE_PREDICATE>
    using cover_sync_inclusion_t
    = tchecker::covreach::details::cover_node_t
    <NODE_PTR, STATE_PREDICATE, tchecker::covreach::details::cover_sync_zone_inclusion_t<NODE_PTR>>;
    
  } // end of namespace covreach
  
} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COVREACH_COVER_HH
