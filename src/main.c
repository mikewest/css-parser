#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "token.h"
#include "tokenizer.h"
#include "parser.h"

#define MAX_TOKENS 100

int main() {
    Token *token;
    Token* *token_list;
    int num_tokens = 0;
    token_list = malloc( ( MAX_TOKENS + 1 ) * sizeof( Token* ) ); 

    while ( ( token = get_next_token( stdin, stderr ) ) != NULL ) {
        token_list[ num_tokens++ ] = token;
    }
    token_list[ num_tokens ] = L'\0';
    generate_parse_tree( token_list );
}
