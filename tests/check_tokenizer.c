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
        wprintf( L"Token 1: `%S`\t(Length:%d,Error:%d,Type:%d)\n", t1->value, t1->length, ( t1->error != NULL ), t1->type );
        if ( t1->error ) {
            wprintf( L"-   ERROR: '%S'\n", ( t1->error )->message );
        }
        wprintf( L"Token 2: `%S`\t(Length:%d,Error:%d,Type:%d)\n", t2->value, t2->length, ( t2->error != NULL ), t2->type );
        if ( t2->error ) {
            wprintf( L"-   ERROR: '%S'\n", ( t2->error )->message );
        }
    }
    token_free( t1 );
    t1 = NULL;
    token_free( t2 );
    t2 = NULL;
    tokenizer_free( tzr );
    ss_free( ss );

    return returnvalue;
}
int errSingleTokenString( wchar_t *s, wchar_t *expected, TokenType type ) {
    int returnvalue;
    StatefulString *ss  = ss_fromstring( s );
    Tokenizer *tzr      = tokenizer_new( ss );
    Token *t1           = tokenizer_next( tzr );
    Token *t2           = faketoken( expected, type );
    t2->error           = malloc( sizeof( TokenError ) );
    returnvalue         = token_eq( t1, t2 );
    if (
        !returnvalue
    ) {
        wprintf( L"Token 1: `%S`\t(Length:%d,Error:%d,Type:%d)\n", t1->value, t1->length, ( t1->error != NULL ), t1->type );
        wprintf( L"-   NO ERROR.\n" );
        wprintf( L"Token 2: `%S`\t(Length:%d,Error:%d,Type:%d)\n", t2->value, t2->length, ( t2->error != NULL ), t2->type );
    }
    token_free( t1 );
    t1 = NULL;
    token_free( t2 );
    t2 = NULL;
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
START_TEST( test_tokenizer_types_whitespace_single )
{
    //                                 Tokenize         Expected        Type
    fail_unless(    singleTokenString( L" ",            L" ",           WHITESPACE ) );
    fail_unless(    singleTokenString( L"\n",           L"\n",          WHITESPACE ) );
    fail_unless(    singleTokenString( L"\r",           L"\r",          WHITESPACE ) );
    fail_unless(    singleTokenString( L"\t",           L"\t",          WHITESPACE ) );
    fail_unless(    singleTokenString( L"\f",           L"\f",          WHITESPACE ) );
    fail_unless(    singleTokenString( L"\r\n",         L"\r\n",        WHITESPACE ) );
    fail_unless(    singleTokenString( L" \r\t\n   ",   L" \r\t\n   ",  WHITESPACE ) );
}
END_TEST
START_TEST( test_tokenizer_types_identifier_single )
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
START_TEST( test_tokenizer_types_function_single )
{
    //                                 Tokenize         Expected        Type 
    fail_unless(    singleTokenString( L"identifier(",  L"identifier(",     FUNCTION ) );
    
    fail_unless(    singleTokenString( L"identifier1(", L"identifier1(",    FUNCTION ),  "Numbers are valid at the end of identifiers." );
    fail_unless(    singleTokenString( L"ident1fier(",  L"ident1fier(",     FUNCTION ),  "Numbers are valid inside identifiers." );
    fail_unless( notSingleTokenString( L"1dentifier(",  L"1dentifier(",     FUNCTION ),  "Numbers are _not_ valid at the beginning of identifiers." );

    fail_unless(    singleTokenString( L"identi-fier(", L"identi-fier(",    FUNCTION ), "`-` is valid inside an identifier." );
    fail_unless(    singleTokenString( L"-dentifier(",  L"-dentifier(",     FUNCTION ), "`-` is valid at the beginning of an identifier." );
    fail_unless( notSingleTokenString( L"--dentifier(", L"--dentifier(",    FUNCTION ), "`--` is _not_ valid at the beginning of an identifier." );
    fail_unless(    singleTokenString( L"identifier-(", L"identifier-(",    FUNCTION ), "`-` is valid at the end of an identifier." );

    fail_unless(    singleTokenString( L"identïfier(",  L"identïfier(",     FUNCTION ), "Unicode characters are valid in identifiers." );
    fail_unless(    singleTokenString( L"ïdentifier(",  L"ïdentifier(",     FUNCTION ), "Unicode characters are valid at the beginning of identifiers." );
    fail_unless(    singleTokenString( L"identifieï(",  L"identifieï(",     FUNCTION ), "Unicode characters are valid at the end of identifiers." );
}
END_TEST
START_TEST( test_tokenizer_types_atkeyword_single )
{
    //                                 Tokenize         Expected        Type 
    fail_unless(    singleTokenString( L"@keyword",     L"keyword",     ATKEYWORD ) );
    fail_unless(    singleTokenString( L"@-keyword",    L"-keyword",    ATKEYWORD ), "@keywords can start with a `-`." );
    fail_unless( notSingleTokenString( L"@--keyword",   L"--keyword",   ATKEYWORD ), "@keywords cannot start with a double `-`." );
    fail_unless( notSingleTokenString( L"@4eyword",     L"4eyword",     ATKEYWORD ), "@keywords cannot start with a number." );
}
END_TEST
START_TEST( test_tokenizer_types_hashkeyword_single )
{
    //                                 Tokenize         Expected        Type 
    fail_unless(    singleTokenString( L"#keyword",     L"keyword",     HASHKEYWORD ) );
    fail_unless(    singleTokenString( L"#-keyword",    L"-keyword",    HASHKEYWORD ), "#keywords can start with a `-`." );
    fail_unless(    singleTokenString( L"#--keyword",   L"--keyword",   HASHKEYWORD ), "#keywords can start with a double `-`." );
    fail_unless(    singleTokenString( L"#4eyword",     L"4eyword",     HASHKEYWORD ), "#keywords can start with a number." );
}
END_TEST
START_TEST( test_tokenizer_types_string_single )
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
START_TEST( test_tokenizer_types_number_single )
{
    //                                 Tokenize     Expected    Type
    fail_unless(    singleTokenString( L"1",        L"1",       NUMBER ) );
    fail_unless(    singleTokenString( L"1234",     L"1234",    NUMBER ) );
    fail_unless(    singleTokenString( L"12.34",    L"12.34",   NUMBER ) );
    fail_unless(    singleTokenString( L"-1",       L"-1",      NUMBER ) );
    fail_unless(    singleTokenString( L"-1234",    L"-1234",   NUMBER ) );
    fail_unless(    singleTokenString( L"-12.34",   L"-12.34",  NUMBER ) );
    fail_unless( notSingleTokenString( L"-12.3.4",  L"-12.3.4", NUMBER ), "Numbers can't contain more than one decimal point." );
    fail_unless( notSingleTokenString( L"-12-3.4",  L"-12-3.4", NUMBER ), "Numbers can't contain more than one negation." );
}
END_TEST
START_TEST( test_tokenizer_types_percentage_single )
{
    //                                 Tokenize     Expected        Type
    fail_unless(    singleTokenString( L"1%",       L"1%",          PERCENTAGE ) );
    fail_unless(    singleTokenString( L"1234%",    L"1234%",       PERCENTAGE ) );
    fail_unless(    singleTokenString( L"12.34%",   L"12.34%",      PERCENTAGE ) );
    fail_unless(    singleTokenString( L"-1%",      L"-1%",         PERCENTAGE ) );
    fail_unless(    singleTokenString( L"-1234%",   L"-1234%",      PERCENTAGE ) );
    fail_unless(    singleTokenString( L"-12.34%",  L"-12.34%",     PERCENTAGE ) );
    fail_unless( notSingleTokenString( L"-12.3.4%", L"-12.3.4%",    PERCENTAGE ), "Percentages can't contain more than one decimal point." );
    fail_unless( notSingleTokenString( L"-12-3.4%", L"-12-3.4%",    PERCENTAGE ), "Percentages can't contain more than one negation." );
}
END_TEST
START_TEST( test_tokenizer_types_dimension_single )
{
    //                                 Tokenize         Expected        Type
    fail_unless(    singleTokenString( L"1em",          L"1em",         DIMENSION ) );
    fail_unless(    singleTokenString( L"1234em",       L"1234em",      DIMENSION ) );
    fail_unless(    singleTokenString( L"12.34em",      L"12.34em",     DIMENSION ) );
    fail_unless(    singleTokenString( L"-1em",         L"-1em",        DIMENSION ) );
    fail_unless(    singleTokenString( L"-1234em",      L"-1234em",     DIMENSION ) );
    fail_unless(    singleTokenString( L"-12.34em",     L"-12.34em",    DIMENSION ) );
    fail_unless( notSingleTokenString( L"-12.3.4em",    L"-12.3.4em",   DIMENSION ), "Dimensions can't contain more than one decimal point." );
    fail_unless( notSingleTokenString( L"-12-3.4em",    L"-12-3.4em",   DIMENSION ), "Dimensions can't contain more than one negation." );
}
END_TEST
START_TEST( test_tokenizer_types_url_single )
{
    //                                 Tokenize             Expected                Type
    fail_unless(    singleTokenString( L"url(/)",           L"url(/)",              URL ) );
    fail_unless(    singleTokenString( L"url(omg)",         L"url(omg)",            URL ) );
    fail_unless(    singleTokenString( L"url( omg)",        L"url( omg)",           URL ) );
    fail_unless(    singleTokenString( L"url(omg )",        L"url(omg )",           URL ) );
    fail_unless(    singleTokenString( L"url( omg )",       L"url( omg )",          URL ) );
    fail_unless(    singleTokenString( L"url('omg')",       L"url('omg')",          URL ) );
    fail_unless(    singleTokenString( L"url('omg' )",      L"url('omg' )",         URL ) );
    fail_unless(    singleTokenString( L"url( 'omg')",      L"url( 'omg')",         URL ) );
    fail_unless(    singleTokenString( L"url( 'omg' )",     L"url( 'omg' )",        URL ) );
    fail_unless(    singleTokenString( L"url(\"omg\")",     L"url(\"omg\")",        URL ) );
    fail_unless(    singleTokenString( L"url(\"omg\" )",    L"url(\"omg\" )",       URL ) );
    fail_unless(    singleTokenString( L"url( \"omg\")",    L"url( \"omg\")",       URL ) );
    fail_unless(    singleTokenString( L"url( \"omg\" )",   L"url( \"omg\" )",      URL ) );

    fail_unless( errSingleTokenString( L"url(')",           L"url(')",      URL ), "Unclosed strings should error off." );
    fail_unless( errSingleTokenString( L"url(\"')",         L"url(\"')",    URL ), "Unclosed strings should error off." );
    fail_unless( errSingleTokenString( L"url(\")",          L"url(\")",     URL ), "Unclosed strings should error off." );
}
END_TEST
START_TEST( test_tokenizer_types_cdocdc_single )
{
    //                                 Tokenize     Expected    Type
    fail_unless(    singleTokenString( L"<!--",     L"<!--",    SGML_COMMENT_OPEN ) );
    fail_unless(    singleTokenString( L"-->",      L"-->",     SGML_COMMENT_CLOSE ) );
}
END_TEST
START_TEST( test_tokenizer_types_comment_single )
{
    //                                 Tokenize             Expected                Type
    fail_unless(    singleTokenString( L"/**/",             L"/**/",                COMMENT ) );
    fail_unless(    singleTokenString( L"/* comment */",    L"/* comment */",       COMMENT ) );
    fail_unless(    singleTokenString( L"/* \\*/ */",       L"/* \\*/ */",          COMMENT ) );
    fail_unless( errSingleTokenString( L"/* \\*/abc",       L"/* \\*/abc",          COMMENT ) );
}
END_TEST
START_TEST( test_tokenizer_types_operator_single )
{
    //                                 Tokenize     Expected    Type
    fail_unless(    singleTokenString( L":",        L":",       COLON ) );
    fail_unless(    singleTokenString( L";",        L";",       SEMICOLON ) );
    fail_unless(    singleTokenString( L"{",        L"{",       CURLY_BRACE_OPEN ) );
    fail_unless(    singleTokenString( L"}",        L"}",       CURLY_BRACE_CLOSE ) );
    fail_unless(    singleTokenString( L"[",        L"[",       SQUARE_BRACE_OPEN ) );
    fail_unless(    singleTokenString( L"]",        L"]",       SQUARE_BRACE_CLOSE ) );
    fail_unless(    singleTokenString( L"(",        L"(",       PAREN_OPEN ) );
    fail_unless(    singleTokenString( L")",        L")",       PAREN_CLOSE ) );
    fail_unless(    singleTokenString( L"@",        L"@",       AT ) );
    fail_unless(    singleTokenString( L"%",        L"%",       PERCENT ) );
    fail_unless(    singleTokenString( L"#",        L"#",       HASH ) );
    fail_unless(    singleTokenString( L".",        L".",       DOT ) );
    fail_unless(    singleTokenString( L"~=",       L"~=",      INCLUDES ) );
    fail_unless(    singleTokenString( L"|=",       L"|=",      DASHMATCH) );
    fail_unless(    singleTokenString( L"^=",       L"^=",      PREFIXMATCH ) );
    fail_unless(    singleTokenString( L"$=",       L"$=",      SUFFIXMATCH ) );
    fail_unless(    singleTokenString( L"*=",       L"*=",      SUBSTRINGMATCH ) );
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
    tcase_add_test( tc_types, test_tokenizer_types_whitespace_single );
    tcase_add_test( tc_types, test_tokenizer_types_identifier_single );
    tcase_add_test( tc_types, test_tokenizer_types_function_single );
    tcase_add_test( tc_types, test_tokenizer_types_atkeyword_single );
    tcase_add_test( tc_types, test_tokenizer_types_hashkeyword_single );
    tcase_add_test( tc_types, test_tokenizer_types_string_single );
    tcase_add_test( tc_types, test_tokenizer_types_number_single );
    tcase_add_test( tc_types, test_tokenizer_types_percentage_single );
    tcase_add_test( tc_types, test_tokenizer_types_dimension_single );
    tcase_add_test( tc_types, test_tokenizer_types_url_single );
    tcase_add_test( tc_types, test_tokenizer_types_cdocdc_single );
    tcase_add_test( tc_types, test_tokenizer_types_comment_single );
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
