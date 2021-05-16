add_test( LexerTest.TestNextToken [==[/home/eemil/dev/c++/lups/nodeps/build/lups_test]==] [==[--gtest_filter=LexerTest.TestNextToken]==] --gtest_also_run_disabled_tests)
set_tests_properties( LexerTest.TestNextToken PROPERTIES WORKING_DIRECTORY [==[/home/eemil/dev/c++/lups/nodeps/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( LexerTest.IfStatements [==[/home/eemil/dev/c++/lups/nodeps/build/lups_test]==] [==[--gtest_filter=LexerTest.IfStatements]==] --gtest_also_run_disabled_tests)
set_tests_properties( LexerTest.IfStatements PROPERTIES WORKING_DIRECTORY [==[/home/eemil/dev/c++/lups/nodeps/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( LexerTest.Variables [==[/home/eemil/dev/c++/lups/nodeps/build/lups_test]==] [==[--gtest_filter=LexerTest.Variables]==] --gtest_also_run_disabled_tests)
set_tests_properties( LexerTest.Variables PROPERTIES WORKING_DIRECTORY [==[/home/eemil/dev/c++/lups/nodeps/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( LexerTest.FunctionDeclaration [==[/home/eemil/dev/c++/lups/nodeps/build/lups_test]==] [==[--gtest_filter=LexerTest.FunctionDeclaration]==] --gtest_also_run_disabled_tests)
set_tests_properties( LexerTest.FunctionDeclaration PROPERTIES WORKING_DIRECTORY [==[/home/eemil/dev/c++/lups/nodeps/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( LexerTest.EqualNonEqual [==[/home/eemil/dev/c++/lups/nodeps/build/lups_test]==] [==[--gtest_filter=LexerTest.EqualNonEqual]==] --gtest_also_run_disabled_tests)
set_tests_properties( LexerTest.EqualNonEqual PROPERTIES WORKING_DIRECTORY [==[/home/eemil/dev/c++/lups/nodeps/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( ParserTest.LetStatements [==[/home/eemil/dev/c++/lups/nodeps/build/lups_test]==] [==[--gtest_filter=ParserTest.LetStatements]==] --gtest_also_run_disabled_tests)
set_tests_properties( ParserTest.LetStatements PROPERTIES WORKING_DIRECTORY [==[/home/eemil/dev/c++/lups/nodeps/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( ParserTest.ReturnStatements [==[/home/eemil/dev/c++/lups/nodeps/build/lups_test]==] [==[--gtest_filter=ParserTest.ReturnStatements]==] --gtest_also_run_disabled_tests)
set_tests_properties( ParserTest.ReturnStatements PROPERTIES WORKING_DIRECTORY [==[/home/eemil/dev/c++/lups/nodeps/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( ParserTest.IdentifierExpression [==[/home/eemil/dev/c++/lups/nodeps/build/lups_test]==] [==[--gtest_filter=ParserTest.IdentifierExpression]==] --gtest_also_run_disabled_tests)
set_tests_properties( ParserTest.IdentifierExpression PROPERTIES WORKING_DIRECTORY [==[/home/eemil/dev/c++/lups/nodeps/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
add_test( ParserTest.IntegerLiteralExpression [==[/home/eemil/dev/c++/lups/nodeps/build/lups_test]==] [==[--gtest_filter=ParserTest.IntegerLiteralExpression]==] --gtest_also_run_disabled_tests)
set_tests_properties( ParserTest.IntegerLiteralExpression PROPERTIES WORKING_DIRECTORY [==[/home/eemil/dev/c++/lups/nodeps/build]==] SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( lups_test_TESTS LexerTest.TestNextToken LexerTest.IfStatements LexerTest.Variables LexerTest.FunctionDeclaration LexerTest.EqualNonEqual ParserTest.LetStatements ParserTest.ReturnStatements ParserTest.IdentifierExpression ParserTest.IntegerLiteralExpression)
