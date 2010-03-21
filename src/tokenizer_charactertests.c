#include <wchar.h>
#include "tokenizer_charactertests.h"
//
//  wchar_tacter-Based Tests
//
int isAlpha( wchar_t c ) {
    return (
        ( c >= 'a' && c <= 'z' ) ||
        ( c >= 'A' && c <= 'Z' )
    );
}
int isNumeric( wchar_t c ) {
    return ( c >= '0' && c <= '9' );
}
int isAlphanumeric( wchar_t c ) {
    return isAlpha( c ) || isNumeric( c );
}
int isIdentifier( wchar_t c ) {
    return (
        isAlphanumeric( c ) || c == '-' || c == '_'
    );
}
int isIdentifierStart( wchar_t c ) {
    return (
        isIdentifier( c ) && !isNumeric( c )
    );
}
int isWhitespace( wchar_t c ) {
    return (
        c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\f'
    );
}
int isQuote( wchar_t c ) {
    return (
        c == '"' || c == '\''
    );
}
int isOperator( wchar_t c ) {
    return 0;
}

/* int isNameStart( wchar_t c ) { */
    /* return ( */
        /* isAlpha( c ) || c == '_' ||  */
    /* ); */
/* } */

/* IDENTIFIER::=   '-'? nmstart nmchar* */
/* name::= nmchar+ */
/* nmstart::=  [a-zA-Z] | '_' | nonascii | escape */
/* nonascii::= [#x80-#xD7FF#xE000-#xFFFD#x10000-#x10FFFF] */
/* unicode::=  '\' [0-9a-fA-F]{1,6} wc? */
/* escape::=   unicode | '\' [#x20-#x7E#x80-#xD7FF#xE000-#xFFFD#x10000-#x10FFFF] */
/* nmchar::=   [a-zA-Z0-9] | '-' | '_' | nonascii | escape */
/* NUMBER::=   [0-9]+ | [0-9]* '.' [0-9]+ */
/* string::=   '"' (stringchar | "'")* '"' | "'" (stringchar | '"')* "'" */
/* stringchar::=   urlchar | #x20 | '\' nl */
/* urlchar::=  [#x9#x21#x23-#x26#x27-#x7E] | nonascii | escape */
/* nl::=   #xA | #xD #xA | #xD | #xC */
/* wc::=   wc* */
/* wc::=   #x9 | #xA | #xC | #xD | #x20 */
