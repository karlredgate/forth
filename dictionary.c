
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

/** \file dictionary.c
 * \brief Dictionary Modification/Traversal words
 */

#include "compiler.h"

/**
 */
void compile_dictionary_words() {
    Colon( "allot" );  /* ( n -- ) */
          c("dp"); c("+!"); End();
    Colon( "," );    /* ( n -- ) */
          c("here"); c("!"); c("/cell"); c("allot"); End();

    Colon( "compile," );  /* ( xt -- ) */
          /* c("origin-"); */ c(","); End();
    Colon( "compile" );   /* ( -- ) */
          c("ip>"); c("dup"); c("cell+"); c(">ip");
          c("@"); c("compile,"); End();

    /* FIX - Probably want lit and literal defined earlier */
    Colon( "lit" ); c("ip>"); c("dup"); c("cell+"); c(">ip");
          c("@"); End();
    Colon( "literal" ); c("compile"); c("lit"); c(","); End();

    /* From page 296 - Kelly/Spies */
    /* Maybe redefine in terms of ta1+... */
    Colon( ">body" );  /* ( acf -- apf ) */
          Literal((Cell)sizeof(ACF)); c("+"); End();
    Colon( ">link" );  /* ( acf -- alf ) */
          Literal((Cell)sizeof(ALF)); c("-"); End();
    Colon( "l>name" );  /* ( alf -- anf ) */
          c("1-"); End();
    Colon( "n>link" );  /* ( alf -- anf ) */
          c("1+"); End();
    Colon( ">name" );  /* ( acf -- anf ) */
          c(">link"); c("l>name"); End();
    Colon( "body>" );  /* ( apf -- acf ) */
          Literal((Cell)sizeof(ACF)); c("-"); End();
    Colon( "link>" );  /* ( alf -- acf ) */
          Literal((Cell)sizeof(ALF)); c("+"); End();
    Colon( "name>" );  /* ( anf -- acf ) */
          c("n>link"); c("link>"); End();
}

/*
 * vim:autoindent
 * vim:expandtab
 */
