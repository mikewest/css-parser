#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "logging.h"

//
//  Logging
//
void warn( char *msg ) {
    fprintf( stderr, "[WARNING]: %s\n", msg );
}
void error( char *msg ) {
    fprintf( stderr, "[ERROR]: %s\n", msg );
}
void fatal( char *msg ) {
    fprintf( stderr, "[FATAL]: %s\n", msg );
    exit( 1 );
}
