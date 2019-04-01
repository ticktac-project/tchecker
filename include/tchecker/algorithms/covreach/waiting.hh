/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COVREACH_WAITING_HH
#define TCHECKER_ALGORITHMS_COVREACH_WAITING_HH

#include "tchecker/utils/waiting.hh"

/*!
 \file waiting.hh
 \brief Waiting containers for covering reachability algorithm
 */

namespace tchecker {
  
  namespace covreach {
    
    namespace details {
      
      /*!
       \class active_node_t
       \brief Node with active flag
       \note allows to remove nodes efficiently from a waiting nodes container
       */
      class active_node_t {
      public:
        /*!
         \brief Constructor
         \post this node is active
         */
        active_node_t() : _active(1)
        {}
        
        /*!
         \brief Copy constructor
         */
        active_node_t(tchecker::covreach::details::active_node_t const &) = default;
        
        /*!
         \brief Move constructor
         */
        active_node_t(tchecker::covreach::details::active_node_t &&) = default;
        
        /*!
         \brief Destructor
         */
        ~active_node_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::covreach::details::active_node_t & operator= (tchecker::covreach::details::active_node_t const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::covreach::details::active_node_t & operator= (tchecker::covreach::details::active_node_t &&) = default;
        
        /*!
         \brief Accessor
         \return true if this node is active, false otherwise
         */
        bool is_active() const
        {
          return (_active == 1);
        }
        
        /*!
         \brief Make inactive
         \post this node is inactive
         */
        void make_inactive()
        {
          _active = 0;
        }
      private:
        unsigned char _active : 1;    /*!< Active node flag */
      };
      
      
      
      
      /*!
       \brief Filter active nodes
       \param node : a node, should derive from tchecker::covreach::details::active_node_t
       \return true if node is active, false otherwise
       */
      template <class NODE_PTR>
      bool active_node_filter(NODE_PTR const & node)
      {
        return node->is_active();
      }
      
      
      
      
      /*!
       \class active_waiting_t
       \brief Waiting container that filters active nodes
       \param W : type of underlying waiting container, should contain nodes that inherit
       from tchecker::covreach::details::active_node_t
       \note this container acts as W restricted to its active nodes
       */
      template <class W>
      class active_waiting_t : public tchecker::filter_waiting_t<W> {
      public:
        /*!
         \brief Type of pointers to node
         */
        using node_ptr_t = typename W::element_t;
        
        /*!
         \brief Constructor
         */
        active_waiting_t()
        : tchecker::filter_waiting_t<W>(tchecker::covreach::details::active_node_filter<node_ptr_t>)
        {}
        
        /*!
         \brief Copy constructor
         */
        active_waiting_t(tchecker::covreach::details::active_waiting_t<W> const &) = default;
        
        /*!
         \brief Move constructor
         */
        active_waiting_t(tchecker::covreach::details::active_waiting_t<W> &&) = default;
        
        /*!
         \brief Destructor
         */
        ~active_waiting_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::covreach::details::active_waiting_t<W> &
        operator= (tchecker::covreach::details::active_waiting_t<W> const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::covreach::details::active_waiting_t<W> &
        operator= (tchecker::covreach::details::active_waiting_t<W> &&) = default;
        
        /*!
         \brief Remove node
         \param n : a node
         \post n is inactive
         \note n is actually not removed from this waiting container but marked inactive. The node n is removed and
         ingored when it becomes the first element in this container
         */
        void remove(node_ptr_t const & n)
        {
          n->make_inactive();
        }
      };
      
    } // end of namespace details
    
    
    /*!
     \brief First-In-First-Out waiting container
     \note this container filters active nodes
     */
    template <class NODE_PTR>
    using fifo_waiting_t = tchecker::covreach::details::active_waiting_t<tchecker::fifo_waiting_t<NODE_PTR>>;
    
    
    /*!
     \brief Last-In-First-Out waiting container
     \note this container filters active nodes
     */
    template <class NODE_PTR>
    using lifo_waiting_t = tchecker::covreach::details::active_waiting_t<tchecker::lifo_waiting_t<NODE_PTR>>;
    
  } // end of namespace covreach
  
} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COVREACH_WAITING_HH
