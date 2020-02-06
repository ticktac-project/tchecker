The TChecker file format is used to describe timed automata. A file
consists in a sequence of declarations of processes, clocks,
locations, edges, etc, as described below. Declarations can appear in any order, as soon as
these two rules are respected:

- the `system` declaration must be the first declaration in the file

- every item (process, location, edge, event, variable, ...) must be
declared before it is used.

All declarations appear in the same global scope. Hence, all declared
variables (bounded integers, clocks, etc) are global. Local variables
can be simulated by adding a prefix to the name of the variable,
e.g. `P1_x` instead of `x` for clock `x` of process `P1`.

A comment starts with symbol `#` and runs till the end of the line.

Symbols `:`, `@` and `#` have a special meaning in TChecker files,
and are thus reserved. Keywords: `clock`, `edge`, `event`, `int`,
`location`, `process`, `sync` and `system` are reserved.

Identifiers are any string containing letters, numbers, underscore
(`_`) and dot (`.`) and starting with either a letter or underscore
(`_`).

1. [The `system` declaration](#the-system-declaration)
1. [The `process` declaration](#the-process-declaration)
1. [The `event` declaration](#the-event-declaration)
1. [The `clock` declaration](#the-clock-declaration)
1. [The `int` declaration](#the-int-declaration)
1. [The `location` declaration](#the-location-declaration)
1. [The `edge` declaration](#the-edge-declaration)
1. [The `sync` declaration](#the-sync-declaration)
1. [Attributes](#attributes)
   1. [Finite-state machines](#finite-state-machines)
   1. [Timed automata](#timed-automata)
1. [Expressions](#expressions)
1. [Statements](#statements)
1. [Semantics](#semantics)
   1. [Finite-state machines](#finite-state-machines-1)
   1. [Timed automata](#timed-automata-1)

# The `system` declaration

```
system:id
```

where `id` is the identifier of the system.

There shall be only one `system` declaration in a TChecker file. And it shall appear as the first declaration in the file.

# The `process` declaration

```
process:id
```

where `id` is the identifier of the process. No other process shall have
the same identifier.

A `process` declaration declares a process name. It does not declare a new scope. Hence all declarations following the process declaration are in the global scope.

There is no way to declare a type of process, and instantiate it in TChecker. In order to specify several instances of the same process type, the process declaration and all the related declarations (locations, edges, etc) shall be duplicated. This can be solved by writing a script that generates the TChecker model and that handles process instantiation smoothly.


# The `event` declaration

```
event:id
```

where `id` is the identifier of the event. No other event shall have the
same identifier.


# The `clock` declaration

```
clock:size:id
```

declares an array of `size` clocks with identifier `id`. No other clock shall
have the same identifier.

For instance:

```
clock:1:x
```

declares a single clock `x`. And:

```
clock:10:y
```

declares an array `y` of 10 clocks. Then, `y[0]` is the first clock, and
`y[9]` is the last one.

Out-of-bounds access like `y[10]` are detected and raise a fatal error
when the model is analysed.

Clocks have an implicit domain of values ranging from 0 to +infinity,
and implicit initial value 0. Clocks only admit a restricted set of
operations (see [Expressions](#expressions) and [Statements](#statements) for
details). Some errors can be detected at compilation time, but some
others can only be detected when the model is analysed. In particular,
assigning a value to clock out of its domain raise a fatal error when
the model is analysed.

NB: while arrays of clocks may be convenient for modeling purposes, they
make some analysis harder. In particular, the clock bounds that are used
for zone abstractions are harder to compute (and often much less precise)
when using clock arrays. This may result in exponentially bigger zone
graphs. We thus recommend to avoid using clock arrays when possible.


# The `int` declaration

```
int:size:min:max:init:id
```

declares the array of `size` bounded integer variables with identifier
`id`. Each variable in the array takes values between `min` and `max`
(both included) and initial value `init`. No other integer variable
shall have the same identifier.

For instance:

```
int:1:0:127:0:i
```

declares a single integer variable `i` with values between 0 and 127, and
initial value 0.

And:

```
int:5:-128:127:-1:j
```

declares an array `j` of 5 integer variables with values between -128 and 127,
and initial value -1. The first variable of the array is `j[0]` and the last
one is `j[4]`.

Out-of-bounds access like `j[10]` are detected and raise a fatal error
when the model is analysed.

NB: the semantics of bounded integers in TChecker does not allow to assign
an out-of-bound value to a variable. Consider variable `i` as declared
above. Assigning -1 to `i` is illegal. Similarly, if `i` has value 127, then
adding 1 to `i` is illegal. Illegal statements are simply deemed invalid and
not executable (see section [Semantics](#semantics) for details).


# The `location` declaration

```
location:p:id{attributes}
```

declares location with identifier `id` in process with identifier
`p`, and given `attributes`. The process identifier `p` shall have
been declared previously. No other location within process `p` shall
have the same identifier. It is perfectly valid that two locations in
different processes have the same identifier.

The `{attributes}` part of the declaration can be ommitted if no
attribute is associated to the location (or it can be empty:
`{}`). See section [Attributes](#attributes) for details.


# The `edge` declaration

```
edge:p:source:target:e{attributes}
```

declares an edge in process `p` from location `source` to location
`target` and labelled with event `e`.  The process `p` shall have been
declared previously. The two locations `source` and `target` shall
have been declared as well, and they shall both belong to process
`p`. The event `e` shall have been declared before the edge is
declared.

The `{attributes}` part of the declaration can be omitted if no
attribute is associated to the location (or it can be empty:
`{}`). See section [Attributes](#attributes) for details.


# The `sync` declaration

```
sync:sync_constraints
```

declares a synchronisation constraint. `sync_constraints` is a
column-separated list of synchronisation constraints of the form `p@e`
or `p@e?` where `p` is a process name, `e` is an event name, and the
option question mark `?` denotes a weak synchronisation. Process `p`
and event `e` shall have been declared before the synchronisation is
declared.

A `sync` declaration must contain at least 2 synchronisation
constraints, and at most one synchronisation constraint for each
process in the model.  A (strong) synchronization constraint `p@e`
means that process `p` *shall* synchronise one of its `e` labeled
edge. A weak synchronisation constraint `p@e?` means that process `p`
*may* synchronize one of its `e` labeled edge. Process `p` shall
synchronize if it has an enabled `e` labelled edge. It does not
prevent synchronization of the other processes otherwise.

If an event `e` appears in a `sync` declaration along with process `p`
(i.e.  in a constraint `p@e` or `p@e?`), then `e` labeled edges of
process `p` shall only be taken synchronously (according to `sync`
declarations). An event `e` that does not appear in any
synchronisation constraint with process `p` is asynchronous in process
`p`.

For instance, the following declarations (assuming processes and events have
been declared):

```
sync:P1@a:P2@a
sync:P1@a:P2@b:P3@c?:P4@d?
```

entails that event `a` is synchronous in processes `P1` and `P2`,
event `b` is synchronous in process `P2`, event `c` is synchronous in
process `P3`, and event `d` is synchronous in process `P4`. All other
events are asynchronous.

Thus assuming the following set of edges (and that locations and
events have been declared):

```
edge:P1:l0:l1:a
edge:P1:l0:l2:a

edge:P2:l0:l1:b

edge:P3:l0:l1:a

edge:P4:l0:l1:d
```

the set of global edges from the tuple of locations `<l0,l0,l0,l0>`
(i.e. all the processes are in location `l0`) is:

```
<l0,l0,l0,l0> - <P1@a,P2@b,P4@d> -> <l1,l1,l0,l1>  // 2nd `sync` declaration
<l0,l0,l0,l0> - <P1@a,P2@b,P4@d> -> <l2,l1,l0,l1>  // 2nd `sync` declaration
<l0,l0,l0,l0> - <P3@a>           -> <l0,l0,l1,l0>  // asynchronous
```

Observe that the first `sync` declaration cannot be instantiated as
process `P2` has no `a` labelled edge from `l0`.

The second `sync` declaration can be instantiated as the two strong
constraints `P1@a` and `P2@b` have corresponding edges. The weak
synchronisation constraint `P4@d?` has a corresponding edge in `P4`
from `l0`, so `P4` is involved in the global edge. `P3` however has no
`c` labelled edge from `l0`.  Thus, the weak constraint `P3@c?` cannot
be instantiated, and `P3` does not participate to the global
edge. However this does not prevent synchronisation of the other three
processes. The second `sync` declaration is instantiated twice since
process `P1` has two `a` labelled edges from location `l0`.

Event `a` is asynchronous in process `P3` as it does not appear in any
`sync` declaration along with process `P3`. Hence there is a global
asynchronous edge involving only process `P3` with event `a`.

A `sync` declaration consisting only of weak synchronisation
constraints like:

```
sync:P1@e?:P2@f?
```

is instantiated as soon as at least one of the constraint can be
instantiated. Hence there is no global edge when `P1` has no `e` labelled edge, and `P2` has no `f` edge.


# Attributes

Attributes allow to define properties of declarations. Currently, only
`edge` and `location` declarations support attributes.

A list of attributes `{attributes}` is a colon-separated list of
pairs `key:value`. Keys are identifier and values are any string not
containing reserved symbols `:`, `@` and spaces. The value may be
empty.

There is no fixed list of allowed attributes. New attributes can be
introduced at will as a way to provide information to
algorithms. Current algorithms may emit a warning when an unknown
attribute is encountered. But this shall not prevent the algorithm to
analyse the model (without taking the unknown attribute into account).

Current models support the attributes described below.


## Finite-state machines

Supported location attributes:

- `initial:` (no value) declares an initial location. Each process
  shall have at least one initial location, and it can have several of
  them

- `labels: list_of_labels` declares the labels of a location (later
  used for reachability checking for instance). The list of labels is
  a comma-separated list of strings (not containing any reserved
  characters or spaces).

- `invariant: expression` declares the invariant of the the
  location. See section [Expressions](#expressions) for details on expressions

Supported edge attributes:

- `provided: expression` declares the guard of the edge. See section
  [Expressions](#expressions) for details on expressions.

- `do: statement` declares the statement of the edge. See section
  [Statements](#statements) for details on statements.

NB: finite-state models shall have no `clock` declaration. As a
result, expression and statements shall not involve clocks.


## Timed automata

Extends attributes supported by finite-state machines with the
following ones, for locations:

- `committed:` (no value) declares the location committed (see
  [Semantics](#semantics) for details).

- `urgent:` (no value) declares the location urgent (see [Semantics](#semantics) for
  details).



# Expressions

Expressions are conjunctions of atomic expression or negation of
atomic expressions. An atomic expression is either a term or a binary
predicate applied to two terms. Terms consists in integer constants,
variables and arithmetic operators applied to terms. Expressions are
defined by the following grammar:

```
expr ::= atomic_expr
       | atomic_expr && expr

atomic_expr ::= int_term
              | predicate_expr
	      | ! atomic_expr
	      | clock_expr

predicate_expr ::= ( predicate_expr )
                 | int_term == int_term
		 | int_term != int_term
		 | int_term < int_term
		 | int_term <= int_term
		 | int_term >= int_term
		 | int_term > int_term

int_term ::= INTEGER
           | lvalue_int
           | - int_term
           | int_term + int_term
           | int_term - int_term
           | int_term * int_term
           | int_term / int_term
           | int_term % int_term
           | (if expr then int_term else int_term)

lvalue_int ::= INT_VAR_ID
             | INT_VAR_ID [ term ]
```

where `INT_VAR_ID` is a bounded integer variable identifier, and
`INTEGER` is a signed integer constant. Notice that integer terms
`int_term` are atomic expressions with the usual convention that the
expression is false iff the term has value 0.

There are restrictions on atomic expressions involving clock
variables. We only allow comparisons of clocks or difference of clocks
w.r.t. integer terms:

```
clock_expr ::= clock_term == int_term
             | clock_term < int_term
	     | clock_term <= int_term
	     | clock_term >= int_term
	     | clock_term > int_term

clock_term ::= clock_lvalue
             | clock_lvalue - clock_lvalue

clock_lvalue ::= CLOCK_ID
               | CLOCK_ID [ int_term ]
```

where `CLOCK_ID` is a clock identifier.


# Statements

Statements are sequences of assignements or `nop`:

```
stmt ::= statement [;]
       | statement ; stmt

statement ::= if_statement
           |  loop_statement
           |  simple_statement

statement ::= if expr then stmt end
           |  if expr then stmt else stmt end
           |  while expr do stmt end
           |  local_declaration
           |  int_assignment
           |  clock_assignment
    	   |  nop    	           

int_assignement ::= lvalue_int = int_term
```

Assignments to clock variables are restricted to assignment of a
constant to a clock `x = y`, or "diagonal assignments" of the form `x
= y + d` where d is any integer term:

```
clock_assignment ::= lvalue_clock = int_term
		   | lvalue_clock = lvalue_clock + int_term
```

It is possible to declare local variables using the keyword `local`. Even if 
local declarations may occur anywhere in an attribute, variables must not 
conflict with an existing variable (global or local). Size of 
local arrays must be computable at compilation time.
```
local_declaration ::= local var
                   |  local var = expr
                   |  local var [ expr ] 
```
# Semantics

## Finite-state machines

A *configuration* of the finite-state machine consists in a tuple of
locations L (one location per process) and a valuation V of the
bounded integer variables in the model.

The *initial configurations* are pairs (L,V) such that L is a tuple of
initial locations, V maps each variable to its initial value, and V
satisfies the `invariant` attribute of each location in L.

There is a *transition* (L,V) -- E -> (L',V') if there is a tuple E of
edges from L that instantiates a `sync` declaration, or E consists of
a single asynchronous edge, and:

- the `provided` attribute of each edge in E is satisfied by V

- V' is obtained from V by applying the `do` attribute of each edge
  in E consecutively

- for each variable i, V'(i) is in the domain of i

- L' is obtained from L according to the edges in E

- and V' satisfies the `invariant` of each location in in L'

This defines a transition system with configurations, initial
configurations and transitions as described above.


### Timed automata

The *configurations* of the timed automaton consists in a tuple of
locations L and a valuation V of the bounded integer variables (as for
finite-state machines) and a valuation X of the clocks in the model.

The *initial configurations* are triples (L,V,X) such that L and V are
initial as for the finite-state machine, X maps each clock to 0, and
the `invariant` attribute if each location in L is satisfied by V and
X.

There are two kinds of *transitions*. Discrete transitions (L,V,X) --
E -> (L',V',X') are defined as for finite-state machines, and
furthermore:

- clocks are taken into account for the evaluation of `provided`,
  `invariant` and `do` attributes of locations and edges in L and E

- and E involves (at least) a process with a `committed` location in L
  if any

Timed transitions (L,V,X) ---> (L,V,X')

- L has no `committed` and no `urgent` location

- there is a non-negative real number d such that X'(y)=X(y)+d for
  each clock y

- and the `invariant` in L is satisfied by V and X'.

This defines a transition system with configurations, initial
configurations and transitions as described above.
