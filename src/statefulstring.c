#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include "logging.h"
#include "statefulstring.h"

#define BASE_STRING_LENGTH  2       //  Start with 10k, malloc from there.
#define BASE_LINES          1000    //  Start with 1k, malloc from there.
#define ERROR_ALLOCATION    -1

void allocationerror( unsigned int size, wchar_t* where ) {
        wchar_t msg[200];
        wchar_t *format = L"Could not allocate %d characters in `%s`.";
        swprintf( msg, 200, format, size, where );
        printf( "%s", msg );
}

void addlinebreak( StatefulString* ss, int index ) {
    ( ss->lines )++;
    ss->linebreaks[ ss->lines++ ] = index;
    // @TODO: Allocate more memory if we exceed `BASE_LINES`.
}

int readstream( FILE* stream, StatefulString* ss ) {
    unsigned int max_length = BASE_STRING_LENGTH;
    wchar_t *temp = malloc( ( max_length + 1 ) * sizeof( wchar_t ) );
    int length = 0;

    if ( temp == NULL ) {
        allocationerror( ( max_length + 1 ), L"StatefulString::readstream" );
        return ERROR_ALLOCATION;
    }
    
    while ( ( temp[ length++ ] = fgetwc( stream ) ) != WEOF ) { 
        if ( temp[ length - 1 ] == L'\n'  ) {
            addlinebreak( ss, length - 1 );
        }
        if ( length == max_length ) {
            max_length *= 2;
            temp        = realloc( temp, ( max_length + 1 ) * sizeof( wchar_t ) );
            if ( temp == NULL ) {
                allocationerror( max_length, L"StatefulString::readstream" );
                return ERROR_ALLOCATION;
            }
        }
    }
    length--;
    temp[ length ] = L'\0';

    ss->value   = temp;
    ss->length  = length;
}

StatefulString *ss_fromstream( FILE* stream ) {
    StatefulString *ss  = malloc( sizeof( StatefulString ) );

    ss->length                      = 0;
    ss->lines                       = 1; // Every file has a line.
    ss->next_index                  = 0;
    ( ss->next_position ).line      = 0;
    ( ss->next_position ).column    = 0;
    ss->linebreaks                  = malloc( ( BASE_LINES + 1 ) * sizeof( unsigned int ) );
    readstream( stream, ss );

    return ss;
}

void ss_free( StatefulString* ss ) {
    free( ss->value );
    free( ss->linebreaks );
    free( ss );
}

wchar_t ss_getchar( StatefulString* ss ) {
    if ( ss->next_index >= ss->length ) {
        return WEOF;
    } else {
        if ( ss->value[ ss->next_index ] == L'\n' ) {
            ( ss->next_position ).line++;
            ( ss->next_position ).column = 0;
        } else {
            ( ss->next_position ).column++;
        }
        ss->next_index++;
        return ss->value[ ss->next_index - 1 ];
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
    if ( ( ss->next_index + x ) >= ss->length ) {
        return WEOF;
    } else {
        return ss->value[ ss->next_index + x ];
    }    
}
