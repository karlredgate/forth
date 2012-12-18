
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

/** \file implementation.c
 * \brief Implementation Support Words
 */

#include <stdlib.h>
#include <stdio.h>

/* either the readline stuff can be moved to main.c or we can make
   the rl_* functions forth primitives */

#include "linenoise.h"
#include "compiler.h"

static char *history_filename = ".forth_history";
static int history_offset;

/* Can this be made into a word which can be called as part of cold ?? */

/* FIX - I bet the SEGV problem is related to malloc and so ...
   maybe if we move the rl_init.. etc to separate words and make
   them part of "cold" it will fix it.  Or better yet, make my
   own memory management using sbrk. */

/**
 */
void init_support() {
    linenoiseHistoryLoad( history_filename );
}

/**
 */
static void
bye() {
    linenoiseHistorySave( history_filename );
   /* fprintf( stderr, "Writing history file ... " ); */
   /* system( "stty cooked" ); */
   printf( "\n" );
   /* fprintf( stderr, "done.\n" ); */
   exit( 0 );
}

/**
 */
void
implementation_words() {
   Primitive( "bye",   &bye );

   Colon( "codefield@" );  /* ( xt -- codefield ) */  c("@"); End();
   Colon( "ndrop" );  /* ( n1 ... nN N -- ) */
          c("dup"); c("0<"); If(); c("exit"); Then();
          Begin(); c("dup"); While(); c("nip"); c("1-"); Repeat();
          c("drop");
          End();

   Colon( "name>$" );  /* ( anf -- adr len ) */
          c("dup"); c("c@");
          Literal((Cell)0x1f); c("and");
          c("dup"); c("under-"); End();

   Colon( ">xt$" );  /* ( xt -- adr len ) */
          c("s>d"); c("<#");
          Literal((Cell)')'); c("hold");
          c("#s");
          Literal((Cell)'('); c("hold");
          c("#>"); End();

   Colon( ">name$" );  /* ( xt -- adr len ) */
          c("dup"); c(">name"); c("name>$");
          c("2dup"); c("$ascii?"); If();
             c("rot"); c("drop"); c("exit");
          Then();
          c("2drop"); c(">xt$"); End();

   Colon( "variable?" );  /* ( xt -- flag ) */
          c("codefield@"); c("do-variable"); c("="); End();

   Colon( "user@" ); /* ( xt -- n ) */ c(">body"); c(">user"); c("@"); End();
}

/*
 * vim:autoindent
 * vim:expandtab
 */
