#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>
#include "logging.h"
#include "statefulstring.h"
#include "csstoken.h"

Token* token_new( wchar_t *value, unsigned int length, TokenType type, const StatefulStringPosition start, const StatefulStringPosition end ) {
    Token *temp     = malloc( sizeof( Token ) );
    temp->type      = type;
    temp->value     = malloc( ( length + 1 ) * sizeof( wchar_t ) );
    temp->length_   = length;
    wcsncpy( temp->value, value, length );

    temp->start             = malloc( sizeof( StatefulStringPosition ) );
    (temp->start)->line     = start.line;
    (temp->start)->column   = start.column;

    temp->end           = malloc( sizeof( StatefulStringPosition ) );
    (temp->end)->line   = end.line;
    (temp->end)->column = end.column;
    return temp;
}
void token_free( Token *t ) {
    free( t->start );
    free( t->end );
    free( t->value );
    free( t );
}
void token_print( Token *t ) {
    wprintf( L"TOKEN: {\n" );
    wprintf( L"    Start:  Line %d, Column %d\n", (t->start)->line, (t->start)->column );
    wprintf( L"    End:    Line %d, Column %d\n", (t->end)->line, (t->end)->column );
    wprintf( L"    Value:  `%S`\n",               t->value );
    wprintf( L"    Type:   %d\n",                 t->type );
    wprintf( L"}\n" );
}
int token_eq( Token *t1, Token *t2 ) {
    return (
        //  Value
        wcscmp( t1->value, t2->value )              &&
        //  Type
        t1->type == t2->type                        &&
        //  Start
        (t1->start)->line == (t2->start)->line      &&
        (t1->start)->column == (t2->start)->column  &&
        //  End
        (t1->end)->line == (t2->end)->line          &&
        (t1->end)->column == (t2->end)->column
    );
}
