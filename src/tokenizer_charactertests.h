//
//  Character-Based Tests
//
//  From: http://www.w3.org/TR/css3-syntax/#tokenization
//

// Name Start:          [a-zA-Z] | '_' | nonascii | escape
int isNameStart( wchar_t c );
// Name Character:      [a-zA-Z0-9] | '-' | '_' | nonascii | escape
int isNameChar( wchar_t c );
// String Character:    urlchar | #x20 | '\' nl
int isStringChar( wchar_t c );
// URL Character:       [#x9#x21#x23-#x26#x27-#x7E] | nonascii | escape
int UrlChar( wchar_t c );
// Newline:             #xA | #xD #xA | #xD | #xC
int isNewline( wchar_t c );
// Whitespace:          #x9 | #xA | #xC | #xD | #x20
int isWhitespace( wchar_t c );
// Alphanumeric:        [a-zA-Z0-9]
int isAlphanumeric( wchar_t c );
// Alphabetic:          [a-zA-Z]
int isAlpha( wchar_t c );
// Numeric:             [0-9]
int isNumeric( wchar_t c );
// Quote:               " | '
int isQuote( wchar_t c );
// Operator:            Other Stuff.
int isOperator( wchar_t c );
