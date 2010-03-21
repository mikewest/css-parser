#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>
#include "logging.h"
#include "token.h"
#include "tokenizer.h"
#include "tokenizer_charactertests.h"

#define MAX_STRING_LENGTH 1024  // 1k's probably not enough, but it's a start

//
//  Malloc/Realloc Helper Functions
//
wchar_t* allocateLargeString( wchar_t *from ) {
    // Allocate some large amount of memory
    wchar_t *temp = malloc( ( MAX_STRING_LENGTH + 1 ) * sizeof( wchar_t ) );
    if ( temp == NULL ) {
        // If it failed...
        wchar_t msg[200];
        swprintf( msg, 200, L"Could not allocate %d characters in `%s`.", ( MAX_STRING_LENGTH + 1 ), from );
        fatal( msg );
    }
    return temp;
}

wchar_t* downsizeLargeString( wchar_t *string, int length, wchar_t *from ) {
    // Downsize `temp`
    string = realloc( string, length * sizeof( wchar_t ) );
    if ( string == NULL ) {
        // If it failed...
        wchar_t msg[200];
        swprintf( msg, 200, L"Could not downsize from %d to %d characters in `parseIdentifier`.", ( MAX_STRING_LENGTH + 1 ), length );
        fatal( msg );
    }
    return string;
}

//
//  "Global" state.  This is ugly.
//
unsigned int line_number    = 0;
unsigned int column_number  = 0;

FileLocation* getCurrentLocation() {
    FileLocation* current = malloc( sizeof( FileLocation ) );
    current->line   = line_number;
    current->column = column_number;
    return current;
}


//
//  Get and Peek
//
wchar_t fget( FILE *in ) {
    wint_t  c;
    if ( !feof( in ) ) {
        c = fgetwc( in );
        if ( c == '\n' ) {
            line_number    += 1;
            column_number   = 0;
        } else {
            column_number  += 1;
        }
    } else {
        return WEOF;
    }
    return (wchar_t) c;
}
wchar_t fpeekx( FILE *in, int x ) {
    wchar_t *temp = allocateLargeString( L"fpeekx" );
    if ( x > MAX_STRING_LENGTH ) {
        error( L"`fpeek` can't look more than MAX_STRING_LENGTH characters ahead." );
        x = MAX_STRING_LENGTH;
    }

    int        i;
    wchar_t    value;
    for( i = 0; i < x; i++ ) {
        temp[ i ] = fgetwc( in );
    }
    value = temp[ i - 1 ];
    for ( i = i - 1; i >= 0; i-- ) {
        ungetwc( temp[ i ], in );
    }
    free( temp );
    return value;
}
wchar_t fpeek( FILE *in ) {
    return fpeekx( in, 1 );
}



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

    wchar_t type;
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
    //  5.   Numbers
        } else if ( isNumeric( next ) ) {
            token = parse( &parseNumeric, in, NUMBER );
    //  
        } else if ( isOperator( next ) ) {
            fget( in ); 
        } else {
            c = fget( in );
            wprintf( L"`%c` is unknown!\n", c );
        }
        next = fpeek( in );
    }
    if ( token ) {
        return token;
    } else {
        return NULL;
    }
}
