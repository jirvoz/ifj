#ifndef _STATEMENTS_H_
#define _STATEMENTS_H_

#include <stdbool.h>

// Parse variable definition
bool dim_stat();

// Parse assignment
bool assignment_stat();

// Parse input command
bool input_stat();

// Parse print command
bool print_stat();

#endif
