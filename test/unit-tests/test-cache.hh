/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <functional>

#include "tchecker/utils/cache.hh"
#include "tchecker/utils/shared_objects.hh"

// Object for testing
class A_t {
public:
  A_t(int x, int y) : _x(x), _y(y) {}
  int x() const { return _x; }
  int y() const { return _y; }
private:
  int _x;
  int _y;
};

namespace std {
  template<> struct hash<A_t> {
    std::size_t operator() (A_t const& a) const noexcept
    {
      return static_cast<std::size_t>(a.x());
    }
  };
}

bool operator== (A_t const & a1, A_t const & a2)
{
  return ((a1.x() == a2.x()) && (a1.y() == a2.y()));
}

using shared_A_t = tchecker::make_shared_t<A_t>;

using A_cache_t = tchecker::cache_t<shared_A_t, std::hash<A_t>, std::equal_to<A_t>>;

namespace tchecker {
  template <>
  class allocation_size_t<A_t> {
  public:
    static constexpr std::size_t alloc_size()
    {
      return sizeof(A_t);
    }
    
    template <class ... ARGS>
    static constexpr std::size_t alloc_size(ARGS && ... args)
    {
      return sizeof(A_t);
    }
  };
}



// Test cases

TEST_CASE( "Empty cache", "[cache]" ) {

  A_cache_t cache;

  SECTION( "Empty cache has size 0" ) {
    REQUIRE( cache.size() == 0 );
  }
  
  SECTION( "Empty cache does not find " ) {
    shared_A_t * a1 = shared_A_t::allocate_and_construct(1, 1);
    tchecker::intrusive_shared_ptr_t<shared_A_t> p1(a1);
    
    REQUIRE( ! cache.find(p1) );
    
    cache.clear();
    p1 = nullptr;
    shared_A_t::destruct_and_deallocate(a1);
  }
}


TEST_CASE( "Cache with 1 element", "[cache]" ) {

  A_cache_t cache;
  
  SECTION( "Caching an A" ) {
    shared_A_t * a1 = shared_A_t::allocate_and_construct(1, 1);
    tchecker::intrusive_shared_ptr_t<shared_A_t> p1(a1);
    
    REQUIRE( cache.find_else_insert(p1) == p1 );
    REQUIRE( cache.size() == 1 );
    
    cache.clear();
    p1 = nullptr;
    shared_A_t::destruct_and_deallocate(a1);
  }
  
  SECTION( "Finding a cached A" ) {
    shared_A_t * a1 = shared_A_t::allocate_and_construct(1, 1);
    tchecker::intrusive_shared_ptr_t<shared_A_t> p1(a1);
    
    cache.find_else_insert(p1);
    
    shared_A_t * a2 = shared_A_t::allocate_and_construct(1, 1);
    tchecker::intrusive_shared_ptr_t<shared_A_t> p2(a2);
    
    REQUIRE( cache.find_else_insert(p2) == p1 );
    REQUIRE( cache.size() == 1 );
    
    cache.clear();
    p2 = nullptr;
    shared_A_t::destruct_and_deallocate(a2);
    p1 = nullptr;
    shared_A_t::destruct_and_deallocate(a1);
  }
  
  SECTION( "Not finding a non-cached A" ) {
    shared_A_t * a1 = shared_A_t::allocate_and_construct(1, 1);
    tchecker::intrusive_shared_ptr_t<shared_A_t> p1(a1);
    
    cache.find_else_insert(p1);
    
    shared_A_t * a2 = shared_A_t::allocate_and_construct(1, 2);
    tchecker::intrusive_shared_ptr_t<shared_A_t> p2(a2);
    
    REQUIRE( ! cache.find(p2) );
    REQUIRE( cache.size() == 1 );
    
    shared_A_t * a3 = shared_A_t::allocate_and_construct(2, 1);
    tchecker::intrusive_shared_ptr_t<shared_A_t> p3(a3);
    
    REQUIRE( ! cache.find(p3) );
    REQUIRE( cache.size() == 1 );
    
    cache.clear();
    p3 = nullptr;
    shared_A_t::destruct_and_deallocate(a3);
    p2 = nullptr;
    shared_A_t::destruct_and_deallocate(a2);
    p1 = nullptr;
    shared_A_t::destruct_and_deallocate(a1);
  }
}

TEST_CASE( "Cache with many elements", "[cache]" ) {

  A_cache_t cache;
 
  shared_A_t * a1 = shared_A_t::allocate_and_construct(1, 1);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p1(a1);
  shared_A_t * a2 = shared_A_t::allocate_and_construct(1, 2);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p2(a2);
  shared_A_t * a3 = shared_A_t::allocate_and_construct(2, 1);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p3(a3);
  shared_A_t * a4 = shared_A_t::allocate_and_construct(4, 7);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p4(a4);
  shared_A_t * a1bis = shared_A_t::allocate_and_construct(*a1);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p1bis(a1bis);
  shared_A_t * a3bis = shared_A_t::allocate_and_construct(*a3);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p3bis(a3bis);
  
  REQUIRE(cache.size() == 0);
  
  REQUIRE(cache.find_else_insert(p1) == p1);
  REQUIRE(cache.size() == 1);
  
  REQUIRE(cache.find_else_insert(p1bis) == p1);
  REQUIRE(cache.size() == 1);
  
  REQUIRE(cache.find_else_insert(p2) == p2);
  REQUIRE(cache.size() == 2);
  
  REQUIRE(cache.find_else_insert(p3) == p3);
  REQUIRE(cache.size() == 3);
  
  REQUIRE(cache.find(p3bis));
  REQUIRE(cache.find_else_insert(p3bis) == p3);
  REQUIRE(cache.size() == 3);
  
  REQUIRE(! cache.find(p4));
  REQUIRE(cache.find_else_insert(p4) == p4);
  REQUIRE(cache.size() == 4);
  
  cache.clear();
  p1 = nullptr;
  p2 = nullptr;
  p3 = nullptr;
  p4 = nullptr;
  p1bis = nullptr;
  p3bis = nullptr;
  shared_A_t::destruct_and_deallocate(a3bis);
  shared_A_t::destruct_and_deallocate(a1bis);
  shared_A_t::destruct_and_deallocate(a4);
  shared_A_t::destruct_and_deallocate(a3);
  shared_A_t::destruct_and_deallocate(a2);
  shared_A_t::destruct_and_deallocate(a1);
}
