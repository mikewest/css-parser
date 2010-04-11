#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include <check.h>
#include "../src/statefulstring.h"
#include "../src/token.h"
#include "../src/tokenizer.h"
#include "../src/tokenizer_charactertests.h"
#include "../src/tokenizer_statefulstringtests.h"

#define BASE_MAXTOKENS  1024
#define BASE_MAXERRORS  1024

//  @constructor
Tokenizer *tokenizer_new( StatefulString *ss ) {
    Tokenizer *temp = malloc( sizeof( Tokenizer ) );

    temp->numtokens     = 0;
    temp->numerrors     = 0;
    temp->maxerrors_    = BASE_MAXERRORS;
    temp->errors        = malloc( ( BASE_MAXERRORS + 1 ) * sizeof( TokenError* ) );
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

TokenError *tokenizer_error( Tokenizer *tokenizer, wchar_t *msg, Token *token ) {
    TokenError *error = malloc( sizeof( TokenError ) );
    error->message  = msg;
    error->token    = token;

    if ( tokenizer->numerrors >= tokenizer->maxerrors_ ) {
        tokenizer->maxerrors_ *= 2;
        tokenizer->errors = realloc( tokenizer->errors, ( tokenizer->maxerrors_ + 1 ) * sizeof( TokenError* ) );
        if ( tokenizer->errors == NULL ) {
            allocationerror( tokenizer->maxerrors_, L"Tokenizer::tokenizer_error" );
            exit( EXIT_FAILURE );
        }
    }
    tokenizer->errors[ tokenizer->numerrors++ ] = error;
    token->error = error;
    return error; 
}

///////////////////////////////////////////////////////////////////////////
//
//  Parsing Functions
//
/////////////////////////////////////
//
//  Whitespace
//
Token *parseWhitespace( Tokenizer *tokenizer ) {
    StatefulString *ss = tokenizer->ss_;
    assert( isWhitespaceStart( ss, 0 ) );

    int start, length;
    StatefulStringPosition pos1, pos2;

    start   = ss->next_index;
    length  = 0;
    pos1    = ss->next_position;
    while ( isWhitespace( ss_peek( ss ) ) ) {
        ss_getchar( ss );
        length++;
    }
    pos2    = ss->next_position;
    return token_new( ss_substr( ss, start, length ), length, WHITESPACE, pos1, pos2 ); 
}
/////////////////////////////////////
//
//  Strings
//
Token *parseString( Tokenizer *tokenizer ) {
    StatefulString *ss = tokenizer->ss_;
    assert( isStringStart( ss, 0 ) );

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
    } else {
        free( error );
    }
    return token;
}

