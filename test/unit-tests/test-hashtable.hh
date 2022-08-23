/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <functional>
#include <iterator>

#include "tchecker/utils/hashtable.hh"
#include "tchecker/utils/shared_objects.hh"

// Object for testing collision table
class cto_t : public tchecker::collision_table_object_t {
public:
  cto_t(int x, int y) : _x(x), _y(y) {}
  cto_t(std::tuple<int, int> t) : _x(std::get<0>(t)), _y(std::get<1>(t)) {}
  int x() const { return _x; }
  int y() const { return _y; }

private:
  int _x;
  int _y;
};

std::size_t hash(cto_t const & o) { return static_cast<std::size_t>(o.x()); }

bool operator==(cto_t const & o1, cto_t const & o2) { return ((o1.x() == o2.x()) && (o1.y() == o2.y())); }

namespace tchecker {
template <> class allocation_size_t<cto_t> {
public:
  static constexpr std::size_t alloc_size() { return sizeof(cto_t); }

  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... args) { return sizeof(cto_t); }
};
} // namespace tchecker

using shared_cto_t = tchecker::make_shared_t<cto_t>;

using cto_sptr_t = tchecker::intrusive_shared_ptr_t<shared_cto_t>;

class cto_sptr_hash_t {
public:
  std::size_t operator()(cto_sptr_t const & p) const { return hash(*p); }
};

class cto_sptr_equal_t {
public:
  bool operator()(cto_sptr_t const & p1, cto_sptr_t const & p2) const { return (*p1 == *p2); }
};

// Test cases

TEST_CASE("Empty collision table", "[hashtable]")
{
  cto_sptr_hash_t hash;
  tchecker::collision_table_t<cto_sptr_t, cto_sptr_hash_t> t(1024, hash);

  SECTION("Empty collision table has size 0") { REQUIRE(t.size() == 0); }
}

TEST_CASE("Collision table with one element", "[hashtable]")
{
  cto_sptr_hash_t hash;
  tchecker::collision_table_t<cto_sptr_t, cto_sptr_hash_t> t(1024, hash);

  cto_sptr_t o1{shared_cto_t::allocate_and_construct(1, 1)};
  t.add(o1);

  SECTION("Collision table with one object has size 1") { REQUIRE(t.size() == 1); }

  SECTION("The range of objects in the table is exactly o")
  {
    auto r = t.range();
    REQUIRE(std::distance(r.begin(), r.end()) == 1);

    auto it = r.begin();
    REQUIRE(*it == o1);
  }

  SECTION("The range of objects in the collision list of o is exactly o")
  {
    auto r = t.collision_range(o1);
    REQUIRE(std::distance(r.begin(), r.end()) == 1);

    auto it = r.begin();
    REQUIRE(*it == o1);
  }

  t.clear();
  shared_cto_t * p1 = o1.ptr();
  o1 = nullptr;
  shared_cto_t::destruct_and_deallocate(p1);
}

TEST_CASE("Collision table with two elements no collision", "[hashtable]")
{
  cto_sptr_hash_t hash;
  tchecker::collision_table_t<cto_sptr_t, cto_sptr_hash_t> t(1024, hash);

  cto_sptr_t o1{shared_cto_t::allocate_and_construct(1, 1)};
  t.add(o1);
  cto_sptr_t o2{shared_cto_t::allocate_and_construct(14, 157)};
  t.add(o2);
  REQUIRE(hash(o1) != hash(o2));

  SECTION("Collision table with two object has size 2") { REQUIRE(t.size() == 2); }

  SECTION("The range of objects in the table is exactly o1, o2")
  {
    auto r = t.range();
    REQUIRE(std::distance(r.begin(), r.end()) == 2);

    bool o1_found = false, o2_found = false;
    for (cto_sptr_t p : r) {
      if (p == o1)
        o1_found = true;
      if (p == o2)
        o2_found = true;
    }
    REQUIRE(o1_found);
    REQUIRE(o2_found);
  }

  SECTION("The range of objects in the collision list of o1 is exactly o1")
  {
    auto r = t.collision_range(o1);
    REQUIRE(std::distance(r.begin(), r.end()) == 1);

    auto it = r.begin();
    REQUIRE(*it == o1);
  }

  SECTION("The range of objects in the collision list of o2 is exactly o2")
  {
    auto r = t.collision_range(o2);
    REQUIRE(std::distance(r.begin(), r.end()) == 1);

    auto it = r.begin();
    REQUIRE(*it == o2);
  }

  SECTION("Removing an object from the collision table")
  {
    t.remove(o1);
    REQUIRE(t.size() == 1);

    auto r = t.collision_range(o2);
    REQUIRE(std::distance(r.begin(), r.end()) == 1);

    auto it = r.begin();
    REQUIRE(*it == o2);
  }

  t.clear();
  shared_cto_t *p1 = o1.ptr(), *p2 = o2.ptr();
  o1 = nullptr;
  shared_cto_t::destruct_and_deallocate(p1);
  o2 = nullptr;
  shared_cto_t::destruct_and_deallocate(p2);
}

