# Test suites for TChecker project

Sub-directories contain specific test suites except `testutils` which contains a 
small library used to factorize some code between compiled tests. Each test suite 
can be disabled using a `cmake` option (see description below).

* `bugfixes` contains non-regression test for reported bugs (see [Issues tab on Github](https://github.com/ticktac-project/tchecker/issues?q=is%3Aissue+label%3Abug)). This testsuite is disabled using the option `-DTCK_ENABLE_BUGFIXES_TESTS=OFF`.
* `covreach` executes non-regression test on `tchecker covreach`; input tests come
 from the `examples` directory. This testsuite is disabled using the option `-DTCK_ENABLE_COVREACH_TESTS=OFF`.
* `explore` executes non-regression test on `tchecker explore`; input tests come
from the `examples` directory. This testsuite is disabled using the option `-DTCK_ENABLE_EXPLORE_TESTS=OFF`.
* `simple-nr` contains more simple non-regression tests. This testsuite is disabled
 using the option `-DTCK_ENABLE_SIMPLE_NR_TESTS=OFF`.
* `unit-tests` contains Catch2-based unit-tests for the native code. 
This testsuite is disabled using the option `-DTCK_ENABLE_UNITTESTS=OFF`.

