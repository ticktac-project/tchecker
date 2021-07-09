/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <vector>

#include "tchecker/waiting/queue.hh"
#include "tchecker/waiting/stack.hh"
#include "tchecker/waiting/waiting.hh"

/*!
 \class int_element_t
 \brief int storing element compatible with fast remove waiting containers
*/
class int_element_t : public tchecker::waiting::element_t {
public:
  int_element_t(int x) : _x(x) {}
  int x() const { return _x; }

private:
  int _x;
};

TEST_CASE("waiting queue", "[waiting]")
{
  tchecker::waiting::queue_t<int> empty_queue, non_empty_queue;
  non_empty_queue.insert(1);
  non_empty_queue.insert(2367);
  non_empty_queue.insert(47);

  SECTION("empty")
  {
    REQUIRE(empty_queue.empty());
    REQUIRE_FALSE(non_empty_queue.empty());
  }

  SECTION("insert in empty queue")
  {
    empty_queue.insert(2);
    REQUIRE_FALSE(empty_queue.empty());
    REQUIRE(empty_queue.first() == 2);
    empty_queue.remove_first();
    REQUIRE(empty_queue.empty());
  }

  SECTION("insert in non empty queue")
  {
    non_empty_queue.insert(89);
    REQUIRE_FALSE(non_empty_queue.empty());
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == 89);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("clear an empty queue")
  {
    empty_queue.clear();
    REQUIRE(empty_queue.empty());
  }

  SECTION("clear a non-empty queue")
  {
    non_empty_queue.clear();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("remove first element")
  {
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.first() == 2367);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.first() == 47);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("access to first element") { REQUIRE(non_empty_queue.first() == 1); }

  SECTION("remove head")
  {
    non_empty_queue.remove(1);
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == 2367);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == 47);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("remove middle")
  {
    non_empty_queue.remove(2367);
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == 1);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == 47);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("remove last")
  {
    non_empty_queue.remove(47);
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == 1);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == 2367);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("remove multiple")
  {
    non_empty_queue.insert(2367);
    non_empty_queue.remove(2367);
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == 1);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == 47);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("remove empty queue")
  {
    empty_queue.remove(2);
    REQUIRE(empty_queue.empty());
  }

  SECTION("remove element not in queue")
  {
    non_empty_queue.remove(0);
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == 1);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == 2367);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == 47);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }
}

TEST_CASE("fast remove waiting queue", "[waiting]")
{
  using int_sptr_t = std::shared_ptr<int_element_t>;

  std::vector<int_sptr_t> v;
  v.emplace_back(new int_element_t{12});
  v.emplace_back(new int_element_t{3});
  v.emplace_back(new int_element_t{8923});
  v.emplace_back(new int_element_t{12});

  tchecker::waiting::fast_remove_queue_t<int_sptr_t> empty_queue, non_empty_queue;
  for (int_sptr_t const & p : v)
    non_empty_queue.insert(p);

  SECTION("empty")
  {
    REQUIRE(empty_queue.empty());
    REQUIRE_FALSE(non_empty_queue.empty());
  }

  SECTION("insert in empty queue")
  {
    int_sptr_t x{new int_element_t{290}};
    empty_queue.insert(x);
    REQUIRE_FALSE(empty_queue.empty());
    REQUIRE(empty_queue.first() == x);
    empty_queue.remove_first();
    REQUIRE(empty_queue.empty());
  }

  SECTION("insert in non empty queue")
  {
    int_sptr_t x{new int_element_t{45}};
    non_empty_queue.insert(x);
    REQUIRE_FALSE(non_empty_queue.empty());
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first() == x);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("clear an empty queue")
  {
    empty_queue.clear();
    REQUIRE(empty_queue.empty());
  }

  SECTION("clear a non-empty queue")
  {
    non_empty_queue.clear();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("remove first element")
  {
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.first()->x() == 3);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.first()->x() == 8923);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.first()->x() == 12);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("access to first element") { REQUIRE(non_empty_queue.first()->x() == 12); }

  SECTION("remove head")
  {
    non_empty_queue.remove(v[0]);
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 3);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 8923);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 12);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("remove middle")
  {
    non_empty_queue.remove(v[1]);
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 12);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 8923);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 12);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("remove last")
  {
    non_empty_queue.remove(v[3]);
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 12);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 3);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 8923);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("remove multiple")
  {
    non_empty_queue.insert(v[2]);
    non_empty_queue.remove(v[2]);
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 12);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 3);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 12);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }

  SECTION("remove empty queue")
  {
    int_sptr_t x{new int_element_t{128}};
    empty_queue.remove(x);
    REQUIRE(empty_queue.empty());
  }

  SECTION("remove element not in queue")
  {
    int_sptr_t x{new int_element_t{1298}};
    non_empty_queue.remove(x);
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 12);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 3);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 8923);
    non_empty_queue.remove_first();
    REQUIRE_FALSE(non_empty_queue.empty());
    REQUIRE(non_empty_queue.first()->x() == 12);
    non_empty_queue.remove_first();
    REQUIRE(non_empty_queue.empty());
  }
}

