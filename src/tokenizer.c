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
int parseIdentifier( FILE *in, wchar_t* temp ) {
    assert( isIdentifierStart( fpeek( in ) ) );

    int  length = 0;
    
    while ( length < MAX_STRING_LENGTH && isIdentifier( fpeek( in ) ) ) {
        temp[ length ]  = fget( in );
        length++;
    }

    temp[ length ] = L'\0';
    return length;
}
int parseNumeric( FILE *in, wchar_t* temp) {
    assert( isNumeric( fpeek( in ) ) );

    int  length = 0;

    while ( length < MAX_STRING_LENGTH && isNumeric( fpeek( in ) ) ) {
        temp[ length ]  = fget( in );
        length++;
    }

    temp[ length ] = L'\0';
    return length;
}
int parseString( FILE *in, wchar_t* temp  ) {
    assert( isQuote( fpeek( in ) ) );

    int  length = 0;

    wchar_t quote = fget( in );
    while ( length < MAX_STRING_LENGTH && fpeek( in ) != quote && fpeek( in ) != WEOF ) {
        temp[ length ] = fget( in );
        if ( temp[ length ] == L'\\' ) {
            // Tokens are unescaped
            temp[ length ] = fget( in );
        }
        length++;
    }
    if ( fpeek( in ) == WEOF ) {
        warn( L"`parseString` ran off the end of the file.  Something's unmatched..." );
    } else {
        fget( in ); // get the closing quote, and throw it away.
    }
    temp[ length ] = L'\0';
    return length;
}
int parseComment( FILE *in, wchar_t* temp ) {
    assert( fpeek( in ) == L'/' );

    int  length = 0;
    
    fget( in ); // Throw away leading '/*'
    fget( in );
    while ( length < MAX_STRING_LENGTH && ( fpeek( in ) != L'*' || fpeekx( in, 2 ) != L'/' ) && fpeek( in ) != WEOF ) {
        temp[ length ] = fget( in );
        if ( temp[ length ] == L'\\' ) {
            temp[ length ] = fget( in );
        }
        length++;
    }
    if ( fpeek( in ) == WEOF ) {
        warn( L"`parseComment` ran off the end of the file.  There's an unmatched `/*`." );
    } else {
        fget( in ); // Throw away trailing `*/`.
        fget( in );
    }

    temp[ length ] = L'\0';
    return length;
}
int parseOperator( FILE *in, wchar_t* temp ) {
    assert( isOperator( fpeek( in ) ) );

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
    return length;
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
int parseDelim( FILE *in, wchar_t* temp ) {
    temp[ 0 ] = fget( in );
    temp[ 1 ] = L'\0';
    return 1;
}

//
//  Given a parsing function, build a Token
//
Token* parse( int parser( FILE*, wchar_t* ), FILE *in, token_type type ) {
    FileLocation *start, *end;
    
    wchar_t *temp  = allocateLargeString( L"parse" );
    int     length = 0;

    start   = getCurrentLocation();
    length  = parser( in, temp );
    end     = getCurrentLocation();

    temp    = downsizeLargeString( temp, length, L"parse" );

    return new_token( temp, type, start, end ); 
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
    //  2.  Strings (Before comments, as strings can contain comment-like sequences)
        } else if ( isQuote( next ) ) {
            token = parse( &parseString, in, STRING );
    //  3.  Comments
        } else if ( next == L'/' && fpeekx( in, 2 ) == L'*' ) {
            token = parse( &parseComment, in, COMMENT );
    //  4.  Identifiers
        } else if ( isIdentifierStart( next ) ) {
            token = parse( &parseIdentifier, in, IDENTIFIER );
    //  5.  @keywords
        } else if ( next == L'@' && isIdentifierStart( fpeekx( in, 2 ) ) ) {
            fget( in ); // Thow away the `@`, and parse an identifier
            token = parse( &parseIdentifier, in, ATKEYWORD );
    //  6.  HASH
        } else if ( next == L'#' && isIdentifier( fpeekx( in, 2 ) ) ) {
            fget( in ); // Throw away the `#`, and parse an identifier
            token = parse( &parseIdentifier, in, HASH );
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

        // Postprocess for postfixed tokens:
        if ( token && token->type == NUMBER && next == L'%' ) {
            fget( in ); // Throw away the '%'
            token->type = PERCENTAGE;
            token->end  = getCurrentLocation();
        }
    }
    if ( token ) {
        return token;
    } else {
        return NULL;
    }
}
