#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "parser.h"
#include "scanner.h"

int main ()
{
    // Run parser
    parse();

    // Return code of first error (from errors.h)
    return exit_code;
}
