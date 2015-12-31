
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

/** \file format.c
 * \brief Number formatting words
 */

#include "compiler.h"
#include "kernel.h"
#include "controlflow.h"

/**
 */
void format_words() {
   Variable( "#hold" );
   Variable( "'hold" );
   Constant( "/hold", (Cell)80 );

   ColdChain( "Allocating memory for hold area." );
           c("/hold"); c("allocate"); If();
             DotQuote("Couldn't allocate memory for hold area.");
             c("cr"); c("exit"); /* c("bye"); bye isn't defined yet */
          Then(); c("'hold"); c("!"); End();

   Colon( "<#" );   /* ( -- ) */
          c("'hold"); c("@"); c("/hold"); c("blank");
          c("/hold"); c("#hold"); c("!");
          End();
   Colon( "#>" );   /* ( d -- adr len ) */
          c("2drop");
          c("'hold"); c("@"); c("#hold"); c("@"); c("+");
          c("/hold"); c("#hold"); c("@"); c("-");
          End();
   Colon( "hold" );   /* ( c -- ) */
          c("#hold"); c("dup"); c("@"); c("1-"); c("swap"); c("!");
          c("'hold"); c("@"); c("#hold"); c("@"); c("+"); c("c!");
          End();

   Colon( ">digit" );   /* ( n -- c ) */
          Literal((Cell)'0'); c("+");
          c("dup"); Literal((Cell)'9'); c(">"); If();
             Literal((Cell) ('a'-1) - '9'); c("+");
          Then(); End();

   /* See page 105 Kelly/spies for a description of # */
   Colon( "#" );   /* ( -- ) */
          c("base"); c("@");
          c("s>d"); c("d/mod"); c("rot"); c("drop");
          c("rot"); c(">digit"); c("hold");
          End();

   Colon( "#s" );   /* ( d -- d' ) */
          Begin(); c("#"); c("2dup"); c("d0="); Until(); End();

   Colon( "sign" );
          c("0<"); If(); Literal('-'); c("hold"); Then(); End();
   Colon( "<s#" ); c("tuck"); c("dabs"); c("<#"); End();
   Colon( "ssign" ); c("rot"); c("sign"); End();

   Colon( "(ud.)" ); c("<#"); c("#s"); c("#>"); End();
   Colon( "(d.)" );
          c("<s#"); c("#s"); c("ssign"); c("#>"); End();
   Colon( "(u.)" ); c("0"); c("(ud.)"); End();
   Colon( "(.)" ); c("s>d"); c("(d.)"); End();

   Colon( "ud." ); c("(ud.)"); c("type"); c("space"); End();
   Colon( "d." ); c("(d.)");  c("type"); c("space"); End();
   Colon( "u." ); c("(u.)");  c("type"); c("space"); End();
   Colon( "." ); c("(.)");   c("type"); c("space"); End();
   Colon( ".d" );
          c("base("); c("decimal"); c("."); c(")base"); End();
   Colon( "u.r" );
         c("swap"); c("(u.)"); c("rot"); c("over"); c("-"); c("spaces");
         c("type"); End();
   Colon( "0s" );
          c("dup"); c("0<="); If(); c("drop"); c("exit"); Then();
          c("0"); Do(); Literal((Cell)'0'); c("emit"); Loop(); End();
   Colon( "u.r0" );
         c("swap"); c("(u.)"); c("rot"); c("over"); c("-"); c("0s");
         c("type"); End();

}

/*
 * vim:autoindent
 * vim:expandtab
 */
