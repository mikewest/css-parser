#include <wchar.h>
#include "tokenizer_charactertests.h"
//
//  wchar_tacter-Based Tests
//
int isAlpha( wchar_t c ) {
    return iswalpha( c );
}
int isNumeric( wchar_t c ) {
    return iswdigit( c );
}
int isAlphanumeric( wchar_t c ) {
    return iswalnum( c );
}
int isIdentifier( wchar_t c ) {
    return (
        isAlphanumeric( c ) || c == L'-' || c == L'_'
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
