
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

/** \file string.c
 * \brief String Words
 */

#include "compiler.h"

/**
 */
void
string_words() {
    Colon( "/string" );
          c("tuck"); c("-"); c("-rot"); c("+"); c("swap"); End();

    Colon( "(\")" );
          c("ip>"); c("count"); c("2dup"); c("+"); c("talign"); c(">ip");
          End();
    Colon( "(.\")" );
          c("ip>"); c("count"); c("2dup"); c("+"); c("talign"); c(">ip");
          c("type"); End();

    Colon( "strlen" );  /* ( adr -- n ) */
          c("dup");
          Begin(); c("dup"); c("c@"); While(); c("1+"); Repeat();
          c("swap"); c("-"); End();
    Colon( "(cstr\")" );  /* ( -- adr ) */
          c("ip>");
          c("dup"); c("strlen"); c("over"); c("+"); c("talign");
          c(">ip"); End();
    /* Need to add place-cstring and cstr" */

    Colon( "ascii?" );   /* ( c -- flag ) */
          Literal((Cell)'!'); Literal((Cell)0x7f); c("within"); End();
    Colon( "$ascii?" );  /* ( adr len -- flag ) */
          c("true"); c("-rot");
          c("bounds"); Do();
             c("i"); c("c@"); c("ascii?"); c("and");
          Loop(); End();

    /* Scan the string adr,len looking for c and return the number of
     * characters into the string that c was found.
     */
    Colon( "index" );   /* ( adr len c -- n ) */
          c("0"); c(">r");
          Begin();
             c("over"); c("0>"); While();
             c(">r"); c("over"); c("c@");
             c("r>"); c("tuck"); c("<>"); While();
                c("r>"); c("1+"); c(">r");
                c("-rot"); c("1"); c("/string"); c("rot");
          Repeat(); Then();
          c("3drop"); c("r>");
          End();

    Colon( "skip" );   /* ( adr len c -- adr' len' ) */
          c(">r"); c("tuck");
          Begin(); c("dup"); While();
             c("over"); c("c@"); c("r@"); c("="); While();
                c("1"); c("/string");
          Repeat(); Then();
          c("rot"); c("over"); c("-"); c("r>"); c("drop");
          End();

    Colon( "$." );
          c("count"); c("type"); End();

    /* This is here to chop off the newline from the end of a string
       -- inspiration from Perl */
    Colon( "chop" );  c("1-"); End();
}

/*
 * vim:autoindent
 * vim:expandtab
 */
