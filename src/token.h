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
} token_type;

typedef struct {
    unsigned int line, column;
} FileLocation;

typedef struct {
    FileLocation    *start;
    FileLocation    *end;
    token_type      type;
    wchar_t         *value;
} Token;

Token* new_token( wchar_t*, token_type, FileLocation*, FileLocation* );
void free_token( Token* );
void print_token( Token* );