TEST_CASE("Collision table with some collisions", "[hashtable]")
{
  cto_sptr_hash_t hash;
  tchecker::collision_table_t<cto_sptr_t, cto_sptr_hash_t> t(1024, hash);

  std::size_t const N = 6;
  std::tuple<int, int> values[N] = {{1, 1}, {14, 157}, {1, 13}, {1, 89}, {14, 1390}, {78, 3}};

  cto_sptr_t o[N];
  for (std::size_t i = 0; i < N; ++i) {
    o[i] = shared_cto_t::allocate_and_construct(values[i]);
    t.add(o[i]);
  }

  // Sanity check
  for (std::size_t i = 0; i < N; ++i)
    for (std::size_t j = 0; j < N; ++j)
      if (std::get<0>(values[i]) == std::get<0>(values[j]))
        REQUIRE(hash(o[i]) == hash(o[j]));
      else
        REQUIRE(hash(o[i]) != hash(o[j]));

  SECTION("Collision table has expected size") { REQUIRE(t.size() == N); }

  SECTION("The range of objects in the table contains exactly the objects in o")
  {
    auto r = t.range();
    REQUIRE(std::distance(r.begin(), r.end()) == N);

    bool found[N];
    for (cto_sptr_t p : r) {
      for (std::size_t i = 0; i < N; ++i)
        if (p == o[i])
          found[i] = true;
    }
    for (std::size_t i = 0; i < N; ++i)
      REQUIRE(found[i]);
  }

  SECTION("The range of objects in the collision list of o[0] is exactly o[0], o[2], o[3]")
  {
    auto r = t.collision_range(o[0]);
    REQUIRE(std::distance(r.begin(), r.end()) == 3);

    bool found[N] = {};
    for (cto_sptr_t p : r) {
      for (std::size_t i = 0; i < N; ++i)
        if (p == o[i])
          found[i] = true;
    }
    REQUIRE(N == 6);
    REQUIRE(found[0]);
    REQUIRE_FALSE(found[1]);
    REQUIRE(found[2]);
    REQUIRE(found[3]);
    REQUIRE_FALSE(found[4]);
    REQUIRE_FALSE(found[5]);
  }

  SECTION("The range of objects in the collision list of o[1] is exactly o[1], o[4]")
  {
    auto r = t.collision_range(o[1]);
    REQUIRE(std::distance(r.begin(), r.end()) == 2);

    bool found[N] = {};
    for (cto_sptr_t p : r) {
      for (std::size_t i = 0; i < N; ++i)
        if (p == o[i])
          found[i] = true;
    }
    REQUIRE(N == 6);
    REQUIRE_FALSE(found[0]);
    REQUIRE(found[1]);
    REQUIRE_FALSE(found[2]);
    REQUIRE_FALSE(found[3]);
    REQUIRE(found[4]);
    REQUIRE_FALSE(found[5]);
  }

  SECTION("The range of objects in the collision list of o[2] is exactly o[0], o[2], o[3]")
  {
    auto r = t.collision_range(o[2]);
    REQUIRE(std::distance(r.begin(), r.end()) == 3);

    bool found[N] = {};
    for (cto_sptr_t p : r) {
      for (std::size_t i = 0; i < N; ++i)
        if (p == o[i])
          found[i] = true;
    }
    REQUIRE(N == 6);
    REQUIRE(found[0]);
    REQUIRE_FALSE(found[1]);
    REQUIRE(found[2]);
    REQUIRE(found[3]);
    REQUIRE_FALSE(found[4]);
    REQUIRE_FALSE(found[5]);
  }

  SECTION("The range of objects in the collision list of o[3] is exactly o[0], o[2], o[3]")
  {
    auto r = t.collision_range(o[3]);
    REQUIRE(std::distance(r.begin(), r.end()) == 3);

    bool found[N] = {};
    for (cto_sptr_t p : r) {
      for (std::size_t i = 0; i < N; ++i)
        if (p == o[i])
          found[i] = true;
    }
    REQUIRE(N == 6);
    REQUIRE(found[0]);
    REQUIRE_FALSE(found[1]);
    REQUIRE(found[2]);
    REQUIRE(found[3]);
    REQUIRE_FALSE(found[4]);
    REQUIRE_FALSE(found[5]);
  }

  SECTION("The range of objects in the collision list of o[4] is exactly o[1], o[4]")
  {
    auto r = t.collision_range(o[4]);
    REQUIRE(std::distance(r.begin(), r.end()) == 2);

    bool found[N] = {};
    for (cto_sptr_t p : r) {
      for (std::size_t i = 0; i < N; ++i)
        if (p == o[i])
          found[i] = true;
    }
    REQUIRE(N == 6);
    REQUIRE_FALSE(found[0]);
    REQUIRE(found[1]);
    REQUIRE_FALSE(found[2]);
    REQUIRE_FALSE(found[3]);
    REQUIRE(found[4]);
    REQUIRE_FALSE(found[5]);
  }

  SECTION("The range of objects in the collision list of o[5] is exactly o[5]")
  {
    auto r = t.collision_range(o[5]);
    REQUIRE(std::distance(r.begin(), r.end()) == 1);

    bool found[N] = {};
    for (cto_sptr_t p : r) {
      for (std::size_t i = 0; i < N; ++i)
        if (p == o[i])
          found[i] = true;
    }
    REQUIRE(N == 6);
    REQUIRE_FALSE(found[0]);
    REQUIRE_FALSE(found[1]);
    REQUIRE_FALSE(found[2]);
    REQUIRE_FALSE(found[3]);
    REQUIRE_FALSE(found[4]);
    REQUIRE(found[5]);
  }

  SECTION("Removing an object from the collision table")
  {
    t.remove(o[2]);
    REQUIRE(t.size() == N - 1);

    auto r = t.range();
    REQUIRE(std::distance(r.begin(), r.end()) == N - 1);

    bool found[N] = {};
    for (cto_sptr_t p : r) {
      for (std::size_t i = 0; i < N; ++i)
        if (p == o[i])
          found[i] = true;
    }
    REQUIRE(N == 6);
    REQUIRE(found[0]);
    REQUIRE(found[1]);
    REQUIRE_FALSE(found[2]);
    REQUIRE(found[3]);
    REQUIRE(found[4]);
    REQUIRE(found[5]);
  }

  t.clear();
  for (std::size_t i = 0; i < N; ++i) {
    shared_cto_t * p = o[i].ptr();
    o[i] = nullptr;
    shared_cto_t::destruct_and_deallocate(p);
  }
}

