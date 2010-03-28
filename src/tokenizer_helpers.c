#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include "token.h"
#include "tokenizer.h"
#include "tokenizer_helpers.h"

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

FileLocation* getCurrentLocation( void ) {
    FileLocation* current = malloc( sizeof( FileLocation ) );
    current->line   = line_number;
    current->column = column_number;
    return current;
}
void resetCurrentLocation( void ) {
    line_number     = 0;
    column_number   = 0;
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
