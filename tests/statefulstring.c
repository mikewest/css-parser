#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <wchar.h>
#include "../src/logging.h"
#include "../src/statefulstring.h"

FILE *generateTempfile( const char *string ) {
    char template[] = "/tmp/statefulstring.XXXXX";
    FILE *file      = tmpfile();
    
    if ( file ) {
        fprintf( file, "%s", string );
        rewind( file );
        return file;
    } else {
        return NULL;
    }
}

int testStatefulStringCreation( void ) {
    FILE *file;
    StatefulString *ss;
    int failures = 0;

    file    = generateTempfile( "Omg!" );
    ss      = ss_fromstream( file );

    if ( !wcscmp( ss->value, L"Omg!" ) ) {
        wprintf( L"-   FAILURE:  `ss->value` is `%S`, should be `%S`.\n", ss->value, L"Omg!" );
        failures++;
    }
    if ( ss->current_index != 0 ) {
        wprintf( L"-   FAILURE:  `ss->current_index` is `%d`, should be `%d`.\n", ss->current_index, 0 );
        failures++;
    }
    return failures;
}

void main() {
    wprintf( L"Running `testStatefulStringCreation`:\n" );
    wprintf( testStatefulStringCreation() ? L"-------\nFAILURE!\n--------\n\n" : L"--------\nSUCCESS!\n--------\n\n" );
}