/////////////////////////////////////
//
//  Identifiers
//
int processChar( StatefulString *ss ) {
    int length = isUnicodeSequenceStart( ss, 0 );
    if ( length ) {
        for ( int i = 0; i < length; i++ ) {
            ss_getchar( ss );
        }
        return length;
    } else {
        ss_getchar( ss );
        return 1;
    }
}
Token *parseName( Tokenizer *tokenizer ) {
    StatefulString *ss = tokenizer->ss_;
    assert( isNameChar( ss_peek( ss ) ) || isUnicodeSequenceStart( ss, 0 ) );

    int start, length;
    StatefulStringPosition pos1, pos2;

    start   = ss->next_index;
    length  = 0;
    pos1    = ss->next_position;
    while ( isNameChar( ss_peek( ss ) ) || isUnicodeSequenceStart( ss, 0 ) ) {
        length += processChar( ss );
    }
    if ( ss_peek( ss ) == L'(' ) {
        ss_getchar( ss );
        length++;
    }
    pos2    = ss->next_position;
    return token_new( ss_substr( ss, start, length ), length, NAME, pos1, pos2 ); 
}
Token *parseIdentifier( Tokenizer *tokenizer ) {
    StatefulString *ss = tokenizer->ss_;
    assert( isIdentifierStart( ss, 0 ) );

    // Delegate to the less-strict `parseName`
    Token *t = parseName( tokenizer );
    t->type = ( t->value[ t->length - 1 ] == L'(' ) ? FUNCTION : IDENTIFIER;
    return t;
}
/////////////////////////////////////
//
//  @keyword
//
Token *parseAtkeyword( Tokenizer *tokenizer ) {
    assert( isAtkeywordStart( tokenizer->ss_, 0 ) );

    StatefulStringPosition *start = malloc( sizeof( StatefulStringPosition ) );
    memcpy( start, &( ( tokenizer->ss_ )->next_position ), sizeof( StatefulStringPosition ) );

    ss_getchar( tokenizer->ss_ );   // Throw away the `@`
    Token *t    = parseIdentifier( tokenizer );
    t->type     = ATKEYWORD;
    free( t->start );
    t->start    = start;
    return t;
}
/////////////////////////////////////
//
//  #keyword
//
Token *parseHashkeyword( Tokenizer *tokenizer ) {
    assert( isHashkeywordStart( tokenizer->ss_, 0 ) );

    StatefulStringPosition *start = malloc( sizeof( StatefulStringPosition ) );
    memcpy( start, &( ( tokenizer->ss_ )->next_position ), sizeof( StatefulStringPosition ) );

    ss_getchar( tokenizer->ss_ );   // Throw away the `#`
    Token *t    = parseName( tokenizer );
    t->type     = HASHKEYWORD;
    free( t->start );
    t->start    = start;
    return t;
}
/////////////////////////////////////
//
//  Number
//

Token *parseNumber( Tokenizer *tokenizer ) {
    StatefulString *ss = tokenizer->ss_;
    assert( isNumberStart( ss, 0 ) );

    int start, length;
    StatefulStringPosition pos1, pos2;

    start           = ss->next_index;
    length          = 0;
    pos1            = ss->next_position;
    TokenType type  = NUMBER;
    int     isFloat = 0;
    wchar_t temp;
    while (
        isNumeric( ss_peek( ss ) )                  ||
        ( ss_peek( ss ) == L'-' && length == 0 )    ||
        ( ss_peek( ss ) == L'.' && !isFloat )
    ) {
        temp = ss_getchar( ss );
        if ( temp == L'.' ) {
            isFloat = 1;
        }
        length++;
    }
    // Is the number followed by a percentage?
    if ( ss_peek( ss ) == L'%' ) {
        ss_getchar( ss );
        length++;
        type = PERCENTAGE;
    }
    // Is the number followed by a dimension?
    else if ( isIdentifierStart( ss, 0 ) ) {
        while ( isNameChar( ss_peek( ss ) ) ) {
            ss_getchar( ss );
            length++;
        }
        type = DIMENSION;
    }

    pos2    = ss->next_position;
    return token_new( ss_substr( ss, start, length ), length, type, pos1, pos2 ); 
}
/////////////////////////////////////
//
//  Url 
//
Token *parseUrl( Tokenizer *tokenizer ) {
    StatefulString *ss = tokenizer->ss_;
    assert( isUrlStart( ss, 0 ) );

    int start, length;
    StatefulStringPosition pos1, pos2;
    Token   *token;
    wchar_t *error = malloc( 201 * sizeof( wchar_t ) );
    error[ 0 ] = L'\0';

    start               = ss->next_index;
    pos1                = ss->next_position;
    TokenType   type    = URL;

    // Throw away `url(`
    for ( int i = 0; i<4; i++ ) {
        ss_getchar( ss );
    }
    length          = 4;

    //  Throw away leading whitespace
    while ( isWhitespaceStart( ss, 0 ) ) {
        ss_getchar( ss );
        length++;
    }

    //  Process the url
    if ( isStringStart( ss, 0 ) ) {
        token   = parseString( tokenizer );
        length += token->length + 2; // +2 for the quotes
        if ( token->error != NULL ) {
            length     -= 1;    // Error'd strings don't have a trailing quote
            pos2        = ss->next_position;
            Token *t    = token_new( ss_substr( ss, start, length ), length, type, pos1, pos2 );
            t->error    = token->error;
            ( token->error )->token = t;
            free( token );
            return t;
        } else {
            free( token );
        }
    } else if ( isUrlChar( ss_peek( ss ) ) || isUnicodeSequenceStart( ss, 0 ) ) {
        while ( isUrlChar( ss_peek( ss ) ) || isUnicodeSequenceStart( ss, 0 ) ) {
            length += processChar( ss );
        }
    } else {
        // ERROR:
        if ( ss_peek( ss ) == WEOF ) {
            swprintf( error, 200, L"Encountered end-of-file while parsing a URL." );
        } else {
            swprintf( error, 200, L"Encountered an invalid character (`%C`) while parsing a URL.", ss_peek( ss ) );
        }
    }

    //  Throw away trailing whitespace
    while ( isWhitespaceStart( ss, 0 ) ) {
        ss_getchar( ss );
        length++;
    }

    //  Grab the trailing `)`
    if ( ss_peek( ss ) == L')' ) {
        ss_getchar( ss );
        length++;
    } else {
        // ERROR:
        if ( ss_peek( ss ) == WEOF ) {
            swprintf( error, 200, L"Encountered end-of-file while parsing a URL.  Expected a `)`." );
        } else {
            swprintf( error, 200, L"Encountered an invalid character (`%C`) while parsing a URL.  Expected a `)`.  Perhaps a typo?", ss_peek( ss ) );
        }
    }

    // Return the token.
    pos2    = ss->next_position;
    token   = token_new( ss_substr( ss, start, length ), length, type, pos1, pos2 ); 

    if ( wcscmp( error, L"" ) != 0 ) {
        tokenizer_error( tokenizer, error, token );
    } else {
        free( error );
    }
    return token;
}
/////////////////////////////////////
//
//  CDO/CDC
//

