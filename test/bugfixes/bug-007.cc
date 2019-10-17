#include <cassert>
#include <string>

#include "tchecker/parsing/declaration.hh"
#include "tchecker/utils/log.hh"
#include "utils.hh"

int main(int argc, char **argv)
{
  std::string model= "system:foo\n";

  tchecker::log_t log(&std::cerr);
  tchecker::parsing::system_declaration_t const *
    sysdecl = tchecker::test::parse(model, log);

  
  assert (sysdecl != nullptr);
  std::cout << *sysdecl << std::endl;
  
  delete sysdecl;
}

