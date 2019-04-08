TChecker is a verification tool for timed automata and a plateform for testing new algorithms. It implements several verification algorithms that can be launched from the command line. This document describes how to invoke the algorithms using the command line tool.

Simply launching the command line tool:

```
tchecker
```

lists all available algorithms, for instance:

```
Usage: ./src/tchecker command [options] [file]
    with command:
        covreach      run covering reachability algorithm
        explore       run explore algorithm
    options are command-specific (use -h to get help on the command)
    reads from standard input if no file name is provided
```

Each algorithm has its own set of command line options that can be
obtained by invoking TChecker with the corresponding algorithm, and
option `-h`, for instance:

```
tchecker covreach -h
```

lists all available options for algorithm `covreach` (see below).


# Invoking algorithm `covreach`

Algorithm `covreach` is the classical reachability algorithm with state covering. Given a transition system S and a trace inclusion relation <= on the states of S, `covreach` explores S from its initial states. Each time a new state s is reached, the algorithm checks if it has already visited a state s' with s <= s'. If so, s is discarded and the exploration continue with a new node. If no such node s' exists, then the successors of s are expanded and all the nodes s'' <= s are removed.

This algorithm terminates if S is finite. It is correct if <= is a trace inclusion relation on the states of S.

The `covreach` algorithm can be invoked with the following command:

```
tchecker covreach OPTIONS model.txt
```

where OPTIONS are described below, and `model.txt` is a timed automaton model specified using the [TChecker file format](https://github.com/fredher/tchecker/wiki/TChecker-file-format).

```
-c cover         where cover is one of the following:
                 inclusion     zone inclusion
                 aLUg          aLU abstraction with global clock bounds
                 aLUl          aLU abstraction with local clock bounds
                 aMg           aM abstraction with global clock bounds
                 aMl           aM abstraction with local clock bounds
-f (dot|raw)     output format (graphviz DOT format or raw format)
-h               this help screen
-l labels        accepting labels, where labels is a column-separated list of identifiers
-m model         where model is one of the following:
                 zg:semantics:extrapolation        zone graph with:
                   semantics:      elapsed         time-elapsed semantics
                                   non-elapsed     non time-elapsed semantics
                   extrapolation:  NOextra         no zone extrapolation
                                   extraMg         ExtraM with global clock bounds
                                   extraMl         ExtraM with local clock bounds
                                   extraM+g        ExtraM+ with global clock bounds
                                   extraM+l        ExtraM+ with local clock bounds
                                   extraLUg        ExtraLU with global clock bounds
                                   extraLUl        ExtraLU with local clock bounds
                                   extraLU+g       ExtraLU+ with global clock bounds
                                   extraLU+l       ExtraLU+ with local clock bounds
                 async_zg:semantics:extrapolation  asynchronous zone graph with:
                   semantics:      elapsed         time-elapsed semantics
                                   non-elapsed     non time-elapsed semantics
                   extrapolation:  extraLU+l       ExtraLU+ with local clock bounds
-o filename      output graph to filename
-s (bfs|dfs)     search order (breadth-first search or depth-first search)
-S               output stats
--block-size n   size of an allocation block (number of allocated objects)
--table-size n   size of the nodes table

Default parameters: -c inclusion -f raw -s dfs --block-size 10000 --table-size 65536, output to standard output
                    -m must be specified
```

The transition system is a zone graph of the timed automaton model given to `tchecker`. It is specified using option `-m`. Many different zone graphs are available:

- either the zone graph `zg` or the asynchronous zone graph `async_zg` (also called local-time zone graph)

- two semantics can be chosen for zones: `elapsed` where time-elapse is the last operation in successor computation, or `non-elapsed` where time-elapse is the first operation in successor computation

- to ensure finiteness of the zone graph, an extrapolation can be applied to the zone graph: ExtraM, ExtraM+, ExtraLU and ExtraLU+, using either local clock bounds `l` or global clock bounds `g`. The `covreach` algorithm can be invoked with no extrapolation `NOextra`, but termination is not guaranteed as the zone graph may be infinite.

The trace inclusion relation <= can be chosen with option `-c`: `inclusion` is standard zone inclusion, while `aM` and `aLU` check zone inclusion w.r.t. abstractions aM and aLU, and either local clock bounds `l` or global clock bounds `g`.

The reachability objective is specified with option `-l labels`: the algorithm searches for a configuration (L,V,X) such that the labels in L cover the specified labels.

The order in which the zone graph is explored has a significant impact on the performance of the algorithm. It can be specified with option `-s`.

The `covreach` algorithm builds a subsumption graph that consists of all the maximal nodes w.r.t. the trace inclusion relation <=, and edges among them. The graph can be output using options `-f` to choose the format and option `-o` to choose the output file.

Option `-S` gives access to statistics on the run of the `covreach` algorithm.

Finally, the performance of the algorithm can be improved with options `--block-size` and `--table-size` which define the size of blocks in pool allocations, and the size of hash tables. Increasing these values consumes more memory but yields better performances on large timed automaton as the number of allocations, and the number of hash table collisions, increase significantly with the size of the automaton and its zone graph.