Token *parseSGMLComment( Tokenizer *tokenizer ) {
    StatefulString *ss = tokenizer->ss_;
    assert( isCDOStart( ss, 0 ) || isCDCStart( ss, 0 ) );

    int start, length;
    StatefulStringPosition pos1, pos2;

    start               = ss->next_index;
    pos1                = ss->next_position;
    length              = 4;
    TokenType   type    = SGML_COMMENT_OPEN;
    if ( isCDCStart( ss, 0 ) ) {
        type    = SGML_COMMENT_CLOSE;
        length  = 3;
    }
    for ( int i = 0; i<length; i++ ) {
        ss_getchar( ss );
    }
    pos2    = ss->next_position;
    return token_new( ss_substr( ss, start, length ), length, type, pos1, pos2 ); 
}
/////////////////////////////////////
//
//  Comments
//

Token *parseComment( Tokenizer *tokenizer ) {
    StatefulString *ss = tokenizer->ss_;
    assert( isCommentStart( ss, 0 ) );

    int start, length;
    StatefulStringPosition pos1, pos2;
    Token *token;
    wchar_t *error = malloc( 201 * sizeof( wchar_t ) );
    error[ 0 ] = L'\0';

    start               = ss->next_index;
    pos1                = ss->next_position;
    length              = 2;
    TokenType   type    = COMMENT;

    ss_getchar( ss );   ss_getchar( ss );   // Throw away `/*`
    while (
        ss_peek( ss ) != WEOF               &&
        (
            ss_peek( ss ) != L'*'       ||
            ss_peekx( ss, 1 ) != L'/'
        )
    ) {
        length++;
        if ( ss_getchar( ss ) == L'\\' ) {
            ss_getchar( ss );
            length++;
        }
    }

    if ( ss_peek( ss ) == WEOF ) {
        swprintf( error, 200, L"Encountered end-of-file while parsing a comment.  Probably a forgotten `*/`." );
    } else {
        ss_getchar( ss ); ss_getchar( ss ); // Throw away `*/`
        length += 2;
    }

    // Return the token.
    pos2    = ss->next_position;
    token   = token_new( ss_substr( ss, start, length ), length, type, pos1, pos2 ); 

    if ( wcscmp( error, L"" ) != 0 ) {
        tokenizer_error( tokenizer, error, token );
    } else {
        free( error );
    }
    return token;
}
/////////////////////////////////////
//
//  Operators, et al.
//
Token *parseEverythingElse( Tokenizer *tokenizer ) {
    StatefulString *ss = tokenizer->ss_;

    int start, length;
    StatefulStringPosition pos1, pos2;

    start               = ss->next_index;
    pos1                = ss->next_position;
    TokenType   type    = DELIM;
    length              = 1;

    wchar_t temp        = ss_getchar( ss );

    if (
        ( temp== L'~' || temp == L'|' || temp == L'*' || temp == L'^' || temp == L'$' ) &&
        ss_peek( ss ) == L'='
    ) {
        ss_getchar( ss );
        length++;
    }

    if ( length == 1 ) {
        switch ( temp ) {
            case L'{':
                type    =   CURLY_BRACE_OPEN;
                break;
            case L'}':
                type    =   CURLY_BRACE_CLOSE;
                break;
            case L'[':
                type    =   SQUARE_BRACE_OPEN;
                break;
            case L']':
                type    =   SQUARE_BRACE_CLOSE;
                break;
            case L'(':
                type    =   PAREN_OPEN;
                break;
            case L')':
                type    =   PAREN_CLOSE;
                break;
            case L':':
                type    =   COLON;
                break;
            case L';':
                type    =   SEMICOLON;
                break;
            case L'@':
                type    =   AT;
                break;
            case L'#':
                type    =   HASH;
                break;
            case L'%':
                type    =   PERCENT;
                break;
            case L'.':
                type    =   DOT;
                break;
        }
    } else if ( length == 2 ) {
        switch ( temp ) {
            case L'~':
                type    =   INCLUDES;
                break;
            case L'|':
                type    =   DASHMATCH;
                break;
            case L'^':
                type    =   PREFIXMATCH;
                break;
            case L'$':
                type    =   SUFFIXMATCH;
                break;
            case L'*':
                type    =   SUBSTRINGMATCH;
                break;
        }
    }
    pos2    = ss->next_position;
    return token_new( ss_substr( ss, start, length ), length, type, pos1, pos2 ); 
}


