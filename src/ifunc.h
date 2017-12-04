//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        ifunc.h
//  Description: Header file of inbuild functions module
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

#ifndef _IFUNC_H_
#define _IFUNC_H_

// Convert character in string to number from ascii table
bool callAsc();
// Convert number from ascii table to character
bool callChr();
// Return length of string
bool callLength();
// Return required substring
bool callSubstr();

#endif
