/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VM_HH
#define TCHECKER_VM_HH

#include <cassert>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"


/*!
 \file vm.hh
 \brief Virtual machine for tchecker bytecode
 */

namespace tchecker {
  
  /*!
   \brief Bytecode instruction set
   */
  enum instruction_t {
    // assume stack = v1 ... vK and top symbol is vK
    VM_RET = 0,      // end of operation, return vK
    VM_RETZ,         // end of operation when vK==0, return vK
    VM_FAILNOTIN,    // raise exception when (l <= vK <= h) does not hold, for parameters l and h of VM_FAILNOTIN
    //
    VM_JMP,          // unconditional jump relatively to next instruction;
    //                  offset is a parameter of the instruction
    VM_JMPZ,         // stack = v1 ... vK   jump if vK == 0
    //                  offset is a parameter of the instruction
    VM_PUSH,         // stack = v1 ... vK v                   where v is a parameter of VM_PUSH
    //
    VM_VALUEAT,      // stack = v1 ... vK-1 [vK]              vK replaced by value at addr vK
    VM_ASSIGN,       // stack = v1 ... vK-2                   [vK-1] = vK, i.e. value at address vK-1 is replaced by vK
    //
    VM_LAND,         // stack = v1 ... vK-2 (vK-1 && vK)
    VM_MINUS,        // stack = v1 ... vK-2 (vK-1 - vK)
    VM_DIV,          // stack = v1 ... vK-2 (vK-1 / vK)
    VM_EQ,           // stack = v1 ... vK-2 (vK-1 == vK)
    VM_GE,           // stack = v1 ... vK-2 (vK-1 >= vK)
    VM_GT,           // stack = v1 ... vK-2 (vK-1 > vK)
    VM_LT,           // stack = v1 ... vK-2 (vK-1 < vK)
    VM_LE,           // stack = v1 ... vK-2 (vK-1 <= vK)
    VM_MUL,          // stack = v1 ... vK-2 (vK-1 * vK)
    VM_MOD,          // stack = v1 ... vK-2 (vK-1 % vK)
    VM_NE,           // stack = v1 ... vK-2 (vK-1 != vK)
    VM_SUM,          // stack = v1 ... vK-2 (vK-1 + vK)
    VM_NEG,          // stack = v1 ... vK-1 (- vK)
    VM_LNOT,         // stack = v1 ... vK-1 (! vK)
    //
    VM_CLKCONSTR,    // stack = v1 ... vK-4 1                  output (vK-2 vK-1 s vK) where
    //                                                         s is a parameter of VM_CLKCONSTR (strictness)
    VM_CLKRESET,     // stack = v1 ... vK-3                    output (vK-2 vK-1 vK)
    //
    VM_PUSH_FRAME,   // push a new frame for local variables
    VM_POP_FRAME,    // pop the top-level frame
    VM_VALUEAT_FRAME,// stack = v1 ... vK-1 [vK] 
                     // vK is replaced by the value of the local variable identified by vK.
    VM_ASSIGN_FRAME, // stack = v1 ... vK-2 
                     // [vK-1] is assigned vK where vK-1 identifies a local variables.
    VM_INIT_FRAME,   // stack = v1 ... vK-2
                     // [vK-1] is initialized with vK where vK-1 identifies a local variables.
    VM_NOP,          // SHOULD BE LAST INSTRUCTION
  };
  
  
  
  
  /*!
   \brief Type of bytecode
   */
  using bytecode_t = std::int64_t;
  
  
  static_assert((tchecker::VM_NOP >= std::numeric_limits<tchecker::bytecode_t>::min()) &&
                (tchecker::VM_NOP <= std::numeric_limits<tchecker::bytecode_t>::max()),
                "tchecker::bytecode_t should contain enum tchecker::instruction_t");
  
  
  static_assert((std::numeric_limits<tchecker::integer_t>::min() >= std::numeric_limits<tchecker::bytecode_t>::min()) &&
                (std::numeric_limits<tchecker::integer_t>::max() <= std::numeric_limits<tchecker::bytecode_t>::max()),
                "tchecker::bytecode_t should contain tchecker::integer_t");
  
  
  static_assert((std::numeric_limits<unsigned int>::min() >= std::numeric_limits<tchecker::bytecode_t>::min()) &&
                (std::numeric_limits<unsigned int>::max() <= std::numeric_limits<tchecker::bytecode_t>::max()),
                "tchecker::bytecode_t should contain unsigned int");
  
  
  static_assert((std::numeric_limits<tchecker::intvar_id_t>::min() >= std::numeric_limits<tchecker::bytecode_t>::min()) &&
                (std::numeric_limits<tchecker::intvar_id_t>::max() <= std::numeric_limits<tchecker::bytecode_t>::max()),
                "tchecker::bytecode_t should contain tchecker::intvar_id_t");
  
  
  static_assert((std::numeric_limits<tchecker::clock_id_t>::min() >= std::numeric_limits<tchecker::bytecode_t>::min()) &&
                (std::numeric_limits<tchecker::clock_id_t>::max() <= std::numeric_limits<tchecker::bytecode_t>::max()),
                "tchecker::bytecode_t should contain tchecker::clock_id_t");
  
  
  /*!
   \brief Output
   \param os : output stream
   \param bytecode : sequence of bytecode intructions
   \pre bytecode is null-terminated (i.e. RET terminated), and well-formed
   (i.e. instructions have the expected parameters)
   \post the sequence of instructions from bytecode to null has been output to
   os
   \return os after output
   */
  std::ostream & output(std::ostream & os, tchecker::bytecode_t const * bytecode);
  
