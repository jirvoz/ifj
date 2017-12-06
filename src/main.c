//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        main.c
//  Description: Source file of main module
//               There is main() function, which runs parser
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

#include "errors.h"
#include "parser.h"

int main ()
{
    // Run parser
    parse();

    // Return code of first error (from errors.h)
    return exit_code;
}
