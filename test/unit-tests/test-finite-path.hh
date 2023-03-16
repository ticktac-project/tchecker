/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/graph/path.hh"

class path_node_t {
public:
  path_node_t(int id) : id(id) {}
  int id;
};

class path_edge_t {
public:
  path_edge_t(int event) : event(event) {}
  int event;
};

class finite_path_t : public tchecker::graph::finite_path_t<path_node_t, path_edge_t> {
public:
  using tchecker::graph::finite_path_t<path_node_t, path_edge_t>::finite_path_t;
  using tchecker::graph::finite_path_t<path_node_t, path_edge_t>::attributes;

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(path_node_t const & n, std::map<std::string, std::string> & m) const
  {
    m["id"] = std::to_string(n.id);
  }

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(path_edge_t const & e, std::map<std::string, std::string> & m) const
  {
    m["event"] = std::to_string(e.event);
  }
};

using node_sptr_t = finite_path_t::node_sptr_t;
using edge_sptr_t = finite_path_t::edge_sptr_t;

class path_node_le_t {
public:
  bool operator()(node_sptr_t const & n1, node_sptr_t const & n2) const { return n1->id <= n2->id; }
};

class path_edge_le_t {
public:
  bool operator()(edge_sptr_t const & e1, edge_sptr_t const & e2) const { return e1->event <= e2->event; }
};

void dot_output(std::ostream & os, finite_path_t const & p)
{
  tchecker::graph::dot_output<finite_path_t, path_node_le_t, path_edge_le_t>(os, p, "foo");
}

TEST_CASE("Path with a single node", "[finite_path]")
{
  int const id = 0;
  finite_path_t path(id);

  SECTION("Number of nodes") { REQUIRE(path.nodes_count() == 1); }

  SECTION("Last and first accessors") { REQUIRE(path.first() == path.last()); }

  SECTION("Node identifier") { REQUIRE(path.first()->id == id); }

  SECTION("Iterator")
  {
    auto begin = path.begin();
    auto end = path.end();

    REQUIRE(begin != end);
    REQUIRE((*begin)->id == id);
    REQUIRE(++begin == end);
  }

  SECTION("Reverse iterator")
  {
    auto begin = path.begin();
    auto end = path.end();
    REQUIRE(begin != end);
    REQUIRE((*begin)->id == id);
    REQUIRE(++begin == end);
  }
}

TEST_CASE("Path with a single edge", "[finite_path]")
{
  int const init_id = 0;
  finite_path_t path(init_id);
  path.extend_back(1, 1);

  SECTION("Right number of nodes") { REQUIRE(path.nodes_count() == 2); }

  SECTION("First, last")
  {
    REQUIRE(path.first()->id == 0);
    REQUIRE(path.last()->id == 1);
  }

  SECTION("The first node has exactly one successor")
  {
    auto r = path.outgoing_edges(path.first());
    REQUIRE(r.begin() != r.end());
    auto it = r.begin();
    REQUIRE(++it == r.end());
  }

  SECTION("The last node has no successor")
  {
    auto r = path.outgoing_edges(path.last());
    REQUIRE(r.empty());
  }

  SECTION("The last node has exactly one predecessor")
  {
    auto r = path.incoming_edges(path.last());
    REQUIRE(r.begin() != r.end());
    auto it = r.begin();
    REQUIRE(++it == r.end());
  }

  SECTION("The first node has no predecessor")
  {
    auto r = path.incoming_edges(path.first());
    REQUIRE(r.empty());
  }

  SECTION("Iterator")
  {
    int const expected_ids[] = {0, 1};
    int const expected_events[] = {1};
    int i = 0;
    for (auto n : path) {
      REQUIRE(n->id == expected_ids[i]);
      if (n != path.last())
        REQUIRE(path.outgoing_edge(n)->event == expected_events[i]);
      ++i;
    }
  }

  SECTION("Reverse iterator")
  {
    int const expected_ids[] = {0, 1};
    int const expected_events[] = {1};

    int i = 1;
    for (auto it = path.rbegin(); it != path.rend(); ++it) {
      node_sptr_t n = *it;
      REQUIRE(n->id == expected_ids[i]);
      if (n != path.first())
        REQUIRE(path.incoming_edge(n)->event == expected_events[i - 1]);
      --i;
    }
  }
}

TEST_CASE("Path with more than one edge", "[finite_path]")
{
  int const init_id = 0;
  finite_path_t path(init_id);

  int const max_id = 9;

  for (int i = 1; i <= max_id; ++i)
    if (i % 2 == 0)
      path.extend_back(i, i);
    else
      path.extend_front(i, i);

  SECTION("Right number of nodes") { REQUIRE(path.nodes_count() == max_id + 1); }

  SECTION("First, last")
  {
    REQUIRE(path.first()->id == 9);
    REQUIRE(path.last()->id == 8);
  }

  SECTION("Each non-last node has exactly one successor")
  {
    for (auto n = path.first(); n != path.last();) {
      auto r = path.outgoing_edges(n);
      REQUIRE(r.begin() != r.end());
      auto it = r.begin();
      REQUIRE(++it == r.end());

      auto e = path.outgoing_edge(n);
      n = path.edge_tgt(e);
    }
  }

  SECTION("The last node has no successor")
  {
    auto r = path.outgoing_edges(path.last());
    REQUIRE(r.empty());
  }

  SECTION("Each non-first node has exactly one predecessor")
  {
    for (auto n = path.last(); n != path.first();) {
      auto r = path.incoming_edges(n);
      REQUIRE(r.begin() != r.end());
      auto it = r.begin();
      REQUIRE(++it == r.end());

      auto e = path.incoming_edge(n);
      n = path.edge_src(e);
    }
  }

  SECTION("The first node has no predecessor")
  {
    auto r = path.incoming_edges(path.first());
    REQUIRE(r.empty());
  }

  SECTION("Iterator")
  {
    int const expected_ids[] = {9, 7, 5, 3, 1, 0, 2, 4, 6, 8};
    int const expected_events[] = {9, 7, 5, 3, 1, 2, 4, 6, 8};
    int i = 0;
    for (auto n : path) {
      REQUIRE(n->id == expected_ids[i]);
      if (n != path.last())
        REQUIRE(path.outgoing_edge(n)->event == expected_events[i]);
      ++i;
    }
  }

  SECTION("Reverse iterator")
  {
    int const expected_ids[] = {9, 7, 5, 3, 1, 0, 2, 4, 6, 8};
    int const expected_events[] = {9, 7, 5, 3, 1, 2, 4, 6, 8};

    int i = max_id;
    for (auto it = path.rbegin(); it != path.rend(); ++it) {
      node_sptr_t n = *it;
      REQUIRE(n->id == expected_ids[i]);
      if (n != path.first())
        REQUIRE(path.incoming_edge(n)->event == expected_events[i - 1]);
      --i;
    }
  }
}