// Object for testing hashtable
class hto_t : public tchecker::hashtable_object_t {
public:
  hto_t(int x) : _x(x) {}
  int x() const { return _x; }

private:
  int _x;
};

std::size_t hash(hto_t const & o) { return static_cast<std::size_t>(o.x()); }

bool operator==(hto_t const & o1, hto_t const & o2) { return o1.x() == o2.x(); }

namespace tchecker {
template <> class allocation_size_t<hto_t> {
public:
  static constexpr std::size_t alloc_size() { return sizeof(hto_t); }

  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... args) { return sizeof(hto_t); }
};
} // namespace tchecker

using shared_hto_t = tchecker::make_shared_t<hto_t>;

using hto_sptr_t = tchecker::intrusive_shared_ptr_t<shared_hto_t>;

class hto_sptr_hash_t {
public:
  std::size_t operator()(hto_sptr_t const & p) const { return hash(*p); }
};

class hto_sptr_equal_t {
public:
  bool operator()(hto_sptr_t const & p1, hto_sptr_t const & p2) const { return (*p1 == *p2); }
};

TEST_CASE("Empty hashtable", "[hashtable]")
{
  tchecker::hashtable_t<hto_sptr_t, hto_sptr_hash_t, hto_sptr_equal_t> t(1024, hto_sptr_hash_t{}, hto_sptr_equal_t{});

  SECTION("Empty hashtable has size 0") { REQUIRE(t.size() == 0); }
}

