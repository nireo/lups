add_test( LexerTest.TestNextToken [==[/home/eemil/dev/c++/lups/nodeps/build/lups_test]==] [==[--gtest_filter=LexerTest.TestNextToken]==] --gtest_also_run_disabled_tests)
set_tests_properties( LexerTest.TestNextToken PROPERTIES WORKING_DIRECTORY [==[/home/eemil/dev/c++/lups/nodeps/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( lups_test_TESTS LexerTest.TestNextToken)
