/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/graph/output.hh"

namespace tchecker {

namespace graph {

std::ostream & dot_output_header(std::ostream & os, std::string const & name)
{
  return os << "digraph " << name << " {" << std::endl;
}

std::ostream & dot_output_footer(std::ostream & os) { return os << "}" << std::endl; }

static std::ostream & dot_output_attributes(std::ostream & os, std::map<std::string, std::string> const & attr)
{
  os << "[";
  for (auto it = attr.begin(); it != attr.end(); ++it) {
    if (it != attr.begin())
      os << ", ";
    auto && [key, value] = *it;
    os << key << "=\"" << value << "\"";
  }
  os << "]";
  return os;
}

std::ostream & dot_output_node(std::ostream & os, std::string const & name, std::map<std::string, std::string> const & attr)
{
  os << "  " << name << " ";
  dot_output_attributes(os, attr);
  os << std::endl;
  return os;
}

std::ostream & dot_output_edge(std::ostream & os, std::string const & src, std::string const & tgt,
                               std::map<std::string, std::string> const & attr)
{
  os << "  " << src << " -> " << tgt << " ";
  dot_output_attributes(os, attr);
  os << std::endl;
  return os;
}

} // end of namespace graph

} // end of namespace tchecker