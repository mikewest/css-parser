#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "token.h"
#include "tokenizer.h"
#include "parser.h"

ParseTree * generate_parse_tree( Token* *list ) {
    int current = 0;
    Token *t;
    while ( ( t = list[ current++ ] ) != L'\0' ) {
        print_token( t );
    }
}
