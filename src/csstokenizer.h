typedef struct {
    unsigned int    numtokens;
    
    //  Internal: No touchies, plz!
    StatefulString  *ss_;
    Token           **tokens_;
    unsigned int    maxtokens_;
} Tokenizer;

Tokenizer *tokenizer_new( StatefulString *ss );


void tokenizer_free( Tokenizer *t );
