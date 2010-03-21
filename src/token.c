#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>
#include "logging.h"
#include "token.h"
#include "tokenizer.h"


Token* generate_token( wchar_t* value, token_type type, FileLocation* start, FileLocation* end ) {
    Token *temp = malloc( sizeof( Token ) );
    temp->start = start;
    temp->end   = end;
    temp->type  = type;
    temp->value = value;
    return temp;
}
void print_token( Token* t ) {
    wprintf( L"TOKEN: {\n" );
    wprintf( L"    Start:  Line %d, Column %d\n", (t->start)->line, (t->start)->column );
    wprintf( L"    End:    Line %d, Column %d\n", (t->end)->line, (t->end)->column );
    wprintf( L"    Value:  `%S`\n",               t->value );
    wprintf( L"    Type:   %d\n",                 t->type );
    wprintf( L"}\n" );
}
