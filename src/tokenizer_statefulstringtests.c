#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include <check.h>
#include "../src/logging.h"
#include "../src/statefulstring.h"
#include "../src/token.h"
#include "../src/tokenizer_charactertests.h"
#include "../src/tokenizer_statefulstringtests.h"

int isUnicodeSequenceStart( StatefulString *ss, unsigned int offset ) {
    int length = 0;
    if ( ss_peekx( ss, offset ) == L'\\' ) {
        length++;
        while (
            isHex( ss_peekx( ss, offset + length ) ) &&
            length <= 6
        ) {
            length++;
        }
        if ( isWhitespace( ss_peekx( ss, offset + length ) ) ) {
            length++;
        }
    }
    return ( length != 0 && length != 1 ) ? length : 0;
}

int isIdentifierStart( StatefulString *ss, unsigned int offset ) {
    wchar_t next        = ss_peekx( ss, offset );
    wchar_t nextnext    = ss_peekx( ss, offset + 1 );
    
    return (
        // `next` is a valid identifier-start character
        isNameStart( next )       ||
        // `next` is the start of a unicode sequence
        isUnicodeSequenceStart( ss, offset ) ||
        // OR, `next` is a dash, and the next character is
        // a valid name-start character
        ( next == L'-' && isNameStart( nextnext ) )
    );
}
int isWhitespaceStart( StatefulString *ss, unsigned int offset ) {
    return isWhitespace( ss_peekx( ss, offset ) );
}

int isStringStart( StatefulString *ss, unsigned int offset ) {
    return isQuote( ss_peekx( ss, offset ) );
}

int isAtkeywordStart( StatefulString *ss, unsigned int offset ) {
    return (
        ss_peekx( ss, offset ) == L'@'   &&
        isIdentifierStart( ss, offset + 1 )
    );
}

int isHashkeywordStart( StatefulString *ss, unsigned int offset ) {
    return (
        ss_peekx( ss, offset ) == L'#'   &&
        isNameChar( ss_peekx( ss, offset + 1 ) )
    );
}
int isNumberStart( StatefulString *ss, unsigned int offset ) {
    return (
        isNumeric( ss_peekx( ss, offset ) ) ||
        (
            ss_peekx( ss, offset ) == L'-'  &&
            isNumeric( ss_peekx( ss, offset + 1 ) )
        )
    );
}

int isUrlStart( StatefulString *ss, unsigned int offset ) {
    return (
        ss_peekx( ss, offset     ) == L'u'  &&
        ss_peekx( ss, offset + 1 ) == L'r'  &&
        ss_peekx( ss, offset + 2 ) == L'l'  &&
        ss_peekx( ss, offset + 3 ) == L'('  &&
        (
            isUrlChar( ss_peekx( ss, offset + 4 ) ) ||
            isStringStart( ss, offset + 4 )         ||
            isWhitespaceStart( ss, offset + 4 )
        )
    );
}

int isCDOStart( StatefulString *ss, unsigned int offset ) {
    return (
        ss_peekx( ss, offset     ) == L'<'  &&
        ss_peekx( ss, offset + 1 ) == L'!'  &&
        ss_peekx( ss, offset + 2 ) == L'-'  &&
        ss_peekx( ss, offset + 3 ) == L'-'
    );
}
int isCDCStart( StatefulString *ss, unsigned int offset ) {
    return (
        ss_peekx( ss, offset     ) == L'-'  &&
        ss_peekx( ss, offset + 1 ) == L'-'  &&
        ss_peekx( ss, offset + 2 ) == L'>'
    );
}
int isSGMLCommentStart( StatefulString *ss, unsigned int offset ) {
    return ( isCDOStart( ss, offset ) || isCDCStart( ss, offset ) );
}

int isCommentStart( StatefulString *ss, unsigned int offset ) {
    return(
        ss_peekx( ss, offset )      == L'/' &&
        ss_peekx( ss, offset + 1 )  == L'*'
    );
}
