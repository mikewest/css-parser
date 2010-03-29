#define ERROR_ALLOCATION    -1

typedef struct {
    unsigned int    line;
    unsigned int    column;
} StatefulStringPosition;

typedef struct {
    wchar_t                 *value;         //  The string that's read in
    unsigned int            length;         //  The number of `wchar_t`s in `value`
    unsigned int            lines;          //  The number of lines.
    unsigned int            *linebreaks;    //  An array containing each line's index
    unsigned int            next_index;     //  The next character to read
    StatefulStringPosition  next_position;  //  The position of the next character

    // Internal: No touchies, plz!
    unsigned int            maxlines_;      //  The length of `linebreaks[]`
} StatefulString;

/**
 *  Given a stream, return a pointer to a `StatefulString`
 */
StatefulString *ss_fromstream( FILE* stream );

/**
 *  Free allocated memory, and destroy the `StatefulString`
 */
void ss_free( StatefulString* ss );

/**
 *  Get the next character in the string, and update the
 *  `StatefulString`'s state:
 *
 *  -   increment the current index
 *  -   add line breaks
 */
wchar_t ss_getchar( StatefulString* ss );

/**
 *  Peek at the next character in the string without
 *  changing its state.
 */
wchar_t ss_peek( StatefulString* ss );
/**
 *  Peek at the the Xth-next character in the string without
 *  changing its state
 */
wchar_t ss_peekx( StatefulString* ss, const unsigned int x );