  /*!
   \brief Output
   \param os : output stream
   \param bytecode : sequence of bytecode intructions
   \pre bytecode is null-terminated (i.e. RET terminated), and well-formed
   (i.e. instructions have the expected parameters)
   \post the instruction pointed by bytecode has been output to os
   \return the number of bytes of the instruction
   */
  std::size_t output_instruction(std::ostream & os, tchecker::bytecode_t const *bytecode);


  
  
  // Virtual machine (VM)
  
  /*!
   \class vm_t
   \brief Virtual machine for bytecode interpretation
   */
  class vm_t {
  public:
    /*!
     \brief Constructor
     \param vm_variables : virtual machine variables
     */
    vm_t(std::size_t flat_intvars_size, std::size_t flat_clocks_size)
    : _flat_intvars_size(flat_intvars_size), _flat_clocks_size(flat_clocks_size)
    {}
    
    /*!
     \brief Copy constructor
     \param vm : virtual machine
     \post this is a copy of vm
     */
    vm_t(tchecker::vm_t const & vm) = default;
    
    /*!
     \brief Move constructor
     \patam vm : virtual machine
     \post vm has been moved to this
     */
    vm_t(tchecker::vm_t && vm) = default;
    
    /*!
     \brief Destructor
     */
    ~vm_t() = default;
    
    /*!
     \brief Assignment operator (deleted)
     */
    tchecker::vm_t & operator= (tchecker::vm_t const &) = delete;
    // NB: deleted due to const member
    
    /*!
     \brief Move assignment operator (deleted)
     */
    tchecker::vm_t & operator= (tchecker::vm_t &&) = delete;
    // NB: deleted due to const member
    
    /*!
     \brief Bounded integer variables valuation compatibility check
     \param intvars_val : bounded  integer variables valuation
     \return true if intvars_val can store a valuation of bounded integer variables from this VM
     */
    inline constexpr bool compatible(tchecker::intvars_valuation_t const & intvars_val) const
    {
      return (intvars_val.size() >= _flat_intvars_size);
    }
    
    /*!
     \brief Bytecode interpreter
     \param bytecode : tchecker bytecode
     \param intvars_val : valuation of integer variables
     \param clkconstr : container of clock constraints
     \param clkreset : container of clock resets
     \pre bytecode is null-terminated (i.e. VM_RET) and
     intvars_val.size() <= intvars_layout_size
     \return value computed by the last instruction in bytecode
     \post bytecode has been executed: intvars_val has been updated, clock
     constraints have been pushed into clkconstr and clock resets have
     been pushed into clkreset
     \throw std::runtime_error : if bytecode interpretation fails
     \throw std::invalid_argument : if the size of intvars_val is too small
     (precondition is violated)
     \throw std::out_of_range : if access to inexisting variable or
     out-of-bound array access
     */
    tchecker::integer_t run(tchecker::bytecode_t const * bytecode,
                            tchecker::intvars_valuation_t & intvars_val,
                            tchecker::clock_constraint_container_t & clkconstr,
                            tchecker::clock_reset_container_t & clkreset)
    {
      assert( size() == 0 );    // stack should be empty
      
      if ( intvars_val.capacity() > _flat_intvars_size )
        throw std::invalid_argument("intvars valuation is too large");
      
      tchecker::integer_t eval = 0;
      _return = false;
      
      do {
        try {
          eval = interpret_instruction(bytecode, intvars_val, clkconstr, clkreset);
        }
        catch (...) {
          clear();
          throw;
        }
        
        ++bytecode;
      }
      while( _return == false );
      
      return eval;
    }
  protected:
    // bytecode instructions interpretation
    
