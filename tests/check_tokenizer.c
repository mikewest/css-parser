#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include <check.h>
#include "../src/logging.h"
#include "../src/statefulstring.h"
#include "../src/token.h"
#include "../src/csstokenizer.h"

StatefulString *ss;
Tokenizer *t;
void creation_setup() {
    ss  = ss_fromstring( L"OMG! OMGOMGOMG!" );
    t   = tokenizer_new( ss );
}

void creation_teardown() {
    ss = NULL;
    tokenizer_free( t );
}

//
//  Creation
//
START_TEST (test_tokenizer_create_ss)
{
    fail_unless( t->ss_ == ss, "After creation, StatefulString ought be set correctly." );
}
END_TEST

Suite * tokenizer_suite (void) {
    Suite *s = suite_create( "CSS Tokenizer" );

    //
    //  Creation
    //
    TCase *tc_create = tcase_create( "Creation" );
    tcase_add_checked_fixture (tc_create, creation_setup, creation_teardown);
    tcase_add_test (tc_create, test_tokenizer_create_ss);

    suite_add_tcase (s, tc_create);
  
    return s;
}


int main( void ) {
    int number_failed;
    Suite *s        = tokenizer_suite();
    SRunner *sr     = srunner_create( s );
    srunner_set_fork_status( sr, CK_NOFORK );
    srunner_run_all( sr, CK_NORMAL );
    number_failed   = srunner_ntests_failed( sr );
    srunner_free( sr );
    return ( number_failed == 0 ) ? EXIT_SUCCESS : EXIT_FAILURE;
}
