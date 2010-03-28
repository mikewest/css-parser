#define NOT_ENOUGH_MEMORY -1

typedef struct {
    unsigned int    line;
    unsigned int    column;
} StatefulStringPosition;

typedef struct {
    wchar_t                 *value;
    unsigned int            length;
    unsigned int            lines;
    unsigned int            *linebreaks;
    unsigned int            next_index;
    StatefulStringPosition  next_position;
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