    /*!
     \brief Single instruction interpreter
     \param bytecode : tchecker bytecode
     \param intvars_val : valuation of integer variables
     \param clkconstr : container of clock constraints
     \param clkreset : container of clock resets
     \return computed value
     \pre intvars_val.size() > max_intvar_id
     \post the instruction pointed by bytecode has been interpreted, intvars_val
     and the stack have been updated. All clock constraints have been pushed into
     clkconstr and all clock resets have been pushed into clkreset
     \throw std::runtime_error : if bytecode interpretation fails
     \throw std::out_of_range : if access to inexisting variable or
     out-of-bound array access
     */
    inline tchecker::integer_t interpret_instruction(tchecker::bytecode_t const * & bytecode,
                                                     tchecker::intvars_valuation_t & intvars_val,
                                                     tchecker::clock_constraint_container_t & clkconstr,
                                                     tchecker::clock_reset_container_t & clkreset)
    {
      // Assume stack=v1 ... vK where vK is the top symbol
      switch (*bytecode) {
          // end of operation, return vK
        case VM_RET:
        {
          auto val = top_and_pop<tchecker::integer_t>();
          assert( size() == 0 );
          clear();
          _return = true;
          return val;
        }
          
          
          // end of operation when vK==0, return 0
        case VM_RETZ:
        {
          if (top<tchecker::integer_t>() == 0) {
            clear();
            _return = true;
            return 0;
          }
          return top<tchecker::integer_t>();
        }
          
          
          // raise exception when not (l <= vK <= h) for parameters l and h
          // of instruction VM_FAILNOTIN
        case VM_FAILNOTIN:
        {
          tchecker::bytecode_t const l = * ++bytecode;
          tchecker::bytecode_t const h = * ++bytecode;
          auto const offset = top<tchecker::bytecode_t>();
          assert( contains_value<tchecker::integer_t>(l) );
          assert( contains_value<tchecker::integer_t>(h) );
          assert( contains_value<tchecker::integer_t>(offset) );
          if ( (offset < l) || (offset > h) ) {
              std::stringstream ss;
              ss << offset << " out of [" << l << ", " << h << "]";
              throw std::out_of_range("out-of-bounds value: " + ss.str());
          }

          return top<tchecker::integer_t>();
        }

          // unconditional jump relatively to next instruction;
          // offset is a parameter of the instruction
        case VM_JMP:
        {
          tchecker::bytecode_t const shift = * ++bytecode;
          // jump is relative to the address of the next instruction:
          // - bytecode++;
          // but we have to handle the increment of the IP in the main loop:
          // - bytecode += shift - 1;
          // so:
          bytecode += shift;

          return 1;
        }
          // stack = v1 ... vK   jump if vK == 0
          // offset is a parameter of the instruction
        case VM_JMPZ:
        {
          tchecker::bytecode_t const shift = * ++bytecode;

          if (top_and_pop<tchecker::integer_t>() == 0)
            {
              bytecode += shift;
              return 0;
            }
          return 1;
        }

          // stack = v1 ... vK v   where v is a parameter of instruction VM_PUSH
        case VM_PUSH:
        {
          tchecker::bytecode_t const v = * ++bytecode;
          push<tchecker::bytecode_t>(v);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... [vK]   vK replaced by value at ID vK in intvars
          // valuation
        case VM_VALUEAT:
        {
          auto const id = top_and_pop<tchecker::intvar_id_t>();
          if (id >= _flat_intvars_size)
            throw std::out_of_range("invalid variable ID");
          push<tchecker::integer_t>(intvars_val[id]);
          return top<tchecker::integer_t>();
        }
          
          
          // [vK-1] = vK, stack = v1 ... vK-2
        case VM_ASSIGN:
        {
          auto const value = top_and_pop<tchecker::integer_t>();
          auto const id = top_and_pop<tchecker::intvar_id_t>();
          if (id >= _flat_intvars_size)
            throw std::out_of_range("invalid variable ID");
          intvars_val[id] = value;
          return value;
        }
          
          
          // stack = v1 ... vK-2 (vK-1 && vK)
        case VM_LAND:       				{
          auto const right = top_and_pop<tchecker::integer_t>();
          auto const left = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(left && right);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-2 (vK-1 - vK)
        case VM_MINUS:
        {
          auto const right = top_and_pop<tchecker::integer_t>();
          auto const left = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(left - right);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-2 (vK-1 / vK)
        case VM_DIV:
        {
          auto const right = top_and_pop<tchecker::integer_t>();
          auto const left = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(left / right);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-2 (vK-1 == vK)
        case VM_EQ:
        {
          auto const right = top_and_pop<tchecker::integer_t>();
          auto const left = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(left == right);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-2 (vK-1 >= vK)
        case VM_GE:
        {
          auto const right = top_and_pop<tchecker::integer_t>();
          auto const left = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(left >= right);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-2 (vK-1 > vK)
        case VM_GT:
        {
          auto const right = top_and_pop<tchecker::integer_t>();
          auto const left = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(left > right);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-2 (vK-1 < vK)
        case VM_LT:
        {
          auto const right = top_and_pop<tchecker::integer_t>();
          auto const left = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(left < right);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-2 (vK-1 <= vK)
        case VM_LE:
        {
          auto const right = top_and_pop<tchecker::integer_t>();
          auto const left = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(left <= right);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-2 (vK-1 * vK)
        case VM_MUL:
        {
          auto const right = top_and_pop<tchecker::integer_t>();
          auto const left = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(left * right);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-2 (vK-1 % vK)
        case VM_MOD:
        {
          auto const right = top_and_pop<tchecker::integer_t>();
          auto const left = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(left % right);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-2 (vK-1 != vK)
        case VM_NE:
        {
          auto const right = top_and_pop<tchecker::integer_t>();
          auto const left = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(left != right);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-2 (vK-1 + vK)
        case VM_SUM:
        {
          auto const right = top_and_pop<tchecker::integer_t>();
          auto const left = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(left + right);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-1 (- vK)
        case VM_NEG:
        {
          auto const v = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(- v);
          return top<tchecker::integer_t>();
        }
          
          
          // stack = v1 ... vK-1 (! vK)
        case VM_LNOT:
        {
          auto const v = top_and_pop<tchecker::integer_t>();
          push<tchecker::integer_t>(! v);
          return top<tchecker::integer_t>();
        }

          // no-operation
        case VM_NOP:
          return 1;
          
          
          // stack = v1 ... vK-4 1  output (vK-2 vK-1 s vK)   where s is a
          // parameter of VM_CLKCONSTR (strictness)
        case VM_CLKCONSTR:
        {
          tchecker::bytecode_t cmp =  * ++bytecode;
          auto const bound = top_and_pop<tchecker::integer_t>();
          auto const id2 = top_and_pop<tchecker::clock_id_t>();
          auto const id1 = top_and_pop<tchecker::clock_id_t>();
          if (id1 >= _flat_clocks_size)
            throw std::out_of_range("invalid first clock ID");
          if (id2 >= _flat_clocks_size)
            throw std::out_of_range("invalid second clock ID");
          static_assert(tchecker::clock_constraint_t::LT == 0, "");
          clkconstr.emplace_back(id1, id2, (cmp == 0 ? tchecker::clock_constraint_t::LT : tchecker::clock_constraint_t::LE), bound);
          return 1;
        }
          
          
          // stack = v1 ... vK-3    output (vK-2 vK-1 vK)
        case VM_CLKRESET:
        {
          auto const value = top_and_pop<tchecker::integer_t>();
          auto const right_id = top_and_pop<tchecker::clock_id_t>();
          auto const left_id = top_and_pop<tchecker::clock_id_t>();
          if (right_id >= _flat_clocks_size)
            throw std::out_of_range("invalid first clock ID");
          if (left_id >= _flat_clocks_size)
            throw std::out_of_range("invalid second clock ID");
          clkreset.emplace_back(left_id, right_id, value);
          return 1;
        }

          // push a new frame for local variables
        case VM_PUSH_FRAME:
        {
          _frames.emplace_back ();
          return 1;
        }
          // pop the top-level frame
        case VM_POP_FRAME:
        {
          _frames.pop_back ();
          return 1;
        }

          // stack = v1 ... vK-1 [vK]
          // vK is replaced by the value of the local variable identified by vK.
        case VM_VALUEAT_FRAME:
        {
          auto const id = top_and_pop<tchecker::bytecode_t> ();
          push<tchecker::integer_t> (slot_of (id));
          return top<tchecker::integer_t> ();
        }

          // stack = v1 ... vK-2
          // [vK-1] is assigned vK where vK-1 identifies a local variables.
        case VM_ASSIGN_FRAME:
        {
          auto const value = top_and_pop<tchecker::integer_t> ();
          auto const id = top_and_pop<tchecker::intvar_id_t> ();
          slot_of (id) = value;
          return value;
        }

          // stack = v1 ... vK-2
          // [vK-1] is initialized with vK where vK-1 identifies a local variables.
        case VM_INIT_FRAME:
        {
          auto const value = top_and_pop<tchecker::intvar_id_t> ();
          auto const id = top_and_pop<tchecker::intvar_id_t> ();
          _frames.back ()[id] = value;

          return 0;
        }
      }
      
      // should never be reached
      throw std::runtime_error("incomplete switch statement");
    }

    using frame_t = std::map<tchecker::bytecode_t, tchecker::integer_t>;

    /*!
     \brief Look for a local variable in the stack of frames
     \param id the identifier of the local variable
     \return the lvalue of this variable
     \throw std::out_of_range is the variable is not found.s
     */
    tchecker::integer_t &slot_of (tchecker::bytecode_t id) {
      for(auto it = _frames.rbegin (); it != _frames.rend (); ++it)
        {
          auto kv = it->find (id);
          if (kv != it->end ())
            return kv->second;
      }
      throw std::out_of_range("unknown local variable ID");
    }


    // integer domain checking
    
    /*!
     \brief Check integer type compatibility
     \tparam EXPECTED : expected integer type
     \tparam ACTUAL : actual integer type
     \param val : actual value
     \return true if val can be represented by type EXPECTED, false otherwise
     */
    template <class EXPECTED, class ACTUAL>
    inline static bool contains_value(ACTUAL val)
    {
      static_assert(std::is_integral<EXPECTED>::value, "EXPECTED should be an integral type");
      static_assert(std::is_integral<ACTUAL>::value, "ACTUAL should be an integral type");
      
      return ((val >= std::numeric_limits<EXPECTED>::min()) && (val <= std::numeric_limits<EXPECTED>::max()));
    }
    
    
    
    
    // stack operations
    
    /*!
     \brief Accessor
     \tparam T : type of value
     \return top stack value casted to T
     \pre the stack is not empty and the top value can be represented by type T
     (checked by assertion)
     */
    template <class T>
    inline T top()
    {
      assert( _stack.size() >= 1 );
      tchecker::bytecode_t const val = _stack.back();
      if ( ! contains_value<T>(val) )
        throw std::runtime_error("value out-of-bounds");
      return static_cast<T>(val);
    }
    
    
    /*!
     \brief Accessor
     \tparam T : type of value
     \return top stack values casted to T
     \pre see top()
     \post the top value has been popped
     */
    template <class T>
    inline T top_and_pop()
    {
      T t = top<T>();
      _stack.pop_back();
      return t;
    }
    
    
    /*!
     \brief Push a value to the stack
     \tparam T : type of value
     \param t : value
     \pre t can be represented by tchecker::bytecode_t (checked by assertion)
     \post t has been pushed on the stack
     */
    template <class T>
    inline void push(T t)
    {
      if ( ! contains_value<tchecker::bytecode_t>(t) )
        throw std::runtime_error("value out-of-bounds");
      _stack.push_back(t);
    }
    
    
    /*!
     \brief Clear the stack
     \post the stack is empty
     */
    inline void clear()
    {
      _stack.clear();
    }
    
    
    /*!
     \brief Accessor
     \return size of the stack
     */
    inline std::size_t size() const
    {
      return _stack.size();
    }
    

    std::size_t const _flat_intvars_size;          /*!< Number of flat bounded integer variables */
    std::size_t const _flat_clocks_size;           /*!< Number of flat clock variables */
    bool _return;                                  /*!< Return flag */
    std::vector<tchecker::bytecode_t> _stack;      /*!< Interpretation stack */
    // NB: implemented as an std::vector for methods clear() and size()

    std::vector<frame_t> _frames;
  };
  
} // end of namespace tchecker


#endif // TCHECKER_VM_HH
