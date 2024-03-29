//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        statements.h
//  Description: Header file of statement parsing
//               Rules for parsing statements
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

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

// Parse if block
bool if_stat();

// Parse do while block
bool while_stat();

// Parse return
bool return_stat();

#endif
