/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_POOL_HH
#define TCHECKER_POOL_HH

#include "tchecker/utils/gc.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/utils/spinlock.hh"

/*!
 \file pool.hh
 \brief Pool allocator
 */

namespace tchecker {
  
  /*!
   \class pool_t
   \brief Pool allocator with collection
   \tparam T : type of allocated objects. Should derive from 
   tchecker::make_shared_t<Y> for some Y
   \note Pools allocate blocks of memory. Each block contains a fix number of
   chunks. A chunk stores an object of type T. All chunks have the same fixed
   size alloc_size. A block contains a fixed alloc_nb chunks. The size of a
   block is alloc_nb * alloc_size + sizeof(void *). The extra size for a pointer
   is used to maintain a simple linked list of blocks.
   \note The pool is *NOT* thread-safe except for one particular usage. A thread
   can run the collect() method while another thread uses the other methods (and
   only the other methods). The first thread plays the role of a garbage collector
   whereas the other thread uses the pool to construct objects. A garbage
   collection thread is implemented in the class tchecker::gc_t.
   */
  template <class T>
  class pool_t {
  public:
    
    static_assert(std::is_same<T, tchecker::make_shared_t<typename T::object_t>>::value,
                  "T should have type tchecker::make_shared_t<...>");
    
    /*!
     \brief Size of the reference counter
     */
    static constexpr std::size_t SIZEOF_REFCOUNT = sizeof(typename T::refcount_t);
    
    /*!
     \brief Minimal allocation size
     */
    static constexpr std::size_t MIN_ALLOC_SIZE = SIZEOF_REFCOUNT + sizeof(void*);
    
    /*!
     \brief States of the reference counter used by the allocator
     */
    static constexpr typename T::refcount_t
    COLLECTABLE_CHUNK = 0,                   // used but not referenced anymore
    ALLOCATED_CHUNK   = T::REFCOUNT_MAX + 1, // used but not constructed yet
    FREE_CHUNK        = T::REFCOUNT_MAX + 2; // not used
    
    static_assert(ALLOCATED_CHUNK > T::REFCOUNT_MAX, "overflow on ALLOCATED_CHUNK");
    
    static_assert(FREE_CHUNK > T::REFCOUNT_MAX, "overflow on FREE_CHUNK" );
    
    /*!
     \brief Type of allocated objects
     */
    using t = T;
    
    /*!
     \brief Type of pointer to allocated objects
     */
    using ptr_t = tchecker::intrusive_shared_ptr_t<T>;
    
    /*!
     \brief Constructor
     \param alloc_nb : number of chunks in a block (allocation unit)
     \param alloc_size : fixed size of chunks
     \pre alloc_nb >= 1
     \post initialized to empty pool that allocates memory by blocks of alloc_nb
     chunks, each chunk of size max(alloc_size, MIN_ALLOC_SIZE) bytes
     \note alloc_size should be determined by a call to
     tchecker::allocation_size_t<T>::alloc_size(). This class should in turn be
     specialized for type T
     \note the pool allocates at least MIN_ALLOC_SIZE bytes per chunk. Hence, the
     actual allocation size is max(alloc_size, MIN_ALLOC_SIZE). The MIN_ALLOC_SIZE
     bytes are needed to maintain a list of free chunk, while keeping the value of
     the reference counter of each chunk untouched.
     \note extra sizeof(void *) bytes are allocated for each block to maintain
     a list of allocated blocks
     \throw std::invalid argument when the precondition is not satisfied
     */
    pool_t(std::size_t alloc_nb, std::size_t alloc_size)
    : _alloc_nb(alloc_nb),
    _alloc_size(std::max(alloc_size, MIN_ALLOC_SIZE)),
    _block_size(_alloc_nb * _alloc_size + sizeof(void *)),
    _blocks_count(0),
    _free_head(nullptr),
    _block_head(nullptr),
    _raw_head(nullptr),
    _raw_end(nullptr)
    {
      if (_alloc_nb < 1)
        throw std::invalid_argument("allocation number should be >= 1");
    }
    
    /*!
     \brief Copy constructor (deleted)
     */
    pool_t(pool_t<T> const &) = delete;
    
    /*!
     \brief Move constructor (deleted)
     */
    pool_t(pool_t<T> &&) = delete;
    
    /*!
     \brief Destructor
     \post All the objects allocated by the pool have been destructed
     \note see tchecker::pool_t::destruct_all
     */
    ~pool_t()
    {
      destruct_all();
    }
    
    /*!
     \brief Assignment operator (deleted)
     */
    pool_t<T> & operator= (pool_t<T> const &) = delete;
    
    /*!
     \brief Move Assignment operator (deleted)
     */
    pool_t<T> & operator= (pool_t<T> &&) = delete;
    