TEST_CASE("Non-empty hashtable", "[hashtable]")
{
  hto_sptr_hash_t hash;
  hto_sptr_equal_t equal;
  tchecker::hashtable_t<hto_sptr_t, hto_sptr_hash_t, hto_sptr_equal_t> t(1024, hash, equal);

  hto_sptr_t o1{shared_hto_t::allocate_and_construct(1)};
  hto_sptr_t o1b{shared_hto_t::allocate_and_construct(1)};
  hto_sptr_t o2{shared_hto_t::allocate_and_construct(14)};

  REQUIRE(hash(o1) != hash(o2));
  REQUIRE(hash(o1) == hash(o1b));
  REQUIRE_FALSE(equal(o1, o2));
  REQUIRE(equal(o1, o1b));

  SECTION("Adding two distinct objects to the hashtable")
  {
    bool added1 = t.add(o1);
    REQUIRE(added1);
    bool added2 = t.add(o2);
    REQUIRE(added2);
    REQUIRE(t.size() == 2);
  }

  SECTION("Adding two identical objects to the hashtable")
  {
    bool added1 = t.add(o1);
    REQUIRE(added1);
    bool added1b = t.add(o1b);
    REQUIRE_FALSE(added1b);
    REQUIRE(t.size() == 1);
  }

  SECTION("Adding identical and distinct objects to the hashtable")
  {
    bool added1 = t.add(o1);
    REQUIRE(added1);
    bool added2 = t.add(o2);
    REQUIRE(added2);
    bool added1b = t.add(o1b);
    REQUIRE_FALSE(added1b);
    REQUIRE(t.size() == 2);
  }

  SECTION("Finding object in the hashtable")
  {
    t.add(o1);

    auto && [found1, p1] = t.find(o1);
    REQUIRE(found1);
    REQUIRE(p1 == o1);

    auto && [found1b, p1b] = t.find(o1b);
    REQUIRE(found1b);
    REQUIRE(p1b == o1);

    auto && [found2, p2] = t.find(o2);
    REQUIRE_FALSE(found2);
    REQUIRE(p2 == o2);
  }

  SECTION("Adding to the hashtable if not found")
  {
    hto_sptr_t o1_in_ht = t.find_else_add(o1);
    REQUIRE(o1_in_ht == o1);

    hto_sptr_t o1b_in_ht = t.find_else_add(o1b);
    REQUIRE(o1b_in_ht != o1b);
    REQUIRE(o1b_in_ht == o1);

    hto_sptr_t o2_in_ht = t.find_else_add(o2);
    REQUIRE(o2_in_ht == o2);
  }

  t.clear();
  shared_hto_t *p1 = o1.ptr(), *p1b = o1b.ptr(), *p2 = o2.ptr();
  o1 = nullptr;
  o1b = nullptr;
  o2 = nullptr;
  shared_hto_t::destruct_and_deallocate(p1);
  shared_hto_t::destruct_and_deallocate(p1b);
  shared_hto_t::destruct_and_deallocate(p2);
}
