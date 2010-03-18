#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
char fpeek( FILE *in ) {
    char c;

    c = fgetc( in );
    ungetc( c, in );

    return c;
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

//
//  Composite Parsers
//
char* parseIdentifier( FILE *in ) {
    int  length = 0;
    
    // Allocate some large amount of memory
    char *temp = malloc( ( MAX_STRING_LENGTH + 1 ) * sizeof( char ) );
    if ( temp == NULL ) {
        // If it failed...
        char msg[200];
        sprintf( msg, "Could not allocate %d characters in `parseIdentifier`.", ( MAX_STRING_LENGTH + 1 ) );
        fatal( msg );
    }

    while ( length < MAX_STRING_LENGTH && isAlphanumeric( fpeek( in ) ) ) {
        temp[ length ]  = fget( in );
        length         += 1;
    }
    temp[ length ] = '\0';  // Null terminated strings.

    // Downsize `temp`
    temp = realloc( temp, length * sizeof( char ) );
    if ( temp == NULL ) {
        // If it failed...
        char msg[200];
        sprintf( msg, "Could not downsize from %d to %d characters in `parseIdentifier`.", ( MAX_STRING_LENGTH + 1 ), length );
        fatal( msg );
    }

    return temp;
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

    while ( !feof( in ) ) {
        next = fpeek( in );
        if ( isWhitespace( next ) ) {
            fget( in );              // Throw it away, and continue
            continue;
        }

        if ( isAlpha( next ) ) {
            char *tmp = parseIdentifier( in );
            printf( "`%s` is an identifier!\n", tmp );
        } else if ( isNumeric( next ) ) {
            c = fget( in );
            printf( "`%c` is a number!\n", c );
        } else {
            c = fget( in );
            printf( "`%c` is unknown!\n", c );
        }
    }
    return token;
}
