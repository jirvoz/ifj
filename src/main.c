#include <stdio.h>
#include <stdlib.h>

#include "errors.h"
#include "parser.h"
#include "scanner.h"

int main ()
{
    if (parse())
       return 0;
    else
        // TODO return correct error code
        return 1;
}
