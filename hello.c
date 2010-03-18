#include <stdlib.h>
#include <stdio.h>

int main ( int argc, char *argv[] ) {
    if ( argc != 2 ) {
        fprintf( stderr, "Usage: %s [filename]\n", argv[ 0 ] );
        exit( 1 );
    }

    FILE *f;
    f = fopen( argv[ 1 ], "r" );
    if ( f == NULL ) {
        fprintf( stderr, "Can't open input file `%s`!\n", argv[ 1 ] );
        exit( 1 );
    }

    char c;
    while ( !feof(  ) ) {
        fscanf( f, "%c", &c );
        if ( c != '\n' ) {
            printf( "%c\n", c );
        }
    }

    fclose( f );
}