    /*!
     \brief Construct an object
     \param args : parameters to a constructor of type T
     \return A new instance of T built with args and allocated by the pool
     */
    template <class... ARGS>
    tchecker::intrusive_shared_ptr_t<T> construct(ARGS && ... args)
    {
      void * t = allocate();
      if (t == nullptr)
        return tchecker::intrusive_shared_ptr_t<T>(nullptr);
      // p points after the reference counter
      void * p = static_cast<typename T::refcount_t *>(t) + 1;
      try {
        T::construct(p, std::forward<ARGS>(args)...);  // construct T in p with args
      }
      catch(...) {
        this->release(t);
        throw;
      }
      return tchecker::intrusive_shared_ptr_t<T>( reinterpret_cast<T *>(p) );
    }
    
    /*!
     \brief Destruct an object
     \param p : pointer to object
     \pre p has been allocated by this pool
     p is not nullptr
     \post the object pointed by p has been destructed and de-allocated if its reference
     counter is 1 (i.e. p is the only pointer to the obejct), and the pointer p points to
     nullptr. Does nothing otherwise
     \return true if the object pointed by p has been destructed, false otherwise
     */
    bool destruct(tchecker::intrusive_shared_ptr_t<T> & p)
    {
      if (p.ptr() == nullptr)
        return false;
      if (p->refcount() != 1)
        return false;
      
      T * t = p.ptr();
      T::destruct(t);
      
      typename T::refcount_t * chunk = reinterpret_cast<typename T::refcount_t *>(t) - 1;
      release(chunk);
      
      p = nullptr;
      
      return true;
    }
    
    /*!
     \brief Collects unused chunks
     \post All objects with reference counter = 0 (COLLECTABLE_CHUNK) have been
     collected in the list of free objects, and their counters have been set to
     FREE_CHUNK
     \return Number of collected chunks
     */
    std::size_t collect()
    {
      std::size_t collected = 0;
      void * collected_begin = nullptr, * collected_end = nullptr;
      
      // Move to the free list all chunks in blocks list that are not in the
      // free list and that are unused
      for (void * block = _block_head; block != nullptr;
           block = nextblock(block))
      {
        void * block_end = static_cast<char *>(block) + _block_size;
        
        for (char * chunk = first_chunk_ptr(block) ;  chunk != block_end; chunk += _alloc_size)
        {
          // Ignore chunks inside unused raw block (refcount not set yet)
          if ( (_raw_head <= chunk) && (chunk < _raw_end) )
            break; // ignore the entire raw block
          
          // Collect all unused chunks in the free list
          typename T::refcount_t * refcount = reinterpret_cast<typename T::refcount_t *>(chunk);
          if (*refcount == COLLECTABLE_CHUNK) {
            // make the chunk free using its refcount
            *refcount = FREE_CHUNK;
            // destruct the object in the chunk
            T * t = reinterpret_cast<T*>(refcount + 1);
            T::destruct(t);  // t->~T()
            // add the chunk to the list of collected chunks
            this->nextchunk(chunk) = collected_begin;
            if (collected_end == nullptr)
              collected_end = chunk;
            collected_begin = chunk;
            ++ collected;
          }
        }
      }
      
      // release the collected chunks
      if (collected > 0)
        this->release(collected_begin, collected_end);
      
      return collected;
    }
    
    
    /*!
     \brief Destruct all the objects allocated by the pool
     \post All the objects allocated by the pool have been destructed. All the
     memory allocated by the pool has been freed. The pool is empty.
     */
    void destruct_all() {
      // Call destructor on all chunks in blocks list that
      // - are not in free list
      // - are not in raw block
      for (void * block = _block_head; block != nullptr; block = nextblock(block))
      {
        void * block_end = static_cast<char *>(block) + _block_size;
        
        for (char * chunk = first_chunk_ptr(block); chunk != block_end; chunk += _alloc_size)
        {
          // Ignore chunks inside unused raw block
          if ( (_raw_head <= chunk) && (chunk < _raw_end) )
            break;
          
          // Destruct all chuncks that have been constructed
          typename T::refcount_t * refcount = reinterpret_cast<typename T::refcount_t *>(chunk);
          
          if ( (0 <= *refcount) && (*refcount <= T::REFCOUNT_MAX) ) {
            // make the chunk free using its refcount
            *refcount = FREE_CHUNK;
            // destruct the object in the chunk
            T * t = reinterpret_cast<T*>(refcount + 1);
            T::destruct(t);  // t->~T();
          }
        }
      }
      
      // Free memory
      free_all();
    }
    
    
    /*!
     \brief Free all allocated memory
     \post All the memory allocated by the pool has been freed. The pool is
     empty
     \note All the objects constructed by the pool have been invalidated
     \note No destructor called on allocated objects
     (see tchecker::pool_t::destruct_all for clean destruction)
     */
    void free_all()
    {
      void * p = _block_head, * tmp = nullptr;
      while (p != nullptr) {
        tmp = p;
        p = nextblock(p);
        delete[] static_cast<char *>(tmp);
      }
      _blocks_count = 0;
      _free_head = nullptr;   // _free_head_lock access protection useless
      _block_head = nullptr;
      _raw_head = nullptr;
      _raw_end = nullptr;
    }
    
