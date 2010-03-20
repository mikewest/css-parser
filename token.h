typedef enum {
    IDENTIFIER,         //  {identifier}
    ATKEYWORD,          //  @{identifier}
    STRING,             //  {string}
    HASH,               //  #{name}
    NUMBER,             //  {num}
    PERCENTAGE,         //  {num}%
    DIMENSION,          //  {num}{identifier}
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
    WHITESPACE,         //  [ \t\r\n\f]+
    COMMENT,            //  \/\*[^*]*\*+([^/*][^*]*\*+)*\/
    FUNCTION,           //  {identifier}\(
    INCLUDES,           //  ~=
    DASHMATCH,          //  |=
    DELIM               //  Everything else.
} token_type;

typedef struct {
    unsigned int line, column;
} FileLocation;

typedef struct {
    FileLocation    *start;
    FileLocation    *end;
    token_type      type;
    char            *value;
} Token;

Token* generate_token( char*, token_type, FileLocation*, FileLocation* );
void print_token( Token* );
