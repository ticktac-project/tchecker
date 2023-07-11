# Developer HOWTO

## Update CI/CD tests

CI/CD Tests consist in comparing the output produced by TChecker tools, to an expected output.
The expected outputs are stored in the repository and may need to be updated when algorithms
change (in particular, if the output of the algorithms change).

It can be checked if tests need to be updated running the command `make test` from the build
directory.

If update is required, then follow these steps:

1. configure with `cmake` as usual
2. then, run `make` with targets `save`, `save-bugfixes`, `save-simple-nr` and `save-algos`
3. finally, run the tests with `make test`
4. commit the new version of the expected outputs

Notice that it may be necessary to build successively with `USEINT16`, `USEINT32` (default)
and `USEINT64` to update all the tests.