///////////////////////////////////////////////////////////////////////////
//
//  Parsing Interface
//
Token *tokenizer_next( Tokenizer *tokenizer ) {
    wchar_t c, next;
    Token *token = NULL;

    next = ss_peek( tokenizer->ss_ );
    while ( next != WEOF && !token ) {
//  Whitespace
        if ( isWhitespaceStart( tokenizer->ss_, 0 ) ) {
            token = parseWhitespace( tokenizer );
        }
//  Strings
        else if ( isStringStart( tokenizer->ss_, 0 ) ) {
            token = parseString( tokenizer );
        }
//  Comments
        else if ( isCommentStart( tokenizer->ss_, 0 ) ) {
            token = parseComment( tokenizer );
        }
//  URL
        else if ( isUrlStart( tokenizer->ss_, 0 ) ) {
            token = parseUrl( tokenizer );
        }
//  SGML Comments
        else if ( isSGMLCommentStart( tokenizer->ss_, 0 ) ) {
            token = parseSGMLComment( tokenizer );
        }
//  Identifier
        else if ( isIdentifierStart( tokenizer->ss_, 0 ) ) {
            token = parseIdentifier( tokenizer );
        }
//  @keyword
        else if ( isAtkeywordStart( tokenizer->ss_, 0 ) ) {
            token = parseAtkeyword( tokenizer );
        }
//  #keyword
        else if ( isHashkeywordStart( tokenizer->ss_, 0 ) ) {
            token = parseHashkeyword( tokenizer );
        }
//  Number
        else if ( isNumberStart( tokenizer->ss_, 0 ) ) {
            token = parseNumber( tokenizer );
        }
//  Operators & Delims (everything else)
        else {
            token = parseEverythingElse( tokenizer );
        }
    }
    if ( token ) {
        return token;
    } else {
        return NULL;
    }
}
