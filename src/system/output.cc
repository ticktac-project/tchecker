/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/system/output.hh"

namespace tchecker {

namespace system {

// TChecker output

static std::ostream & operator<<(std::ostream & os, tchecker::system::attributes_t const & attr)
{
  auto r = attr.attributes();
  if (r.begin() == r.end())
    return os;
  os << "{";
  auto begin = r.begin(), end = r.end();
  for (auto it = begin; it != end; ++it) {
    if (it != begin)
      os << " : ";
    os << (*it).first << ":" << (*it).second;
  }
  os << "}";
  return os;
}

void output_tck(std::ostream & os, tchecker::system::system_t const & s)
{
  // System name and attributes
  os << "system:" << s.name() << s.attributes() << std::endl;

  // Events
  tchecker::event_id_t events_count = s.events_count();
  for (tchecker::event_id_t id = 0; id < events_count; ++id)
    os << "event:" << s.event_name(id) << s.event_attributes(id) << std::endl;

  // Bounded integer variables
  tchecker::integer_variables_t const & integer_variables = s.integer_variables();
  tchecker::intvar_id_t intvars_count = s.intvars_count(tchecker::VK_DECLARED);
  for (tchecker::intvar_id_t id = 0; id < intvars_count; ++id) {
    tchecker::intvar_info_t const & info = integer_variables.info(id);
    os << "int:" << info.size() << ":" << info.min() << ":" << info.max() << ":";
    os << info.initial_value() << ":";
    os << integer_variables.name(id) << s.intvar_attributes(id) << std::endl;
  }

  // Clocks
  tchecker::clock_variables_t const & clock_variables = s.clock_variables();
  tchecker::clock_id_t clocks_count = s.clocks_count(tchecker::VK_DECLARED);
  for (tchecker::clock_id_t id = 0; id < clocks_count; ++id) {
    os << "clock:" << clock_variables.info(id).size() << ":" << clock_variables.name(id);
    os << s.clock_attributes(id) << std::endl;
  }

  // Processes
  tchecker::process_id_t processes_count = s.processes_count();
  for (tchecker::process_id_t id = 0; id < processes_count; ++id)
    os << "process:" << s.process_name(id) << s.process_attributes(id) << std::endl;

  // Locations
  for (tchecker::system::loc_const_shared_ptr_t loc : s.locations())
    os << "location:" << s.process_name(loc->pid()) << ":" << loc->name() << loc->attributes() << std::endl;

  // Edges
  for (tchecker::system::edge_const_shared_ptr_t edge : s.edges()) {
    os << "edge:" << s.process_name(edge->pid()) << ":" << s.location(edge->src())->name() << ":";
    os << s.location(edge->tgt())->name() << ":" << s.event_name(edge->event_id());
    os << edge->attributes() << std::endl;
  }

  // Synchronizations
  for (tchecker::system::synchronization_t const & sync : s.synchronizations()) {
    os << "sync";
    for (tchecker::system::sync_constraint_t const & constr : sync.synchronization_constraints()) {
      os << ":" << s.process_name(constr.pid()) << "@" << s.event_name(constr.event_id());
      if (constr.strength() == tchecker::SYNC_WEAK)
        os << "?";
    }
    os << sync.attributes() << std::endl;
  }
}

// dot graphviz output

static void output_dot(std::ostream & os, tchecker::system::attributes_t const & attr)
{
  auto r = attr.attributes();
  if (r.begin() == r.end())
    return;
  os << "[";
  auto begin = r.begin(), end = r.end();
  for (auto it = begin; it != end; ++it) {
    if (it != begin)
      os << ", ";
    os << (*it).first << "=\"" << (*it).second << "\"";
  }
  os << "]";
}

static std::string dot_node_name(std::string const & process_name, std::string const & loc_name, std::string const & delimiter)
{
  return process_name + delimiter + loc_name;
}

static void output_dot(std::ostream & os, tchecker::system::loc_const_shared_ptr_t & loc, tchecker::system::system_t const & s,
                       std::string const & delimiter)
{
  os << dot_node_name(s.process_name(loc->pid()), loc->name(), delimiter);
  tchecker::system::attributes_t attr{loc->attributes()};
  if (!attr.values("label").empty())
    throw std::runtime_error("location already has a \"label\" attribute");
  attr.add_attribute("label", loc->name());
  output_dot(os, attr);
}

static void output_dot(std::ostream & os, tchecker::system::edge_const_shared_ptr_t & edge,
                       tchecker::system::system_t const & s, std::string const & delimiter)
{
  std::string pname = s.process_name(edge->pid());
  std::string src_name = s.location(edge->src())->name();
  std::string tgt_name = s.location(edge->tgt())->name();
  os << dot_node_name(pname, src_name, delimiter) << " -> " << dot_node_name(pname, tgt_name, delimiter);
  tchecker::system::attributes_t attr{edge->attributes()};
  if (!attr.values("label").empty())
    throw std::runtime_error("edge already has a \"label\" attribute");
  attr.add_attribute("label", s.event_name(edge->event_id()));
  output_dot(os, attr);
}

static void output_dot(std::ostream & os, tchecker::system::synchronization_t const & sync,
                       tchecker::system::system_t const & s)
{
  os << "<";
  auto r = sync.synchronization_constraints();
  for (auto it = r.begin(); it != r.end(); ++it) {
    if (it != r.begin())
      os << ",";
    tchecker::system::sync_constraint_t const & constr = *it;
    os << s.process_name(constr.pid()) << "@" << s.event_name(constr.event_id());
    if (constr.strength() == tchecker::SYNC_WEAK)
      os << "?";
  }
  os << ">";
}

void output_dot(std::ostream & os, tchecker::system::system_t const & s, std::string const & delimiter)
{
  os << "digraph " << s.name() << " {" << std::endl;
  // Output each process as a cluster
  tchecker::process_id_t processes_count = s.processes_count();
  for (tchecker::process_id_t pid = 0; pid < processes_count; ++pid) {
    os << "  subgraph cluster_" << s.process_name(pid) << " {" << std::endl;
    os << "    label=\"" << s.process_name(pid) << "\";" << std::endl;
    // Locations
    for (tchecker::system::loc_const_shared_ptr_t loc : s.locations()) {
      if (loc->pid() != pid)
        continue;
      os << "    ";
      output_dot(os, loc, s, delimiter);
      os << ";" << std::endl;
    }
    // Edges
    for (tchecker::system::edge_const_shared_ptr_t edge : s.edges()) {
      if (edge->pid() != pid)
        continue;
      os << "    ";
      output_dot(os, edge, s, delimiter);
      os << ";" << std::endl;
    }
    os << "  }" << std::endl;
  }
  // Output synchronizations as a cluster
  os << "  subgraph cluster_synchronizations {" << std::endl;
  os << "    label=\"Synchronizations\";" << std::endl;
  os << "    node[shape=\"none\"];" << std::endl;
  os << "    \"" << std::endl;
  for (tchecker::system::synchronization_t const & sync : s.synchronizations()) {
    os << "    ";
    output_dot(os, sync, s);
    os << std::endl;
  }
  os << "    \";" << std::endl;
  os << "  }" << std::endl;
  os << "}" << std::endl;
}

//
// JSON pretty-printer
//
static std::string const json_tab = "  ";

static void json_output_attributes(std::ostream & os,
                                   tchecker::system::attributes_t const & attr,
                                   std::string const t0)
{
  std::string const t1 = t0+json_tab;
  auto r = attr.attributes();
  os << t0 << "\"attributes\" : {" << std::endl;
  if (r.begin() != r.end()) {
    auto begin = r.begin(), end = r.end();
    for (auto it = begin; it != end; ++it) {
      if (it != begin)
        os << "," << std::endl;
      os << t1 << "\"" << (*it).first << "\" : \"" << (*it).second << "\"";
    }
    os << t1 << std::endl;
  }
  os << t0 << "}" << std::endl;
}

static void json_output_location(std::ostream & os,
                                 tchecker::system::system_t const & s,
                                 std::string const & delimiter,
                                 tchecker::process_id_t pid,
                                 tchecker::system::loc_const_shared_ptr_t & loc,
                                 bool first,
                                 std::string const t0)
{
  std::string const t1 = t0+json_tab;
  std::string const locid = dot_node_name(s.process_name(loc->pid()), loc->name(), delimiter);

  if (! first)
    os << "," << std::endl;
  os << t0 << "{" << std::endl
      << t1 << "\"id\" : \"" << locid << "\"," << std::endl;
  tchecker::system::attributes_t attr{loc->attributes()};
  if (!attr.values("label").empty())
    throw std::runtime_error("location already has a \"label\" attribute");
  attr.add_attribute("label", loc->name());
  json_output_attributes(os, attr, t1);
  os << t0 << "}";
}

static void json_output_edge(std::ostream & os,
                             tchecker::system::system_t const & s,
                             std::string const & delimiter,
                             tchecker::process_id_t pid,
                             tchecker::system::edge_const_shared_ptr_t & edge,
                             bool first,
                             std::string const t0)

{
  std::string t1 = t0+json_tab;
  std::string pname = s.process_name(edge->pid());
  std::string src_name = s.location(edge->src())->name();
  std::string tgt_name = s.location(edge->tgt())->name();
  if (!first)
    os << "," << std::endl;
  os << t0 << "{" << std::endl
     << t1 << "\"src\" : \"" << dot_node_name(pname, src_name, delimiter) << "\"," << std::endl
     << t1 << "\"tgt\" : \"" << dot_node_name(pname, tgt_name, delimiter) << "\"," << std::endl;
  tchecker::system::attributes_t attr{edge->attributes()};
  if (!attr.values("label").empty())
    throw std::runtime_error("edge already has a \"label\" attribute");
  attr.add_attribute("label", s.event_name(edge->event_id()));
  json_output_attributes(os, attr, t1);
  os << t0 << "}"; // end of edge
}

static void json_output_process(std::ostream & os,
                                tchecker::system::system_t const & s,
                                std::string const & delimiter,
                                tchecker::process_id_t pid,
                                std::string const t0)
{
  std::string const t1 = t0+json_tab;
  os << t0 << "{" << std::endl
     << t1 << "\"pid\" : \"" << s.process_name(pid) << "\"," << std::endl
     << t1 << "\"locations\" : [" << std::endl;
  // Locations
  bool first = true;
  for (tchecker::system::loc_const_shared_ptr_t loc : s.locations()) {
    if (loc->pid() != pid)
      continue;
    json_output_location(os, s, delimiter, pid, loc, first, t1 + json_tab);
    first = false;
  }
  if (! first)
    os << std::endl;
  os << t1 << "]," << std::endl; // end of locations

  os<< t1 << "\"edges\" : [" << std::endl;
  first = true;
  for (tchecker::system::edge_const_shared_ptr_t edge : s.edges()) {
    if (edge->pid() != pid)
      continue;
    json_output_edge(os, s, delimiter, pid, edge, first, t1 + json_tab);
    first = false;
  }
  if (! first)
    os << std::endl;
  os << t1 << "]" << std::endl; //end of edges
  os << t0 << "}"; // end of process
}

static void json_output_sync(std::ostream & os,
                             tchecker::system::synchronization_t const & sync,
                             tchecker::system::system_t const & s,
                             std::string const t0,
                             bool first)
{
  std::string const t1 = t0+json_tab;
  if (!first)
    os << "," << std::endl;

  os << t1 << "[ ";
  auto r = sync.synchronization_constraints();
  for (auto it = r.begin(); it != r.end(); ++it) {
    if (it != r.begin())
      os << ", ";
    tchecker::system::sync_constraint_t const & constr = *it;
    os << "\"" << s.process_name(constr.pid()) << "@" << s.event_name(constr.event_id());
    if (constr.strength() == tchecker::SYNC_WEAK)
      os << "?";
    os << "\"";
  }
  os << " ]";
}

void output_json(std::ostream & os, tchecker::system::system_t const & s, std::string const & delimiter)
{
  std::string const t1 = json_tab;
  os << "{" << std::endl
     << t1 << "\"name\" : \"" << s.name() << "\"," << std::endl
     << t1 << "\"processes\" : [" << std::endl;

  tchecker::process_id_t processes_count = s.processes_count();
  for (tchecker::process_id_t pid = 0; pid < processes_count; ++pid) {
    json_output_process(os, s, delimiter, pid, t1 + json_tab);
    if (pid + 1 < processes_count)
      os << ",";
    os << std::endl;
  }

  os << t1 << "]," << std::endl
     << t1 << "\"sync\" : [" << std::endl;
  bool first = true;
  for (tchecker::system::synchronization_t const & sync : s.synchronizations()) {
    json_output_sync(os, sync, s, t1 + json_tab, first);
    first = false;
  }
  if (! first)
    os << std::endl;
  os << t1 << "]" << std::endl
     << "}" << std::endl;
}


} // end of namespace system

} // end of namespace tchecker