    /*!
     \brief Accessor
     \return Memory footprint of the pool
     \note Constant time
     */
    inline constexpr std::size_t memsize() const
    {
      return (_blocks_count * _block_size);
    }
    
    /*!
     \brief Enroll to garbarge collector
     \param gc : a garbage collector
     \pre this is not enrolled to a garbage collector yet
     \post this is enrolled to gc
     */
    void enroll(tchecker::gc_t & gc)
    {
      gc.enroll( [&] () { this->collect(); } );
    }
  protected:
    /*!
     \brief Accessor to next chunk
     \param ptr : pointer to a chunk
     \return mutable address of the next chunk in the linked list of chunks
     starting at ptr if any, nullptr otherwise
     \pre the address of the next chunk is stored with offset SIZEOF_REFCOUNT
     from ptr
     */
    static constexpr void * & nextchunk(void * const ptr)
    {
      return *( reinterpret_cast<void **>(static_cast<typename T::refcount_t *>(ptr) + 1) );
    }
    
    /*!
     \brief Accessor to next block
     \param ptr : pointer to a block
     \return mutable address of the next block in the linked list of blocks
     starting at ptr if any, nullptr otherwise
     \pre the address of the next block is stored is the first sizeof(void*)
     bytes of the block
     */
    static constexpr void * & nextblock(void * const ptr)
    {
      return *( reinterpret_cast<void **>(ptr) );
    }
    
    /*!
     \brief Accessor
     \param block : address of a block
     \return address of first chunk in block
     \pre the first chunk is located sizeof(void *) bytes after block
     */
    static constexpr char * first_chunk_ptr(void * const block)
    {
      return (static_cast<char *>(block) + sizeof(void *));
    }
    
    /*!
     \brief Memory allocation
     \return a pointer to a chunk of _alloc_size bytes
     \throw std::bad_alloc : if no memory left (i.e. when operator new throws)
     */
    inline void * allocate()
    {
      // Use a free chunk if any
      _free_head_lock.lock();
      if (_free_head != nullptr) {
        typename T::refcount_t * refcount = reinterpret_cast<typename T::refcount_t *>(_free_head);
        *refcount = ALLOCATED_CHUNK; // protect the first chunk from collection (i.e. method collect())
        char * chunk = _free_head;
        _free_head = static_cast<char *>(nextchunk(chunk));
        _free_head_lock.unlock();
        return chunk;
      }
      _free_head_lock.unlock();
      
      // Allocate a new block if no chunk available
      if (_raw_head == _raw_end) {
        // allocate (collect() may interleave with no problem)
        _raw_head = new char[_block_size];
        _raw_end = _raw_head + _block_size;
        // link to allocated blocks
        nextblock(_raw_head) = _block_head;
        _block_head = _raw_head;
        // jump over 1st word used for linking chunks
        _raw_head = first_chunk_ptr(_raw_head);
        // count one more block
        ++ _blocks_count;
      }
      
      // Allocate a chunk from the raw block
      typename T::refcount_t * chunk = reinterpret_cast<typename T::refcount_t *>(_raw_head);
      *chunk = ALLOCATED_CHUNK;  // protect the chunk from GC
      _raw_head += _alloc_size;
      
      return chunk;
    }
    
    /*!
     \brief Release allocated memory
     \param chunk : pointer to chunk to release
     \pre chunk has been returned by allocate()
     \post chunk has been released. The first SIZEOF_REFCOUNT bytes of chunk
     have not been modified
     */
    void release(void const * chunk)
    {
      typename T::refcount_t * refcount = static_cast<typename T::refcount_t *>(const_cast<void *>(chunk));
      *refcount = FREE_CHUNK;
      this->release(chunk, chunk);
    }
    
    /*!
     \brief Release a list of chunks
     \param begin : first chunk in the list
     \param end : last chunk in the list
     \pre begin != nullptr and end != nullptr. Successive applications of
     nextof() from begin eventually lead to end (linked list according to
     nextof()). All the chunks in the list have been returned by allocate() and
     have their refcount value FREE_CHUNK
     \post All the chunks in the list begin..end have been released
     */
    inline void release(void const * begin, void const * end)
    {
      void * pbegin = const_cast<void *>(begin);
      void * pend = const_cast<void *>(end);
      _free_head_lock.lock();
      nextchunk(pend) = _free_head;
      _free_head = static_cast<char *>(pbegin);
      _free_head_lock.unlock();
    }
    
    
    std::size_t const _alloc_nb;     /*!< number of chunks per block */
    std::size_t const _alloc_size;   /*!< size of a chunk (bytes) */
    std::size_t const _block_size;   /*!< size of a block (bytes) */
    std::size_t _blocks_count;       /*!< number of allocated blocks */
    char * _free_head;               /*!< head pointer to list of free chunks */
    char * _block_head;              /*!< head pointer to list of blocks */
    char * _raw_head;                /*!< pointer to raw block */
    char * _raw_end;                 /*!< pointer to past-the-end raw block */
    spinlock_t _free_head_lock;      /*!< protect access to _free_head */
  };
  
} // end of namespace tchecker

#endif // TCHECKER_POOL_HH
