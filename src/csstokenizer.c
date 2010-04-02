#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include <check.h>
#include "../src/logging.h"
#include "../src/statefulstring.h"
#include "../src/token.h"
#include "../src/csstokenizer.h"
#include "../src/tokenizer_charactertests.h"

#define BASE_MAXTOKENS  1024

//  @constructor
Tokenizer *tokenizer_new( StatefulString *ss ) {
    Tokenizer *temp = malloc( sizeof( Tokenizer ) );

    temp->numtokens     = -1;
    temp->ss_           = ss;
    temp->maxtokens_    = BASE_MAXTOKENS;
    temp->tokens_       = malloc( ( BASE_MAXTOKENS + 1 ) * sizeof( Token* ) );
    return temp;
}
//  @destructor
void tokenizer_free( Tokenizer *t ) {
    free( t->ss_ );
    free( t->tokens_ );
    free( t );
}

Token *parseIdentifier( StatefulString *ss ) {
    assert( isIdentifierStart( ss_peek( ss ) ) );

    int start, length;
    StatefulStringPosition pos1, pos2;
    wchar_t temp;

    start   = ss->next_index;
    length  = 0;
    pos1    = ss->next_position;
    while ( isIdentifier( temp = ss_getchar( ss ) ) ) {
        length++;
    }
    pos2    = ss->next_position;
    return token_new( ss_substr( ss, start, length ), length, IDENTIFIER, pos1, pos2 );
}

Token *tokenizer_next( Tokenizer *tokenizer ) {
    wchar_t c, next;
    Token *token = NULL;

    next = ss_peek( tokenizer->ss_ );
    while ( next != WEOF && !token ) {
        // @TODO: Think about pulling these character tests out into StatefulString tests...
        if ( isIdentifierStart( next ) && isIdentifier( ss_peekx( tokenizer->ss_, 1 ) ) && ss_peekx( tokenizer->ss_, 1 ) != L'-' ) {
            token = parseIdentifier( tokenizer->ss_ );
        } else {
            next = ss_getchar( tokenizer->ss_ );
        }
        /* if ( isIdentifierStart( next ) ) { */
            /* token = parseIdentifier( tokenizer->ss_ ); */
        /* } else { */
            /* ss_getchar( tokenizer->ss_ ); */
        /* } */
        /* next = ss_getchar( tokenizer->ss_ ); */
    }
    if ( token ) {
        return token;
    } else {
        return NULL;
    }
}
