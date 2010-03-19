#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tokenizer.h"

#define MAX_STRING_LENGTH 1024  // 1k's probably not enough, but it's a start

//
//  Logging
//
void warn( char *msg ) {
    fprintf( stderr, "[WARNING]: %s\n", msg );
}
void error( char *msg ) {
    fprintf( stderr, "[ERROR]: %s\n", msg );
}
void fatal( char *msg ) {
    fprintf( stderr, "[FATAL]: %s\n", msg );
    exit( 1 );
}
//
//  Malloc/Realloc Helper Functions
//
char* allocateLargeString( char *from ) {
    // Allocate some large amount of memory
    char *temp = malloc( ( MAX_STRING_LENGTH + 1 ) * sizeof( char ) );
    if ( temp == NULL ) {
        // If it failed...
        char msg[200];
        sprintf( msg, "Could not allocate %d characters in `%s`.", ( MAX_STRING_LENGTH + 1 ), from );
        fatal( msg );
    }
    return temp;
}

char* downsizeLargeString( char *string, int length, char *from ) {
    // Downsize `temp`
    string = realloc( string, length * sizeof( char ) );
    if ( string == NULL ) {
        // If it failed...
        char msg[200];
        sprintf( msg, "Could not downsize from %d to %d characters in `parseIdentifier`.", ( MAX_STRING_LENGTH + 1 ), length );
        fatal( msg );
    }
    return string;
}

//
//  "Global" state.  This is ugly.
//
unsigned int line_number    = 0;
unsigned int column_number  = 0;

//
//  Get and Peek
//
char fget( FILE *in ) {
    char c;
    if ( !feof( in ) ) {
        c = fgetc( in );
        if ( c == '\n' ) {
            line_number    += 1;
            column_number   = 0;
        } else {
            column_number  += 1;
        }
    } else {
        return EOF;
    }
    return c;
}
char fpeekx( FILE *in, int x ) {
    char *temp = allocateLargeString( "fpeekx" );
    if ( x > MAX_STRING_LENGTH ) {
        error( "`fpeekx` can't look more than MAX_STRING_LENGTH characters ahead." );
        x = MAX_STRING_LENGTH;
    }

    int     i;
    char    value;
    for( i = 0; i < x; i++ ) {
        temp[ i ] = fgetc( in );
    }
    value = temp[ i - 1 ];
    for ( i = i - 1; i >= 0; i-- ) {
        ungetc( temp[ i ], in );
    }
    free( temp );
    return value;
}
char fpeek( FILE *in ) {
    return fpeekx( in, 1 );
}


//
//  Character-Based Tests
//
int isAlpha( char c ) {
    // @TODO: This needs to support unicode.
    return (
        ( c >= 'a' && c <= 'z' ) ||
        ( c >= 'A' && c <= 'Z' )
    );
}
int isNumeric( char c ) {
    return ( c >= '0' && c <= '9' );
}
int isAlphanumeric( char c ) {
    return isAlpha( c ) || isNumeric( c );
}
int isWhitespace( char c ) {
    return (
        c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f'
    );
}
int isQuote( char c ) {
    return (
        c == '"' || c == '\''
    );
}

//
//  Composite Parsers
//
char* parseIdentifier( FILE *in ) {
    assert( isAlpha( fpeek( in ) ) );

    char *temp  = allocateLargeString( "parseIdentifier" );
    int  length = 0;
    
    while ( length < MAX_STRING_LENGTH && isAlphanumeric( fpeek( in ) ) ) {
        temp[ length ]  = fget( in );
        length         += 1;
    }
    temp[ length ] = '\0';  // Null terminated strings.

    return downsizeLargeString( temp, length, "parseIdentifier" );
}
char* parseString( FILE *in ) {
    assert( isQuote( fpeek( in ) ) );

    char *temp  = allocateLargeString( "parseString" );
    int  length = 0;

    char quote = fget( in );
    while ( length < MAX_STRING_LENGTH && fpeek( in ) != quote && fpeek( in ) != EOF ) {
        temp[ length ] = fget( in );
        if ( temp[ length ] == '\\' ) {
            // Tokens are unescaped
            temp[ length ] = fget( in );
        }
        length += 1;
    }
    if ( fpeek( in ) == EOF ) {
        warn( "`parseString` ran off the end of the file.  Something's unmatched..." );
    } else {
        fget( in ); // get the closing quote, and throw it away.
    }

    return downsizeLargeString( temp, length, "parseString" );
}
char* parseComment( FILE *in ) {
    assert( fpeek( in ) == '/' );

    char type;
    char *temp  = allocateLargeString( "parseComment" );
    int  length = 0;
    
    fget( in ); // Throw away leading '/'
    type = fget( in );
    if ( type == '*' ) {
        // Multiline
    } else if ( type == '/' ) {
        // Single line
        while ( length < MAX_STRING_LENGTH && fpeek( in ) != '\r' && fpeek( in ) != '\n' && fpeek( in ) != EOF ) {
            temp[ length ]  = fget( in );
            length         += 1;
        }
    }

    return downsizeLargeString( temp, length, "parseComment" );
}
//
//  Get Next Token
//
//  This does more or less what it says on the tin, and is the only function
//  in this file that's accessable externally.
//
Token get_next_token( FILE *in, FILE *log ) {
    char c, next;
    Token token;

    next = fpeek( in );
    while ( next != EOF ) {
        if ( isWhitespace( next ) ) {
            fget( in );              // Throw it away, and continue
    // Identifiers
        } else if ( isAlpha( next ) ) {
            char *tmp = parseIdentifier( in );
            printf( "`%s` is an identifier!\n", tmp );
    // Strings
        } else if ( isQuote( next ) ) {
            char *tmp = parseString( in );
            printf( "`%s` is a string!\n", tmp );
    // Comments
        } else if ( next == '/' && ( fpeekx( in, 2 ) == '/' || fpeekx( in, 2 ) == '*' ) ) {
            char *tmp = parseComment( in );
            printf( "`%s` is a comment!\n", tmp );
    // Numbers
        } else if ( isNumeric( next ) ) {
            c = fget( in );
            printf( "`%c` is a number!\n", c );
        } else {
            c = fget( in );
            printf( "`%c` is unknown!\n", c );
        }
        next = fpeek( in );
    }
    return token;
}
