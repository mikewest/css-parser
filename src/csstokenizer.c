#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include <check.h>
#include "../src/logging.h"
#include "../src/statefulstring.h"
#include "../src/token.h"
#include "../src/csstokenizer.h"

#define BASE_MAXTOKENS  1024

Tokenizer *tokenizer_new( StatefulString *ss ) {
    Tokenizer *temp = malloc( sizeof( Tokenizer ) );

    temp->numtokens     = -1;
    temp->ss_           = ss;
    temp->maxtokens_    = BASE_MAXTOKENS;
    temp->tokens_       = malloc( ( BASE_MAXTOKENS + 1 ) * sizeof( Token* ) );
    return temp;
}

void tokenizer_free( Tokenizer *t ) {
    free( t->ss_ );
    free( t->tokens_ );
    free( t );
}
