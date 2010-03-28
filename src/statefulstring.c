#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include "logging.h"
#include "statefulstring.h"

#define BASE_STRING_LENGTH  2   //  Start with 10k, malloc from there.
#define ERROR_ALLOCATION    -1

void allocationerror( unsigned int size, wchar_t* where ) {
        wchar_t msg[200];
        wchar_t *format = L"Could not allocate %d characters in `%s`.";
        swprintf( msg, 200, format, size, where );
        printf( "%s", msg );
}

void addlinebreak( StatefulString* ss, StatefulStringPosition position ) {
    printf( "Do something useful here: ( %d, %d )\n", position.line, position.column );
}

int readstream( FILE* stream, wchar_t** temp, unsigned int* length ) {
    unsigned int max_length = BASE_STRING_LENGTH;
    *length                 = 0;
    
    *temp = malloc( ( max_length + 1 ) * sizeof( wchar_t ) );
    if ( *temp == NULL ) {
        allocationerror( ( max_length + 1 ), L"StatefulString::readstream" );
        return ERROR_ALLOCATION;
    }
    
    while ( ( ( *temp )[ ( *length )++ ] = fgetwc( stream ) ) != WEOF ) { 
        if ( ( *length ) == max_length ) {
            max_length *= 2;
            ( *temp )   = realloc( ( *temp ), ( max_length + 1 ) * sizeof( wchar_t ) );
            if ( ( *temp ) == NULL ) {
                allocationerror( max_length, L"StatefulString::readstream" );
                return ERROR_ALLOCATION;
            }
        }
    }
    ( *temp )[ *length ] = L'\0';
    ( *length )--;
}

StatefulString *ss_fromstream( FILE* stream ) {
    StatefulString *ss  = malloc( sizeof( StatefulString ) );

    ss->length                      = 0;
    ss->current_index               = 0;
    ( ss->current_position ).line   = 0;
    ( ss->current_position ).column = 0;
    readstream( stream, &( ss->value ), &( ss->length ) );

    return ss;
}

wchar_t ss_getchar( StatefulString* ss ) {
    if ( ss->current_index >= ss->length ) {
        return WEOF;
    } else {
        if ( ss->value[ ss->current_index ] == L'\n' ) {
            ( ss->current_position ).line++;
            ( ss->current_position ).column = 0;
            addlinebreak( ss, ss->current_position );
        } else {
            ( ss->current_position ).column++;
        }
        ss->current_index++;
        return ss->value[ ss->current_index - 1 ];
    }
}

/**
 *  Peek at the next character in the string without
 *  changing its state.
 */
wchar_t ss_peek( StatefulString* ss ) {
    return ss_peekx( ss, 1 );
}
/**
 *  Peek at the the Xth-next character in the string without
 *  changing its state
 */
wchar_t ss_peekx( StatefulString* ss, const unsigned int x ) {
    if ( ( ss->current_index + x ) >= ss->length ) {
        return WEOF;
    } else {
        return ss->value[ ss->current_index + x ];
    }    
}
