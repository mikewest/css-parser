#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "token.h"
#include "tokenizer.h"

int main() {
    Token *token;
    while ( ( token = get_next_token( stdin, stderr ) ) != NULL ) {
        print_token( token );
    }
}
