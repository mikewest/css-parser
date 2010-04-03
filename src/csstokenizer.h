typedef struct {
    wchar_t *message;
    Token   *token;
} TokenizerError;

typedef struct {
    unsigned int    numtokens;
    unsigned int    numerrors;
    TokenizerError  **errors;

    //  Internal: No touchies, plz!
    StatefulString  *ss_;
    Token           **tokens_;
    unsigned int    maxtokens_;
    unsigned int    maxerrors_;
} Tokenizer;

Tokenizer *tokenizer_new( StatefulString *ss );
Token *tokenizer_next( Tokenizer *t );

void tokenizer_free( Tokenizer *t );
