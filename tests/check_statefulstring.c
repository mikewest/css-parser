#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include <check.h>
#include "../src/logging.h"
#include "../src/statefulstring.h"


FILE *generateTempfile( const wchar_t *string ) {
    FILE *file      = tmpfile();
    
    if ( file ) {
        fwprintf( file, L"%S", string );
        rewind( file );
        return file;
    } else {
        wprintf( L"Couldn't create a temp file.  Dunno why.  Soz." );
        exit( EXIT_FAILURE );
    }
}

StatefulString *ss;
FILE *file;
wchar_t CORE_TEST_STRING[]  = L"This is a dummy file.\nIt has several lines.\nIt will be good for testing.\n\nOmg!";
int CORE_TEST_STRING_LENGTH = 78;  //wcslen( CORE_TEST_STRING );

void setup( void ) {
    file    = generateTempfile( CORE_TEST_STRING );
    ss      = ss_fromstream( file );
}

void teardown( void ) {
    ss_free( ss );
    fclose( file );
}
/*
 *  Core Tests: Creation, etc.
 */
START_TEST (test_statefulstring_create_value)
{
    fail_unless(
        wcscmp( ss->value, CORE_TEST_STRING ) == 0,
        "After creation, `value` should be set to the contents of the file."
    );
}
END_TEST
START_TEST (test_statefulstring_create_length)
{
    fail_unless(
        ss->length == CORE_TEST_STRING_LENGTH,
        "After creation, `length` should be the correct content length."
    );
}
END_TEST
START_TEST (test_statefulstring_create_lines)
{
    fail_unless(
        ss->lines == 5,
        "After creation, `lines` is properly set."
    );
}
END_TEST
START_TEST (test_statefulstring_create_linebreaks)
{
    unsigned int test[5] = { 0, 21, 43, 72, 73 };
    for ( int i = 0; i < ss->lines; i++ ) {
        fail_unless( ss->linebreaks[ i ] == test[ i ], NULL );
    }
}
END_TEST
START_TEST (test_statefulstring_create_index)
{
    fail_unless(
        ss->next_index == 0,
        "After creation, `next_index` points to the first item."
    );
}
END_TEST
START_TEST (test_statefulstring_create_position)
{
    fail_unless(
        (
            ( ss->next_position ).line == 0 &&
            ( ss->next_position ).column == 0
        ),
        "After creation, `next_position` points to the first line, first column."
    );
}
END_TEST
START_TEST (test_statefulstring_getchar)
{
    for ( int i = 0; i < CORE_TEST_STRING_LENGTH; i++ ) {
        fail_unless( ss_getchar( ss ) == CORE_TEST_STRING[ i ], NULL );
    }
}
END_TEST
START_TEST (test_statefulstring_getchar_index)
{
    for ( int i = 0; i < CORE_TEST_STRING_LENGTH; i++ ) {
        ss_getchar( ss );
        fail_unless( ss->next_index == ( i + 1 ), "Index should increase as we pull characters." );
    }
}
END_TEST
START_TEST (test_statefulstring_getchar_eof)
{
    wchar_t temp;
    while ( ( temp = ss_getchar( ss ) ) != WEOF ) {}

    fail_unless( temp == WEOF && ss_getchar( ss ) == WEOF, "getchar should return WEOF at the end of the string." );
}
END_TEST
START_TEST (test_statefulstring_getchar_position)
{
    int line, column;
    wchar_t temp;
    for ( int i = 0; i < CORE_TEST_STRING_LENGTH; i++ ) {
        temp = ss_getchar( ss );
        if ( temp == L'\n' ) {
            line++;
            column = 0;
        } else {
            column++;
        }
        fail_unless( 
            (
                ( ss->next_position ).line      == line &&
                ( ss->next_position ).column    == column
            ),
            "getchar should bump position, and newlines should work correctly."
        );
    }
}
END_TEST

Suite * statefulstring_suite (void) {
    Suite *s = suite_create ("Stateful String");

    TCase *tc_core = tcase_create ("Core");
    tcase_add_checked_fixture (tc_core, setup, teardown);
    //
    //  Creation
    //
    tcase_add_test (tc_core, test_statefulstring_create_value);
    tcase_add_test (tc_core, test_statefulstring_create_length);
    tcase_add_test (tc_core, test_statefulstring_create_index);
    tcase_add_test (tc_core, test_statefulstring_create_position);
    tcase_add_test (tc_core, test_statefulstring_create_lines);
    tcase_add_test (tc_core, test_statefulstring_create_linebreaks);

    //
    //  getchar
    //
    tcase_add_test (tc_core, test_statefulstring_getchar);
    tcase_add_test (tc_core, test_statefulstring_getchar_index);
    tcase_add_test (tc_core, test_statefulstring_getchar_eof);
    tcase_add_test (tc_core, test_statefulstring_getchar_position);
    suite_add_tcase (s, tc_core);

    return s;
}


int main( void ) {
    int number_failed;
    Suite *s        = statefulstring_suite();
    SRunner *sr     = srunner_create( s );
    srunner_set_fork_status( sr, CK_NOFORK );
    srunner_run_all( sr, CK_NORMAL );
    number_failed   = srunner_ntests_failed( sr );
    srunner_free( sr );
    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
