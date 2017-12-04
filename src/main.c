#include "errors.h"
#include "parser.h"

int main ()
{
    // Run parser
    parse();

    // Return code of first error (from errors.h)
    return exit_code;
}
