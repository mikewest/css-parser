#define MAX_STRING_LENGTH 1024

wchar_t* allocateLargeString( wchar_t *from );
wchar_t* downsizeLargeString( wchar_t *string, int length, wchar_t *from );
FileLocation* getCurrentLocation();
wchar_t fpeek( FILE *in );
wchar_t fpeekx( FILE *in, int x );
wchar_t fget( FILE *in );
