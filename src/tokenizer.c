#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>
#include "logging.h"
#include "token.h"
#include "tokenizer_charactertests.h"
#include "tokenizer_helpers.h"
#include "tokenizer.h"

//
//  Composite Parsers
//
wchar_t* parseIdentifier( FILE *in ) {
    assert( isIdentifierStart( fpeek( in ) ) );

    wchar_t *temp  = allocateLargeString( L"parseIdentifier" );
    int  length = 0;
    
    while ( length < MAX_STRING_LENGTH && isIdentifier( fpeek( in ) ) ) {
        temp[ length ]  = fget( in );
        length         += 1;
    }

    temp[ length ] = L'\0';
    return downsizeLargeString( temp, length, L"parseIdentifier" );
}
wchar_t* parseNumeric( FILE *in ) {
    assert( isNumeric( fpeek( in ) ) );

    wchar_t *temp  = allocateLargeString( L"parseNumeric" );
    int  length = 0;

    while ( length < MAX_STRING_LENGTH && isNumeric( fpeek( in ) ) ) {
        temp[ length ]  = fget( in );
        length         += 1;
    }

    temp[ length ] = L'\0';
    return downsizeLargeString( temp, length, L"parseNumeric" );
}
wchar_t* parseString( FILE *in ) {
    assert( isQuote( fpeek( in ) ) );

    wchar_t *temp  = allocateLargeString( L"parseString" );
    int  length = 0;

    wchar_t quote = fget( in );
    while ( length < MAX_STRING_LENGTH && fpeek( in ) != quote && fpeek( in ) != WEOF ) {
        temp[ length ] = fget( in );
        if ( temp[ length ] == L'\\' ) {
            // Tokens are unescaped
            temp[ length ] = fget( in );
        }
        length += 1;
    }
    if ( fpeek( in ) == WEOF ) {
        warn( L"`parseString` ran off the end of the file.  Something's unmatched..." );
    } else {
        fget( in ); // get the closing quote, and throw it away.
    }
    temp[ length ] = L'\0';
    return downsizeLargeString( temp, length, L"parseString" );
}
wchar_t* parseComment( FILE *in ) {
    assert( fpeek( in ) == L'/' );

    wchar_t *temp  = allocateLargeString( L"parseComment" );
    int  length = 0;
    
    fget( in ); // Throw away leading '/*'
    fget( in );
    while ( length < MAX_STRING_LENGTH && ( fpeek( in ) != L'*' || fpeekx( in, 2 ) != L'/' ) && fpeek( in ) != WEOF ) {
        temp[ length ] = fget( in );
        if ( temp[ length ] == L'\\' ) {
            temp[ length ] = fget( in );
        }
        length += 1;
    }
    if ( fpeek( in ) == WEOF ) {
        warn( L"`parseComment` ran off the end of the file.  There's an unmatched `/*`." );
    } else {
        fget( in ); // Throw away trailing `*/`.
        fget( in );
    }

    temp[ length ] = L'\0';
    return downsizeLargeString( temp, length, L"parseComment" );
}
wchar_t* parseOperator( FILE *in ) {
    assert( isOperator( fpeek( in ) ) );

    wchar_t *temp  = allocateLargeString( L"parseOperator" );
    int  length = 1;
    
    temp[ 0 ] = fget( in ); 

    if (
        ( temp[ 0 ] == L'~' || temp[ 0 ] == L'|' || temp[ 0 ] == L'*' || temp[ 0 ] == L'^' || temp[ 0 ] == L'$' ) &&
        fpeek( in ) == L'='
    ) {
        temp[ 1 ]   = fget( in );
        length      = 2;
    }
    
    temp[ length ] = L'\0';
    return downsizeLargeString( temp, length, L"parseComment" );
}
void categorize_known_operators( Token *t ) {
    if ( wcscmp( t->value, L"~=" ) == 0 ) {
            t->type = INCLUDES;
    } else if ( wcscmp( t->value, L"|=" ) == 0 ) {
            t->type = DASHMATCH;
    } else if ( wcscmp( t->value, L"^=" ) == 0 ) {
            t->type = PREFIXMATCH;
    } else if ( wcscmp( t->value, L"$=" ) == 0 ) {
            t->type = SUFFIXMATCH;
    } else if ( wcscmp( t->value, L"*=" ) == 0 ) {
            t->type = SUBSTRINGMATCH;
    } else if ( wcscmp( t->value, L"{" ) == 0 ) {
            t->type = CURLY_BRACE_OPEN;
    } else if ( wcscmp( t->value, L"}" ) == 0 ) {
            t->type = CURLY_BRACE_CLOSE;
    } else if ( wcscmp( t->value, L"[" ) == 0 ) {
            t->type = SQUARE_BRACE_OPEN;
    } else if ( wcscmp( t->value, L"]" ) == 0 ) {
            t->type = SQUARE_BRACE_CLOSE;
    } else if ( wcscmp( t->value, L"(" ) == 0 ) {
            t->type = PAREN_OPEN;
    } else if ( wcscmp( t->value, L")" ) == 0 ) {
            t->type = PAREN_CLOSE;
    } else if ( wcscmp( t->value, L":" ) == 0 ) {
            t->type = COLON;
    } else if ( wcscmp( t->value, L";" ) == 0 ) {
            t->type = SEMICOLON;
    } else if ( wcscmp( t->value, L"@" ) == 0 ) {
            t->type = AT;
    } else if ( wcscmp( t->value, L"%" ) == 0 ) {
            t->type = PERCENT;
    } else if ( wcscmp( t->value, L"#" ) == 0 ) {
            t->type = HASH;
    } else if ( wcscmp( t->value, L"." ) == 0 ) {
            t->type = DOT;
    } else {
        wprintf( L"`%S` didn't match anything.", t->value );
    }
}
wchar_t* parseDelim( FILE *in ) {
    wchar_t *temp = malloc( 2 * sizeof( wchar_t ) );
    temp[ 0 ] = fget( in );
    temp[ 1 ] = L'\0';
    return temp;    
}

