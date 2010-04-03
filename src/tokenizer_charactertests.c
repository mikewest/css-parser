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

int isUrlChar( wchar_t c ) {
    return (
        c   ==  0x9     ||
        c   ==  0x21    ||
        ( c >=  0x23    &&  c   <= 0x26 ) ||
        ( c >=  0x27    &&  c   <= 0x7E ) ||
        isNonAscii( c )
    );
}
int isStringChar( wchar_t c ) {
    return (
        isUrlChar( c )  ||
        c   == 0x20
    );
}
int isNewline( wchar_t c ) {
    return (
        c == 0xA || c == 0xD || c == 0xC
    );
}
int isWhitespace( wchar_t c ) {
    return (
        isNewline( c )   ||
        c == 0x9    ||  c == 0x20
    );
}

int isNameChar( wchar_t c ) {
    return (
        iswalpha( c )   ||
        iswdigit( c )   ||
        c == L'-'       ||
        c == L'_'       ||
        isNonAscii( c )
    );
}
int isNameStart( wchar_t c ) {
    return (
        iswalpha( c )   ||
        c == L'_'       ||
        isNonAscii( c )
    );
};



int isAlpha( wchar_t c ) {
    return iswalpha( c );
}
int isNumeric( wchar_t c ) {
    return (
        iswdigit( c )
    );
}
int isAlphanumeric( wchar_t c ) {
    return iswalnum( c );
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
