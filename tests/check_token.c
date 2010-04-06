#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include <check.h>
#include "../src/logging.h"
#include "../src/statefulstring.h"
#include "../src/token.h"

wchar_t *value      = L"This is a test.";
unsigned int length = 15;
TokenType type      = IDENTIFIER;
Token *t1, *t2;
StatefulStringPosition s1;
StatefulStringPosition s2;

void setup() {
    s1.line     = 0;
    s1.column   = 1;

    s2.line     = 2;
    s2.column   = 3;

    t1 = token_new( value, length, type, s1, s2 );
    t2 = token_new( value, length, type, s1, s2 );
}

void teardown() {
    token_free( t1 );
}

//
//  Creation
//
START_TEST (test_csstoken_create_value)
{
    fail_unless( wcscmp( t1->value, value ) == 0, "After creation, value ought be set correctly." );
}
END_TEST
START_TEST (test_csstoken_create_start)
{
    fail_unless( sspos_eq( *(t1->start), s1 ), "After creation, start pos should be correct." );
}
END_TEST
START_TEST (test_csstoken_create_end)
{
    fail_unless( sspos_eq( *(t1->end), s2 ), "After creation, end pos should be correct." );
}
END_TEST
START_TEST (test_csstoken_create_length)
{
    fail_unless( t1->length == length, "After creation, length should be set correctly." );
}
END_TEST
START_TEST (test_csstoken_create_type)
{
    fail_unless( t1->type == type, "After creation, type should be set correctly." );
}
END_TEST

//
//  Equality
//
START_TEST (test_csstoken_eq)
{
    fail_unless( token_eeq( t1, t2 ), "Tokens with the same values ought be equal." );
}
END_TEST
START_TEST (test_csstoken_eq_value)
{
    t1->value[0] = L'N';
    fail_unless( !token_eeq( t1, t2 ), "Tokens with different values ought not be equal." );
}
END_TEST
START_TEST (test_csstoken_eq_start)
{
    ( t1->start )->line = 1;
    fail_unless( !token_eeq( t1, t2 ), "Tokens with different starts ought not be equal." );
}
END_TEST
START_TEST (test_csstoken_eq_end)
{
    ( t1->end )->line = 1;
    fail_unless( !token_eeq( t1, t2 ), "Tokens with different ends ought not be equal." );
}
END_TEST
START_TEST (test_csstoken_eq_length)
{
    t1->length = 0;
    fail_unless( !token_eeq( t1, t2 ), "Tokens with different lengths ought not be equal." );
}
END_TEST
START_TEST (test_csstoken_eq_type)
{
    t1->type = ATKEYWORD;
    fail_unless( !token_eeq( t1, t2 ), "Tokens with different types ought not be equal." );
}
END_TEST


Suite * csstoken_suite (void) {
    Suite *s = suite_create( "CSS Token" );

    //
    //  Creation
    //
    TCase *tc_create = tcase_create( "Creation" );
    tcase_add_checked_fixture (tc_create, setup, teardown);
    tcase_add_test (tc_create, test_csstoken_create_value);
    tcase_add_test (tc_create, test_csstoken_create_start);
    tcase_add_test (tc_create, test_csstoken_create_end);
    tcase_add_test (tc_create, test_csstoken_create_length);
    tcase_add_test (tc_create, test_csstoken_create_type);
    suite_add_tcase (s, tc_create);
    //
    //  Equality 
    //
    TCase *tc_eq = tcase_create( "Equality" );
    tcase_add_checked_fixture (tc_eq, setup, teardown);
    tcase_add_test (tc_eq, test_csstoken_eq);
    tcase_add_test (tc_eq, test_csstoken_eq_value);
    tcase_add_test (tc_eq, test_csstoken_eq_start);
    tcase_add_test (tc_eq, test_csstoken_eq_end);
    tcase_add_test (tc_eq, test_csstoken_eq_length);
    tcase_add_test (tc_eq, test_csstoken_eq_type);
    suite_add_tcase (s, tc_eq);

    return s;
}


int main( void ) {
    int number_failed;
    Suite *s        = csstoken_suite();
    SRunner *sr     = srunner_create( s );
    srunner_set_fork_status( sr, CK_NOFORK );
    srunner_run_all( sr, CK_NORMAL );
    number_failed   = srunner_ntests_failed( sr );
    srunner_free( sr );
    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
