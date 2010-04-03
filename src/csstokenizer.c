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
#define BASE_MAXERRORS  1024

//  @constructor
Tokenizer *tokenizer_new( StatefulString *ss ) {
    Tokenizer *temp = malloc( sizeof( Tokenizer ) );

    temp->numtokens     = 0;
    temp->numerrors     = 0;
    temp->maxerrors_    = BASE_MAXERRORS;
    temp->errors        = malloc( ( BASE_MAXERRORS + 1 ) * sizeof( TokenizerError* ) );
    temp->ss_           = ss;
    temp->maxtokens_    = BASE_MAXTOKENS;
    temp->tokens_       = malloc( ( BASE_MAXTOKENS + 1 ) * sizeof( Token* ) );
    return temp;
}
//  @destructor
void tokenizer_free( Tokenizer *t ) {
    free( t->errors );
    free( t->tokens_ );
    free( t );
}

Token* addtoken( Tokenizer *tokenizer, const wchar_t *value,
                        const unsigned int length, const TokenType type,
                        const StatefulStringPosition start,
                        const StatefulStringPosition end ) {
    Token *token = token_new( value, length, type, start, end );
    if ( tokenizer->numtokens >= tokenizer->maxtokens_ ) {
        tokenizer->maxtokens_ *= 2;
        tokenizer->tokens_ = realloc( tokenizer->tokens_, ( tokenizer->maxtokens_ + 1 ) * sizeof( Token* ) );
        if ( tokenizer->tokens_ == NULL ) {
            allocationerror( tokenizer->maxtokens_, L"Tokenizer::addtoken" );
            exit( EXIT_FAILURE );
        }
    }
    tokenizer->tokens_[ tokenizer->numtokens++ ] = token;
    return token;
}

TokenizerError *tokenizer_error( Tokenizer *tokenizer, wchar_t *msg, Token *token ) {
    TokenizerError *error = malloc( sizeof( TokenizerError ) );
    error->message  = msg;
    error->token    = token;

    if ( tokenizer->numerrors >= tokenizer->maxerrors_ ) {
        tokenizer->maxerrors_ *= 2;
        tokenizer->errors = realloc( tokenizer->errors, ( tokenizer->maxerrors_ + 1 ) * sizeof( TokenizerError* ) );
        if ( tokenizer->errors == NULL ) {
            allocationerror( tokenizer->maxerrors_, L"Tokenizer::tokenizer_error" );
            exit( EXIT_FAILURE );
        }
    }
    tokenizer->errors[ tokenizer->numerrors++ ] = error;
    return error; 
}

///////////////////////////////////////////////////////////////////////////
//
//  Parsing Functions
//

/////////////////////////////////////
//
//  Strings
//
int isStringStart( StatefulString *ss ) {
    return isQuote( ss_peek( ss ) );
}
Token *parseString( Tokenizer *tokenizer ) {
    StatefulString *ss = tokenizer->ss_;
    assert( isStringStart( ss ) );

    int start, length;
    StatefulStringPosition pos1, pos2;
    wchar_t temp;

    Token *token;

    length  = 0;
    pos1    = ss->next_position;

    wchar_t quote = ss_getchar( ss );
    start   = ss->next_index;           // Exclude the quotes
    while (
        ( temp = ss_getchar( ss ) ) != WEOF &&
        !isNewline( temp )                  &&
        temp != quote
    ) {
        length++;
        if ( temp == L'\\' ) {
            ss_getchar( ss );
            length++;
        }
    }
    pos2    = ss->next_position;
    token   = token_new( ss_substr( ss, start, length ), length, STRING, pos1, pos2 );

    wchar_t *error = malloc( 201 * sizeof( wchar_t ) );
    error[ 0 ] = L'\0';
    if ( temp == WEOF ) {
        swprintf( error, 200, L"Encountered end-of-file while parsing a string.  Looks like a `%C` is missing.", quote );
    } else if ( isNewline( temp ) ) {
        error = L"Encountered a newline while parsing a string.  Strings can only contain newlines if they're properly escaped (e.g. `\\[\\n]`)";
    }
    if ( wcscmp( error, L"" ) != 0 ) {
        tokenizer_error( tokenizer, error, token );
    }
    return token;
}

/////////////////////////////////////
//
//  Identifiers
//
int isIdentifierStart( StatefulString *ss ) {
    wchar_t next        = ss_peek( ss );
    wchar_t nextnext    = ss_peekx( ss, 1 );
    
    return (
        // `next` is a valid identifier-start character
        isNameStart( next )       ||
        // OR, `next` is a dash, and the next character is
        // a valid name-start character
        ( next == L'-' && isNameStart( nextnext ) )
    );
}
Token *parseIdentifier( Tokenizer *tokenizer ) {
    StatefulString *ss = tokenizer->ss_;
    assert( isIdentifierStart( ss ) );

    int start, length;
    StatefulStringPosition pos1, pos2;
    wchar_t temp;

    start   = ss->next_index;
    length  = 0;
    pos1    = ss->next_position;
    while ( isNameChar( temp = ss_getchar( ss ) ) ) {
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
//  1.  Strings
        if ( isStringStart( tokenizer->ss_ ) ) {
            token = parseString( tokenizer );
        }
//  1.  Identifier
        else if ( isIdentifierStart( tokenizer->ss_ ) ) {
            token = parseIdentifier( tokenizer );
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
