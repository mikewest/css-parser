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
Token *t1;
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
    fail_unless( tzr->numtokens == 0, "After creation, numtokens ought be `0`." );
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
int processSingleTokenString( wchar_t *s, wchar_t *expected, TokenType type, int printFailure, int printSuccess ) {
    int returnvalue;
    StatefulString *ss  = ss_fromstring( s );
    Tokenizer *tzr      = tokenizer_new( ss );
    Token *t1           = tokenizer_next( tzr );
    Token *t2           = faketoken( expected, type );
    returnvalue         = token_eq( t1, t2 );
    if (
        ( printFailure && !returnvalue ) ||
        ( printSuccess && returnvalue )
    ) {
        wprintf( L"Token 1: `%S` (%d)\n", t1->value, t1->type );
        wprintf( L"Token 2: `%S` (%d)\n", t2->value, t2->type );
    }
    token_free( t1 );
    token_free( t2 );
    tokenizer_free( tzr );
    ss_free( ss );

    return returnvalue;
}
int singleTokenString( wchar_t *s, wchar_t *expected, TokenType type ) {
    return processSingleTokenString( s, expected, type, 1, 0 );
}
int notSingleTokenString( wchar_t *s, wchar_t *expected, TokenType type ) {
    return !processSingleTokenString( s, expected, type, 0, 1 );
}
START_TEST (test_tokenizer_types_identifier_single)
{
    //                                 Tokenize         Expected        Type 
    fail_unless(    singleTokenString( L"identifier",   L"identifier",  IDENTIFIER ) );
    
    fail_unless(    singleTokenString( L"identifier1",  L"identifier1", IDENTIFIER ),  "Numbers are valid at the end of identifiers." );
    fail_unless(    singleTokenString( L"ident1fier",   L"ident1fier",  IDENTIFIER ),  "Numbers are valid inside identifiers." );
    fail_unless( notSingleTokenString( L"1dentifier",   L"1dentifier",  IDENTIFIER ),  "Numbers are _not_ valid at the beginning of identifiers." );

    fail_unless(    singleTokenString( L"identi-fier",  L"identi-fier", IDENTIFIER ), "`-` is valid inside an identifier." );
    fail_unless(    singleTokenString( L"-dentifier",   L"-dentifier",  IDENTIFIER ), "`-` is valid at the beginning of an identifier." );
    fail_unless( notSingleTokenString( L"--dentifier",  L"--dentifier", IDENTIFIER ), "`--` is _not_ valid at the beginning of an identifier." );
    fail_unless(    singleTokenString( L"identifier-",  L"identifier-", IDENTIFIER ), "`-` is valid at the end of an identifier." );

    fail_unless(    singleTokenString( L"identïfier",   L"identïfier",  IDENTIFIER ), "Unicode characters are valid in identifiers." );
    fail_unless(    singleTokenString( L"ïdentifier",   L"ïdentifier",  IDENTIFIER ), "Unicode characters are valid at the beginning of identifiers." );
    fail_unless(    singleTokenString( L"identifieï",   L"identifieï",  IDENTIFIER ), "Unicode characters are valid at the end of identifiers." );
}
END_TEST
START_TEST (test_tokenizer_types_atkeyword_single)
{
    //                                 Tokenize             Expected        Type 
    fail_unless(    singleTokenString( L"@keyword",     L"keyword",     ATKEYWORD ) );
    fail_unless(    singleTokenString( L"@-keyword",    L"-keyword",    ATKEYWORD ), "@keywords can start with a `-`." );
    fail_unless( notSingleTokenString( L"@--keyword",   L"--keyword",   ATKEYWORD ), "@keywords cannot start with a double `-`." );
    fail_unless( notSingleTokenString( L"@4eyword",     L"4eyword",     ATKEYWORD ), "@keywords cannot start with a number." );
}
END_TEST
START_TEST (test_tokenizer_types_hashkeyword_single)
{
    //                                 Tokenize             Expected        Type 
    fail_unless(    singleTokenString( L"#keyword",     L"keyword",     HASHKEYWORD ) );
    fail_unless(    singleTokenString( L"#-keyword",    L"-keyword",    HASHKEYWORD ), "#keywords can start with a `-`." );
    fail_unless(    singleTokenString( L"#--keyword",   L"--keyword",   HASHKEYWORD ), "#keywords can start with a double `-`." );
    fail_unless(    singleTokenString( L"#4eyword",     L"4eyword",     HASHKEYWORD ), "#keywords can start with a number." );
}
END_TEST
START_TEST (test_tokenizer_types_string_single)
{
    //                                 Tokenize             Expected        Type 
    fail_unless(    singleTokenString( L"'string'",         L"string",      STRING ) );
    fail_unless(    singleTokenString( L"\"string\"",       L"string",      STRING ) );
    fail_unless( notSingleTokenString( L"'string\"",        L"string",      STRING ) );
    fail_unless( notSingleTokenString( L"\"string'",        L"string",      STRING ) );


    fail_unless(    singleTokenString( L"'str\\'ing'",      L"str\\'ing",   STRING ) );
    fail_unless(    singleTokenString( L"\"str\\\"ing\"",   L"str\\\"ing",  STRING ) );

    fail_unless(    singleTokenString( L"'strïng'",         L"strïng",      STRING ) );
    fail_unless(    singleTokenString( L"\"strïng\"",       L"strïng",      STRING ) );

    fail_unless( notSingleTokenString( L"'str\ning'",       L"str\ning",    STRING ), "Strings can't contain bare newlines." );
    fail_unless( notSingleTokenString( L"\"str\ning\"",     L"str\ning",    STRING ), "Strings can't contain bare newlines." );
    fail_unless(    singleTokenString( L"'str\\\ning'",     L"str\\\ning",  STRING ), "Strings can contain escaped newlines." );
    fail_unless(    singleTokenString( L"\"str\\\ning\"",   L"str\\\ning",  STRING ), "Strings can contain escaped newlines." );
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
    tcase_add_test( tc_types, test_tokenizer_types_atkeyword_single );
    tcase_add_test( tc_types, test_tokenizer_types_hashkeyword_single );
    tcase_add_test( tc_types, test_tokenizer_types_string_single );
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
