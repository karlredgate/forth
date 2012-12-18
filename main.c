
/*
 * Copyright (c) 2010-2012 Karl N. Redgate
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/** \file main.c
 * \brief Wrapper to start the VM.
 */

#include <strings.h>
#include "compiler.h"

#define DEF_DICTSIZE 0x10000  /* 64k */

extern void init_support();
extern void init_dictionary(int);
extern void StartVM();

int main( int argc, char **argv, char **envp ) {
   int dictsize = DEF_DICTSIZE;  /* pass alternate dict size as an arg */

   init_support();

   /* FIX - change to loop around evaluate - this also means move
      below init_dictionary */
/* debug is a define for user area */
   /* if ( (argc > 1) && strcmp( *(argv+1), "-d" ) == 0 ) debug = 1; */

   init_dictionary( dictsize );
   StartVM( "cold", argc, argv );
}

/*
 * vim:autoindent
 * vim:expandtab
 */
