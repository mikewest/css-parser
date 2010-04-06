typedef enum {
    IDENTIFIER,         //  {identifier}                                                        *
    NAME,               //  Less strict than identifier, really shouldn't exist on it's own.    *
    ATKEYWORD,          //  @{identifier}                                                       *
    HASHKEYWORD,        //  #{name}                                                             *
    STRING,             //  {string}                                                            *
    NUMBER,             //  {num}                                                               *
    DIMENSION,          //  {num}{identifier}                                                   *
    PERCENTAGE,         //  {num}%                                                              *
    URL,                //  url\({whitespace}{string}{whitespace}\)                             ~   @TODO: URLs can contain `)`
                        //  url\({whitespace}([!#$%&*-~]|{nonascii}|{escape})*{whitespace}\)
    UNICODE,            //  u\+[0-9a-f?]{1,6}(-[0-9a-f]{1,6})?
    SGML_COMMENT_OPEN,  //  <!--                                                                *
    SGML_COMMENT_CLOSE, //  -->                                                                 *
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
    WHITESPACE,         //  [ \t\r\n\f]+                                                        *
    COMMENT,            //  \/\*[^*]*\*+([^/*][^*]*\*+)*\/                                      *
    FUNCTION,           //  {identifier}\(
    INCLUDES,           //  ~=
    DASHMATCH,          //  |=
    PREFIXMATCH,        //  ^=
    SUFFIXMATCH,        //  $=
    SUBSTRINGMATCH,     //  *=
    DELIM               //  Everything else.
} TokenType;

struct TokenError;
struct Token;

typedef struct TokenError {
    wchar_t *message;
    struct  Token   *token;
} TokenError;

typedef struct Token {
    StatefulStringPosition  *start;
    StatefulStringPosition  *end;
    TokenType               type;
    wchar_t                 *value;
    struct  TokenError      *error;
    unsigned int            length;
} Token;

Token* token_new( const wchar_t *value, const unsigned int length, const TokenType type, const StatefulStringPosition start, const StatefulStringPosition end );
void token_free( Token *t );
void token_print( const Token *t );

// Are Type and Value equal?  (Think `==`)
int token_eq( const Token *t1, const Token *t2 );
// Are Type and Value AND position equal?  (Think `===`)
int token_eeq( const Token *t1, const Token *t2 );
