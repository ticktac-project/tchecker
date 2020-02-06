# Adding a new kind of expression 

In this document we will add a new kind of operator to the syntax of expressions. The new
operator introduced by following changes is the well-known ternary operator `if-then-else`. 

## Creating the new classes  

An expression `X` needs the creation of two classes:
* `X_expression_t` declared in [include/tchecker/expression/expression.hh](file:../include/tchecker/expression/expression.hh) 
* and `typed_X_expression_t` in [include/tchecker/expression/typed_expression.hh](file:../include/tchecker/expression/typed_expression.hh)  

Instead of creating generic classes that represent ternary expressions, we add _ad-hoc_ ones:

* in header file [include/tchecker/expression/expression.hh](file:../include/tchecker/expression/expression.hh)
    * declare the new class `tchecker::ite_expression_t`  
    * add a new `visit` method in class `expression_visitor_t`

* in [src/expression/expression.cc](file:../src/expression/expression.cc)
    * implement methods of the class `tchecker::ite_expression_t`

* in [include/tchecker/expression/type_inference.hh](file:../include/tchecker/expression/type_inference.hh) and [src/tchecker/expression/type_inference.cc](include/tchecker/expression/type_inference.cc)
    * add inference function that computes the `expression_type_t` w.r.t. type of operands of an `ite`. 
    
* in header file [include/tchecker/expression/typed_expression.hh](file:../include/tchecker/expression/typed_expression.hh)     
    * forward-declared the new class of typed expression : `typed_ite_expression_t` 
    * extend signature of `typed_expression_visitor_t` with the `visit` method that accept `typed_ite_expression_t`  
    * declare class `typed_ite_expression_t` that inherits from `tchecker::make_typed_expression_t<tchecker::ite_expression_t>`
    
* in [src/expression/typed_expression.cc](file:../src/expression/typed_expression.cc)
    * implement methods for `typed_ite_expression_t`. 

## Add necessary VM bytecodes

To implement the `ite` operator we use an _ad-hoc_ instruction `VM_ITE` added to 
program interpreter. However it could have been implement using jump instructions.

* in [src/vm/compilers.cc](file:../src/vm/compilers.cc):
    * add the compilation of the operator into a `VM_ITE` instruction.
        * bytecodes of operands are inserted in the order `if`, `then` and `else` (the)
        interpreter must pop evaluations of operands in reverse order.
        * then 'VM_ITE' is inserted.
    * Update visitors of bytecode compiler in [src/vm/compilers.cc](file:../src/vm/compilers.cc):
        * `rvalue_expression_compiler_t`
        * `lvalue_expression_compiler_t`
        * `variable_bounds_visitor_t`    
    
* in [include/tchecker/vm/vm.hh](file:../include/tchecker/vm/vm.hh), add to `interpret_instruction`
the code that implements the execution of the instruction `VM_ITE`.

* in [src/vm/vm.cc](file:../src/vm/vm.cc), add the output of `VM_ITE` instruction.

## Update visitors

* Update typechecker's visitor, in [src/expression/typechecking.cc](file:../src/expression/typechecking.cc)
    * update `tchecker::details::expression_typechecker_t`
           
* Update visitors of static analyzers in [src/expression/static_analysis.cc](file:../src/expression/static_analysis.cc)
    * Update visitors (prefixed by `tchecker::details::`). 
        * `const_expression_evaluator_t`.                
        * `base_variable_ids_extractor_t`                
        * `extract_offset_variables_visitor_t`
        * `extract_variables_visitor_t`                     

* Update visitor of clockbounds solver in [src/clockbounds/solver.cc](file:../src/clockbounds/solver.cc)
    * `solver_updater_t`

          
 ## Syntax
 
 First `if`, `then` and `else` tokens are added to the lexical analyzer [src/parsing/program_parser/programm.ll](file:../src/parsing/program_parser/programm.ll):
 
     "if"           { return program::parser_t::make_TOK_IF(loc); }
     "then"         { return program::parser_t::make_TOK_THEN(loc); }
     "else"         { return program::parser_t::make_TOK_ELSE(loc); }
 
 Note: they have to be declared before the rule that handles `identifier`s.      
 
 The grammar of programs [src/parsing/program_parser/programm.yy](file:../src/parsing/program_parser/programm.yy) is also updated
 in order to accept `if-then-else` operator applied to `term`s (i.e. it doesn't operate on predicates):
 
    term : 
    ... 
    | TOK_IF predicate_formula TOK_THEN term TOK_ELSE term
    {
      try {
        $$ = new tchecker::ite_expression_t($2, $4, $6);
      }
      catch (std::exception const & e) {
        error(@$, e.what());
        $$ = new fake_expression_t();
      }
    }

