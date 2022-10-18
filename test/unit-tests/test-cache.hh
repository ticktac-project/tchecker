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
class A_t : public tchecker::cached_object_t {
public:
  A_t(int x, int y) : _x(x), _y(y) {}
  int x() const { return _x; }
  int y() const { return _y; }

private:
  int _x;
  int _y;
};

using shared_A_t = tchecker::make_shared_t<A_t>;

using A_sptr_t = tchecker::intrusive_shared_ptr_t<shared_A_t>;

class A_hash_t {
public:
  std::size_t operator()(A_sptr_t const & a) const noexcept { return static_cast<std::size_t>(a->x()); }
};

class A_equal_t {
public:
  bool operator()(A_sptr_t const & a1, A_sptr_t const & a2) const { return (a1->x() == a2->x() && a1->y() == a2->y()); }
};

using A_cache_t = tchecker::cache_t<A_sptr_t, A_hash_t, A_equal_t>;

using A_periodic_collectable_cache_t = tchecker::periodic_collectable_cache_t<A_sptr_t, A_hash_t, A_equal_t>;

namespace tchecker {
template <> class allocation_size_t<A_t> {
public:
  static constexpr std::size_t alloc_size() { return sizeof(A_t); }

  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... args) { return sizeof(A_t); }
};
} // namespace tchecker

// Test cases

TEST_CASE("Empty cache", "[cache]")
{

  A_cache_t cache;

  SECTION("Empty cache has size 0") { REQUIRE(cache.size() == 0); }

  SECTION("Empty cache does not find ")
  {
    shared_A_t * a1 = shared_A_t::allocate_and_construct(1, 1);
    tchecker::intrusive_shared_ptr_t<shared_A_t> p1(a1);

    REQUIRE_FALSE(cache.find(p1));

    cache.clear();
    p1 = nullptr;
    shared_A_t::destruct_and_deallocate(a1);
  }
}

TEST_CASE("Cache with 1 element", "[cache]")
{

  A_cache_t cache;

  shared_A_t * a1 = shared_A_t::allocate_and_construct(1, 1);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p1(a1);
  shared_A_t * a1bis = shared_A_t::allocate_and_construct(1, 1);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p1bis(a1bis);
  shared_A_t * a2 = shared_A_t::allocate_and_construct(1, 2);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p2(a2);
  shared_A_t * a3 = shared_A_t::allocate_and_construct(2, 1);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p3(a3);

  SECTION("Caching an A")
  {
    REQUIRE(cache.find_else_add(p1) == p1);
    REQUIRE(cache.find(p1));
    REQUIRE(cache.size() == 1);
  }

  SECTION("Finding a cached A")
  {
    cache.find_else_add(p1);
    REQUIRE(cache.find(p1));

    REQUIRE(cache.find_else_add(p1bis) == p1);
    REQUIRE(cache.size() == 1);
  }

  SECTION("Not finding a non-cached A")
  {
    cache.find_else_add(p1);
    REQUIRE(cache.find(p1));

    REQUIRE_FALSE(cache.find(p2));
    REQUIRE(cache.size() == 1);

    REQUIRE_FALSE(cache.find(p3));
    REQUIRE(cache.size() == 1);
  }

  SECTION("Collecting nothing from cache with used element")
  {
    cache.find_else_add(p1);
    REQUIRE(cache.find(p1));

    std::size_t ncollect = cache.collect();

    REQUIRE(ncollect == 0);
    REQUIRE(cache.size() == 1);
    REQUIRE(cache.find(p1));
  }

  SECTION("Collecting from cache with unused element")
  {
    cache.find_else_add(p1);
    REQUIRE(cache.find(p1));

    p1 = nullptr;

    std::size_t ncollect = cache.collect();

    REQUIRE(ncollect == 1);
    REQUIRE(cache.size() == 0);
  }

  cache.clear();
  p3 = nullptr;
  shared_A_t::destruct_and_deallocate(a3);
  p2 = nullptr;
  shared_A_t::destruct_and_deallocate(a2);
  p1bis = nullptr;
  shared_A_t::destruct_and_deallocate(a1bis);
  p1 = nullptr;
  shared_A_t::destruct_and_deallocate(a1);
}

