#include <cassert>
#include <string>

#include "tchecker/parsing/declaration.hh"
#include "utils.hh"

int main(int /*argc*/, char ** /*argv*/)
{
  std::string model = "system:foo\n";

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{tchecker::test::parse(model)};

  assert(sysdecl != nullptr);
  std::cout << *sysdecl << std::endl;

  return 0;
}
