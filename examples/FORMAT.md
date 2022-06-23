# TChecker file format

A model is a list of declarations. Objects must be declared before they are
used. For instance, the source and target locations of an edge must be
declared before the edge. Please, notice that all declarations are in the
global scope. The model must start with a system declaration.

Please, refer to [TChecker
wiki](https://github.com/ticktac-project/tchecker/wiki/TChecker-file-format) for
a detailed documentation of the input language

## Declarations

system:name
clock:size:name
int:size:min:max:init:name
process:name
event:name
location:process:name
edge:process:source:target:event
sync:events
   where events is a colon-separated list of process@event, for instance:
   P1@a:P2@b:P3@c

## Attributes

Each declaration can be appended a list of attributes between braces, for
instance: { initial: : invariant: x<1 }
Each attribute is a pair "key: value" and attributes are separated by
columns. It is syntactically valid to omit the values (as for
attribute "initial" above), but some keys may require a value (for instance,
attribute "invariant").