//
//  Given a parsing function, build a Token
//
Token* parse( wchar_t *parser( FILE* ), FILE *in, token_type type ) {
    FileLocation *start, *end;
    wchar_t *value;
    start = getCurrentLocation();
    value = parser( in );
    end = getCurrentLocation();

    return generate_token( value, type, start, end ); 
}


//
//  Get Next Token
//
//  This does more or less what it says on the tin, and is the only function
//  in this file that's accessable externally.
//
Token* get_next_token( FILE *in, FILE *log ) {
    wchar_t c, next;
    Token *token = NULL;

    next = fpeek( in );
    while ( next != EOF && !token ) {
    //  1.  Throw away whitespace outside of tokens
        if ( isWhitespace( next ) ) {
            fget( in );
    //  2.  Comments
        } else if ( next == '/' && fpeekx( in, 2 ) == '*' ) {
            token = parse( &parseComment, in, COMMENT );
    //  3.  Strings
        } else if ( isQuote( next ) ) {
            token = parse( &parseString, in, STRING );

    //  4.  Identifiers
        } else if ( isIdentifierStart( next ) ) {
            token = parse( &parseIdentifier, in, IDENTIFIER );
    //  5.  Numbers
        } else if ( isNumeric( next ) ) {
            token = parse( &parseNumeric, in, NUMBER );
    //  6.  Known Operators
        } else if ( isOperator( next ) ) {
            token = parse( &parseOperator, in, DELIM );
            categorize_known_operators( token );
    //  7.  Otherwise...
        } else {
            token = parse( &parseDelim, in, DELIM );
        }
        next = fpeek( in );
    }
    if ( token ) {
        return token;
    } else {
        return NULL;
    }
}
