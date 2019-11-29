/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <limits>
#include <vector>

#include "tchecker/vm/vm.hh"

namespace tchecker {
  
  /* bytecode_t */
  
  
  std::ostream & output(std::ostream & os, tchecker::bytecode_t const * bytecode)
  {
    for (bool ret = false; ! ret; ++bytecode) {
      switch (*bytecode) {
        case VM_RET:
          os << "RET";
          ret = true;
          break;
          
        case VM_RETZ:
          os << "RETZ";
          break;
          
        case VM_FAILNOTIN:
          os << "FAILNOTIN";
          ++bytecode;
          os << " " << *bytecode;
          ++bytecode;
          os << " " << *bytecode;
          break;

        case VM_JMP:
          os << "JMP ";
          ++bytecode;
          os << *bytecode;
          break;

        case VM_JMPZ:
          os << "JMPZ ";
          ++bytecode;
          os << *bytecode;
          break;

        case VM_PUSH:
          os << "PUSH";
          ++bytecode;
          os << " " << *bytecode;
          break;
          
        case VM_VALUEAT:
          os << "VALUEAT";
          break;
          
        case VM_ASSIGN:
          os << "ASSIGN";
          break;
          
        case VM_LAND:
          os << "LAND";
          break;
          
        case VM_MINUS:
          os << "MINUS";
          break;
          
        case VM_DIV:
          os << "DIV";
          break;
          
        case VM_EQ:
          os << "EQ";
          break;
          
        case VM_GE:
          os << "GE";
          break;
          
        case VM_GT:
          os << "GT";
          break;
          
        case VM_LT:
          os << "LT";
          break;
          
        case VM_LE:
          os << "LE";
          break;
          
        case VM_MUL:
          os << "MUL";
          break;
          
        case VM_MOD:
          os << "MOD";
          break;
          
        case VM_NE:
          os << "NE";
          break;
          
        case VM_SUM:
          os << "SUM";
          break;
          
        case VM_NEG:
          os << "NEG";
          break;
          
        case VM_LNOT:
          os << "LNOT";
          break;

        case VM_CLKCONSTR:
          os << "CLKCONSTR";
          ++bytecode;
          os << " " << *bytecode;
          break;
          
        case VM_CLKRESET:
          os << "CLKRESET";
          break;
          
        case VM_NOP:
          os << "NOP";
          break;
          
        default:
          throw std::runtime_error("incomplete switch statement");
      }
      os << std::endl;
    }
    
    return os;
  }
  
} // end of namespace tchecker