TEST_CASE("waiting stack", "[waiting]")
{
  tchecker::waiting::stack_t<int> empty_stack, non_empty_stack;
  non_empty_stack.insert(27);
  non_empty_stack.insert(4);
  non_empty_stack.insert(198);
  non_empty_stack.insert(1567);

  SECTION("empty")
  {
    REQUIRE(empty_stack.empty());
    REQUIRE_FALSE(non_empty_stack.empty());
  }

  SECTION("insert in empty stack")
  {
    empty_stack.insert(16);
    REQUIRE_FALSE(empty_stack.empty());
    REQUIRE(empty_stack.first() == 16);
    empty_stack.remove_first();
    REQUIRE(empty_stack.empty());
  }

  SECTION("insert in non empty stack")
  {
    non_empty_stack.insert(89);
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 89);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("clear an empty stack")
  {
    empty_stack.clear();
    REQUIRE(empty_stack.empty());
  }

  SECTION("clear a non-empty stack")
  {
    non_empty_stack.clear();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("remove first element")
  {
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.first() == 198);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.first() == 4);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.first() == 27);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("access to first element") { REQUIRE(non_empty_stack.first() == 1567); }

  SECTION("remove head")
  {
    non_empty_stack.remove(1567);
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 198);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 4);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 27);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("remove middle")
  {
    non_empty_stack.remove(4);
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 1567);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 198);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 27);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("remove last")
  {
    non_empty_stack.remove(27);
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 1567);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 198);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 4);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("remove multiple")
  {
    non_empty_stack.insert(198);
    non_empty_stack.insert(4);
    non_empty_stack.remove(198);
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 4);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 1567);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 4);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 27);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("remove empty stack")
  {
    empty_stack.remove(1289);
    REQUIRE(empty_stack.empty());
  }

  SECTION("remove element not in stack")
  {
    non_empty_stack.remove(0);
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 1567);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 198);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 4);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == 27);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }
}

TEST_CASE("fast remove waiting stack", "[waiting]")
{
  using int_sptr_t = std::shared_ptr<int_element_t>;

  std::vector<int_sptr_t> v;
  v.emplace_back(new int_element_t{15});
  v.emplace_back(new int_element_t{0});
  v.emplace_back(new int_element_t{6});
  v.emplace_back(new int_element_t{178});

  tchecker::waiting::fast_remove_stack_t<int_sptr_t> empty_stack, non_empty_stack;
  for (int_sptr_t const & p : v)
    non_empty_stack.insert(p);

  SECTION("empty")
  {
    REQUIRE(empty_stack.empty());
    REQUIRE_FALSE(non_empty_stack.empty());
  }

  SECTION("insert in empty stack")
  {
    int_sptr_t x{new int_element_t{29}};
    empty_stack.insert(x);
    REQUIRE_FALSE(empty_stack.empty());
    REQUIRE(empty_stack.first() == x);
    empty_stack.remove_first();
    REQUIRE(empty_stack.empty());
  }

  SECTION("insert in non empty stack")
  {
    int_sptr_t x{new int_element_t{33}};
    non_empty_stack.insert(x);
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first() == x);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("clear an empty stack")
  {
    empty_stack.clear();
    REQUIRE(empty_stack.empty());
  }

  SECTION("clear a non-empty stack")
  {
    non_empty_stack.clear();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("remove first element")
  {
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.first()->x() == 6);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.first()->x() == 0);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.first()->x() == 15);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("access to first element") { REQUIRE(non_empty_stack.first()->x() == 178); }

  SECTION("remove head")
  {
    non_empty_stack.remove(v[3]);
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 6);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 0);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 15);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("remove middle")
  {
    non_empty_stack.remove(v[2]);
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 178);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 0);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 15);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("remove last")
  {
    non_empty_stack.remove(v[0]);
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 178);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 6);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 0);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("remove multiple")
  {
    non_empty_stack.insert(v[1]);
    non_empty_stack.insert(v[0]);
    non_empty_stack.insert(v[1]);
    non_empty_stack.remove(v[1]);
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 15);
    non_empty_stack.remove_first(); // NB: removes 1st occ of v[0] as well
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 178);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 6);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }

  SECTION("remove empty stack")
  {
    int_sptr_t x{new int_element_t{3187}};
    empty_stack.remove(x);
    REQUIRE(empty_stack.empty());
  }

  SECTION("remove element not in stack")
  {
    int_sptr_t x{new int_element_t{12908}};
    non_empty_stack.remove(x);
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 178);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 6);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 0);
    non_empty_stack.remove_first();
    REQUIRE_FALSE(non_empty_stack.empty());
    REQUIRE(non_empty_stack.first()->x() == 15);
    non_empty_stack.remove_first();
    REQUIRE(non_empty_stack.empty());
  }
}