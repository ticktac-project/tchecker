/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cstdio>
#include <string>

#include "tchecker/parsing/declaration.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/utils/log.hh"

#include "utils.hh"

namespace tchecker {
  
  namespace test {
    
    tchecker::parsing::system_declaration_t const * parse(std::string const & model, tchecker::log_t & log)
    {
      // Create the temporary file from model
      std::FILE * f = tmpfile();
      if (f == nullptr)
        return nullptr;
      
      std::fputs(model.c_str(), f);
      std::fseek(f, 0, SEEK_SET);
      
      // Parse the model from the temporary file
      tchecker::parsing::system_declaration_t const * sysdecl = nullptr;
      
      try {
        sysdecl = tchecker::parsing::parse_system_declaration(f, "", log);
      }
      catch (...)
      {
        delete sysdecl;
        std::fclose(f);
        return nullptr;
      }
      
      std::fclose(f);
      return sysdecl;
    }
    
  } // end of namespace test
  
} // end of namespace tchecker