TEST_CASE("Cache with many elements", "[cache]")
{
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

  SECTION("Adding/finding objects in the cache")
  {
    REQUIRE(cache.size() == 0);

    REQUIRE(cache.find_else_add(p1) == p1);
    REQUIRE(cache.size() == 1);

    REQUIRE(cache.find_else_add(p1bis) == p1);
    REQUIRE(cache.size() == 1);

    REQUIRE(cache.find_else_add(p2) == p2);
    REQUIRE(cache.size() == 2);

    REQUIRE(cache.find_else_add(p3) == p3);
    REQUIRE(cache.size() == 3);

    REQUIRE(cache.find(p3bis));
    REQUIRE(cache.find_else_add(p3bis) == p3);
    REQUIRE(cache.size() == 3);

    REQUIRE(!cache.find(p4));
    REQUIRE(cache.find_else_add(p4) == p4);
    REQUIRE(cache.size() == 4);
  }

  SECTION("Collecting unused objects from the cache (1)")
  {
    cache.find_else_add(p1);
    cache.find_else_add(p2);
    cache.find_else_add(p3);
    cache.find_else_add(p4);

    REQUIRE(cache.size() == 4);

    p2 = nullptr;
    p4 = nullptr;

    std::size_t ncollect = cache.collect();

    REQUIRE(ncollect == 2);
    REQUIRE(cache.size() == 2);
    REQUIRE(cache.find(p1));
    REQUIRE(cache.find(p3));
  }

  SECTION("Collecting unused objects from the cache (2)")
  {
    cache.find_else_add(p1);
    cache.find_else_add(p2);
    cache.find_else_add(p3);
    cache.find_else_add(p4);

    REQUIRE(cache.size() == 4);

    p1 = nullptr;
    p4 = nullptr;

    std::size_t ncollect = cache.collect();

    REQUIRE(ncollect == 2);
    REQUIRE(cache.size() == 2);
    REQUIRE(cache.find(p2));
    REQUIRE(cache.find(p3));
  }

  SECTION("Collecting unused objects from the cache (3)")
  {
    cache.find_else_add(p1);
    cache.find_else_add(p2);
    cache.find_else_add(p3);
    cache.find_else_add(p4);

    REQUIRE(cache.size() == 4);

    p1 = nullptr;
    p2 = nullptr;
    p3 = nullptr;
    p4 = nullptr;

    std::size_t ncollect = cache.collect();

    REQUIRE(ncollect == 4);
    REQUIRE(cache.size() == 0);
  }

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

TEST_CASE("Periodic collectable cache with nothing to collect", "[cache]")
{
  A_periodic_collectable_cache_t cache;

  shared_A_t * a1 = shared_A_t::allocate_and_construct(1, 1);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p1(a1);
  shared_A_t * a2 = shared_A_t::allocate_and_construct(1, 2);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p2(a2);
  shared_A_t * a3 = shared_A_t::allocate_and_construct(2, 1);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p3(a3);
  shared_A_t * a4 = shared_A_t::allocate_and_construct(4, 7);
  tchecker::intrusive_shared_ptr_t<shared_A_t> p4(a4);

  SECTION("Object collection at work")
  {
    cache.find_else_add(p1);
    cache.find_else_add(p2);
    cache.find_else_add(p3);
    cache.find_else_add(p4);

    p1 = nullptr;
    p4 = nullptr;

    std::size_t ncollect = cache.collect();
    REQUIRE(ncollect == 2);
  }

  SECTION("Nothing to collect leads to no collection next time")
  {
    cache.find_else_add(p1);
    cache.find_else_add(p2);
    cache.find_else_add(p3);
    cache.find_else_add(p4);

    std::size_t ncollect = cache.collect();
    REQUIRE(ncollect == 0);

    // make p1 collectable, still should not be collected due to collection period
    p1 = nullptr;
    ncollect = cache.collect();
    REQUIRE(ncollect == 0);
  }

  SECTION("Nothing to collect leads collection after two calls")
  {
    cache.find_else_add(p1);
    cache.find_else_add(p2);
    cache.find_else_add(p3);
    cache.find_else_add(p4);

    std::size_t ncollect = cache.collect();
    REQUIRE(ncollect == 0);

    // make p1 and p3 collectable, still should not be collected due to collection period
    p1 = nullptr;
    p3 = nullptr;
    ncollect = cache.collect();
    REQUIRE(ncollect == 0);

    // now collection should occur, as collection period has been reached
    ncollect = cache.collect();
    REQUIRE(ncollect == 2);
  }

  cache.clear();
  p1 = nullptr;
  p2 = nullptr;
  p3 = nullptr;
  p4 = nullptr;
  shared_A_t::destruct_and_deallocate(a4);
  shared_A_t::destruct_and_deallocate(a3);
  shared_A_t::destruct_and_deallocate(a2);
  shared_A_t::destruct_and_deallocate(a1);
}
