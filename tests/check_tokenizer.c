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
Tokenizer *tzr;
Token *t;
void creation_setup() {
    ss  = ss_fromstring( L"OMG! OMGOMGOMG!" );
    tzr = tokenizer_new( ss );
}

void creation_teardown() {
    ss = NULL;
    tokenizer_free( tzr );
}

//
//  Creation
//
START_TEST (test_tokenizer_create_ss)
{
    fail_unless( tzr->ss_ == ss, "After creation, StatefulString ought be set correctly." );
}
END_TEST
START_TEST (test_tokenizer_create_numtokens)
{
    fail_unless( tzr->numtokens == -1, "After creation, numtokens ought be `-1`." );
}
END_TEST
//
//  Tokenization: Token Types
//
Token *faketoken( wchar_t *value, const TokenType type ) {
    StatefulStringPosition pos;
    pos.line   = 0;
    pos.column = 0;
    return token_new( value, wcslen( value ), type, pos, pos );
}
int singleTokenStringIs( wchar_t *s, TokenType type ) {
    int returnvalue;
    ss          = ss_fromstring( s );
    tzr         = tokenizer_new( ss );
    t           = tokenizer_next( tzr );
    returnvalue = token_eq( t, faketoken( s, IDENTIFIER ) );
    ss = NULL;
    token_free( t );
    tokenizer_free( tzr );
    return returnvalue;
}
START_TEST (test_tokenizer_types_identifier_single)
{
    fail_unless(  singleTokenStringIs( L"identifier", IDENTIFIER ) );
    
    fail_unless(  singleTokenStringIs( L"identifier1", IDENTIFIER ), "Numbers are valid at the end of identifiers." );
    fail_unless(  singleTokenStringIs( L"ident1fier", IDENTIFIER ), "Numbers are valid inside identifiers." );
    fail_unless( !singleTokenStringIs( L"1identfier", IDENTIFIER ), "Numbers are _not_ valid at the beginning of identifiers." );

    fail_unless(  singleTokenStringIs( L"identi-fier", IDENTIFIER ), "`-` is valid inside an identifier." );
    fail_unless(  singleTokenStringIs( L"-identifier", IDENTIFIER ), "`-` is valid at the beginning of an identifier." );
    fail_unless( !singleTokenStringIs( L"--identifier", IDENTIFIER ), "`--` is _not_ valid at the beginning of an identifier." );
    fail_unless(  singleTokenStringIs( L"identifier-", IDENTIFIER ), "`-` is valid at the end of an identifier." );

    fail_unless(  singleTokenStringIs( L"identïfier", IDENTIFIER ), "Unicode characters are valid in identifiers." );
    fail_unless(  singleTokenStringIs( L"ïdentifier", IDENTIFIER ), "Unicode characters are valid at the beginning of identifiers." );
    fail_unless(  singleTokenStringIs( L"identifierï", IDENTIFIER ), "Unicode characters are valid at the end of identifiers." );
}
END_TEST



Suite * tokenizer_suite (void) {
    Suite *s = suite_create( "CSS Tokenizer" );

    //
    //  Creation
    //
    TCase *tc_create = tcase_create( "Creation" );
    tcase_add_checked_fixture( tc_create, creation_setup, creation_teardown );
    tcase_add_test( tc_create, test_tokenizer_create_ss );
    tcase_add_test( tc_create, test_tokenizer_create_numtokens );
    suite_add_tcase( s, tc_create );
    //
    //  Tokenization: Token Types
    //
    TCase *tc_types = tcase_create( "Token Types" );
    tcase_add_test( tc_types, test_tokenizer_types_identifier_single );
    suite_add_tcase( s, tc_types );
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
