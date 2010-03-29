#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include <check.h>
#include "../src/logging.h"
#include "../src/statefulstring.h"
#include "../src/csstoken.h"

wchar_t *value      = L"This is a test.";
unsigned int length = 15;
TokenType type      = IDENTIFIER;
Token *t;
StatefulStringPosition s1;
StatefulStringPosition s2;

void create_setup() {
    s1.line     = 0;
    s1.column   = 1;

    s2.line     = 2;
    s2.column   = 3;

    t = token_new( value, length, type, s1, s2 );
}

void create_teardown() {
    token_free( t );
}

START_TEST (test_csstoken_create_value)
{
    fail_unless( wcscmp( t->value, value ) == 0, "After creation, value ought be set correctly." );
}
END_TEST
START_TEST (test_csstoken_create_start)
{
    fail_unless( sspos_eq( *(t->start), s1 ), "After creation, start pos should be correct." );
}
END_TEST
START_TEST (test_csstoken_create_end)
{
    fail_unless( sspos_eq( *(t->end), s2 ), "After creation, end pos should be correct." );
}
END_TEST
START_TEST (test_csstoken_create_length)
{
    fail_unless( t->length_ == length, "After creation, length should be set correctly." );
}
END_TEST
START_TEST (test_csstoken_create_type)
{
    fail_unless( t->type == type, "After creation, type should be set correctly." );
}
END_TEST


Suite * csstoken_suite (void) {
    Suite *s = suite_create ("CSS Token");

    TCase *tc_create = tcase_create ("Creation");
    tcase_add_checked_fixture (tc_create, create_setup, create_teardown);
    //
    //  Creation
    //
    tcase_add_test (tc_create, test_csstoken_create_value);
    tcase_add_test (tc_create, test_csstoken_create_start);
    tcase_add_test (tc_create, test_csstoken_create_end);
    tcase_add_test (tc_create, test_csstoken_create_length);
    tcase_add_test (tc_create, test_csstoken_create_type);


    suite_add_tcase (s, tc_create);

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
