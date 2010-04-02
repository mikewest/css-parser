#include <wchar.h>
#include "tokenizer_charactertests.h"
//
//  wchar_tacter-Based Tests
//

int isNonAscii( wchar_t c ) {
    return (
        ( c >= 0x80     &&  c <= 0xD7FF ) ||
        ( c >= 0xE00    &&  c <= 0xFFFD ) ||
        ( c >= 0x10000  &&  c <= 0x10FFFF )
    );
}

int isAlpha( wchar_t c ) {
    return iswalpha( c );
}
int isNumeric( wchar_t c ) {
    return (
        iswdigit( c ) || c == L'.'
    );
}
int isAlphanumeric( wchar_t c ) {
    return iswalnum( c );
}
int isIdentifier( wchar_t c ) {
    return (
        isAlphanumeric( c ) || isNonAscii( c ) || c == L'-' || c == L'_'
    );
}
int isIdentifierStart( wchar_t c ) {
    return (
        isIdentifier( c ) && !isNumeric( c )
    );
}
int isWhitespace( wchar_t c ) {
    return iswspace( c );
}
int isQuote( wchar_t c ) {
    return (
        c == L'"' || c == L'\''
    );
}
int isOperator( wchar_t c ) {
    switch ( c ) {
        case L'~':
        case L'|':
        case L'^':
        case L':':
        case L';':
        case L'{':
        case L'}':
        case L'[':
        case L']':
        case L'@':
        case L'#':
        case L'.':
        case L'(':
        case L')':
            return 1;

        default:
            return 0;
    }
}
