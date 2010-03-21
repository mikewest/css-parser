#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>
#include "logging.h"

//
//  Logging
//
void warn( wchar_t *msg ) {
    fwprintf( stderr, L"[WARNING]: %s\n", msg );
}
void error( wchar_t *msg ) {
    fwprintf( stderr, L"[ERROR]: %s\n", msg );
}
void fatal( wchar_t *msg ) {
    fwprintf( stderr, L"[FATAL]: %s\n", msg );
    exit( 1 );
}
