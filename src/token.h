typedef enum {
    IDENTIFIER,         //  {identifier}
    ATKEYWORD,          //  @{identifier}
    HASHKEYWORD,        //  #{identifier}
    STRING,             //  {string}
    NUMBER,             //  {num}
    DIMENSION,          //  {num}{identifier}
    PERCENTAGE,         //  {num}%
    URI,                //  url\({whitespace}{string}{whitespace}\)
                        //  url\({whitespace}([!#$%&*-~]|{nonascii}|{escape})*{whitespace}\)
    UNICODE,            //  u\+[0-9a-f?]{1,6}(-[0-9a-f]{1,6})?
    SGML_COMMENT_OPEN,  //  <!--
    SGML_COMMENT_CLOSE, //  -->
    COLON,              //  :
    SEMICOLON,          //  ;
    CURLY_BRACE_OPEN,   //  {
    CURLY_BRACE_CLOSE,  //  }
    PAREN_OPEN,         //  (
    PAREN_CLOSE,        //  )
    SQUARE_BRACE_OPEN,  //  [
    SQUARE_BRACE_CLOSE, //  ]
    AT,                 //  @
    PERCENT,            //  %
    HASH,               //  #
    DOT,                //  .
    WHITESPACE,         //  [ \t\r\n\f]+
    COMMENT,            //  \/\*[^*]*\*+([^/*][^*]*\*+)*\/
    FUNCTION,           //  {identifier}\(
    INCLUDES,           //  ~=
    DASHMATCH,          //  |=
    PREFIXMATCH,        //  ^=
    SUFFIXMATCH,        //  $=
    SUBSTRINGMATCH,     //  *=
    DELIM               //  Everything else.
} TokenType;

typedef struct {
    StatefulStringPosition  *start;
    StatefulStringPosition  *end;
    TokenType               type;
    wchar_t                 *value;

    // Internal: No touchies, plz!
    unsigned int            length_;
} Token;

Token* token_new( const wchar_t *value, const unsigned int length, const TokenType type, const StatefulStringPosition start, const StatefulStringPosition end );
void token_free( Token *t );
void token_print( const Token *t );

// Are Type and Value equal?  (Think `==`)
int token_eq( const Token *t1, const Token *t2 );
// Are Type and Value AND position equal?  (Think `===`)
int token_eeq( const Token *t1, const Token *t2 );
