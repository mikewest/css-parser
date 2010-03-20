#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "logging.h"
#include "token.h"
#include "tokenizer.h"


Token* generate_token( char* value, token_type type, FileLocation* start, FileLocation* end ) {
    Token *temp = malloc( sizeof( Token ) );
    temp->start = start;
    temp->end   = end;
    temp->type  = type;
    temp->value = value;
    return temp;
}
void print_token( Token* t ) {
    puts( "{\n" );
    printf( "    Start:  Line %d, Column %d\n", (t->start)->line, (t->start)->column );
    printf( "    End:    Line %d, Column %d\n", (t->end)->line, (t->end)->column );
    printf( "    Value:  `%s`\n",               t->value );
    printf( "    Type:   %d\n",                 t->type );
    puts( "}\n" );
}
