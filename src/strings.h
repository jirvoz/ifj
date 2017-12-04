//  Course:      Formal Languages and Compilers (IFJ)
//  Project:     Implementation of the IFJ17 imperative language compiler
//  File:        strings.h
//  Description: Header file of String module
//
//  Authors: Tomáš Nereča : xnerec00
//           Samuel Obuch : xobuch00
//           Jiří Vozár   : xvozar04
//           Ján Farský   : xfarsk00

#ifndef _STRINGS_H_
#define _STRINGS_H_

// Data structure representing string 
typedef struct
{
    char* str;            // Loaded string ended with the '\0'
    int length;           // Length of string
    int allocatedSize;    // Size of allocated memory for string
} string;

// Initialize new str, allocated size is STR_LEN_INC
int stringInit(string* str_1);
// Free the str allocated memory
void stringFree(string* str_1);
// Clear content of the str
void stringClear(string* str_1);

// Add new char to the end of str
int stringAddChar(char c, string* str_1);

// Allocate memmory and concatenate string
int stringConcat(const char* str_2, string* str_1);

#endif
