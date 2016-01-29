#include "debug.h"
#include <stdio.h>
#include <stdlib.h>

// Print the given message, then the system error message
// and at last abort the process.
void panic(const char *msg)
{
    perror(msg);
    abort();
}

